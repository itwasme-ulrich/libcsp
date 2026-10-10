#include <check.h>
#include <string.h>
#include "../include/csp/csp.h"
#include "../include/csp/csp_id.h"
#include "../include/csp/csp_iflist.h"
#include "../include/csp/csp_interface.h"
#include "../include/csp/csp_rtable.h"

static char sent[64];

static int capture_nexthop(csp_iface_t * iface, uint16_t via, csp_packet_t * packet, int from_me) {
	(void)via;
	(void)from_me;
	strcat(sent, iface->name);
	strcat(sent, " ");
	csp_buffer_free(packet);
	return CSP_ERR_NONE;
}

static csp_iface_t if_a = {.name = "A", .nexthop = capture_nexthop};
static csp_iface_t if_b = {.name = "B", .nexthop = capture_nexthop};
static csp_iface_t if_c = {.name = "C", .nexthop = capture_nexthop};

static void setup_v1(void) {
	csp_conf.version = 1;
	csp_init();
	csp_iflist_add(&if_a);
	csp_iflist_add(&if_b);
	csp_iflist_add(&if_c);
}

static const char * forward(csp_iface_t * iface, uint16_t src, uint16_t dst) {
	sent[0] = '\0';
	csp_packet_t * packet = csp_buffer_get(0);
	ck_assert_ptr_nonnull(packet);
	packet->length = 1;
	packet->data[0] = 0xAA;
	packet->id.pri = CSP_PRIO_NORM;
	packet->id.src = src;
	packet->id.dst = dst;
	packet->id.dport = 1;
	packet->id.sport = 40;
	packet->id.flags = 0;
	csp_qfifo_write(packet, iface, NULL);
	csp_route_work();
	return sent;
}

START_TEST(test_forward_one_address_netmask_zero_861)
{
	setup_v1();
	if_a.addr = 2; if_a.netmask = 0; if_a.is_default = 1;
	if_b.addr = 2; if_b.netmask = 0; if_b.is_default = 1;
	if_c.addr = 0; if_c.netmask = 0; if_c.is_default = 0;

	ck_assert_str_eq(forward(&if_a, 1, 5), "B ");
	ck_assert_str_eq(forward(&if_b, 5, 1), "A ");
	ck_assert_str_eq(forward(&if_a, 1, 2), "");
}
END_TEST

START_TEST(test_forward_one_address_netmask_host_bits_861)
{
	setup_v1();
	const int hb = csp_id_get_host_bits();
	if_a.addr = 2; if_a.netmask = hb; if_a.is_default = 1;
	if_b.addr = 2; if_b.netmask = hb; if_b.is_default = 1;
	if_c.addr = 0; if_c.netmask = 0; if_c.is_default = 0;

	ck_assert_str_eq(forward(&if_a, 1, 5), "B ");
	ck_assert_str_eq(forward(&if_b, 5, 1), "A ");
}
END_TEST

START_TEST(test_redundant_segment_no_echo_336)
{
	setup_v1();
	if_a.addr = 10; if_a.netmask = 3; if_a.is_default = 0;
	if_b.addr = 10; if_b.netmask = 3; if_b.is_default = 0;
	if_c.addr = 0; if_c.netmask = 0; if_c.is_default = 0;

	ck_assert_str_eq(forward(&if_a, 9, 8), "");
	ck_assert_str_eq(forward(&if_b, 9, 8), "");
}
END_TEST

START_TEST(test_redundant_segment_and_point_to_point)
{
	setup_v1();
	if_a.addr = 10; if_a.netmask = 3; if_a.is_default = 0;
	if_b.addr = 10; if_b.netmask = 3; if_b.is_default = 0;
	if_c.addr = 10; if_c.netmask = 0; if_c.is_default = 1;

	ck_assert_str_eq(forward(&if_a, 9, 20), "C ");
	ck_assert_str_eq(forward(&if_c, 20, 9), "A B ");
	ck_assert_str_eq(forward(&if_a, 9, 8), "");
}
END_TEST

START_TEST(test_same_interface_dropped)
{
	setup_v1();
	if_a.addr = 2; if_a.netmask = 0; if_a.is_default = 1;
	if_b.addr = 2; if_b.netmask = 0; if_b.is_default = 0;
	if_c.addr = 0; if_c.netmask = 0; if_c.is_default = 0;

	ck_assert_str_eq(forward(&if_a, 1, 9), "");
}
END_TEST

#if (CSP_USE_RTABLE)
START_TEST(test_forward_rtable_one_address_861)
{
	setup_v1();
	const int hb = csp_id_get_host_bits();
	if_a.addr = 2; if_a.netmask = 0; if_a.is_default = 0;
	if_b.addr = 2; if_b.netmask = 0; if_b.is_default = 0;
	if_c.addr = 0; if_c.netmask = 0; if_c.is_default = 0;
	ck_assert_int_eq(csp_rtable_set(5, hb, &if_b, CSP_NO_VIA_ADDRESS), CSP_ERR_NONE);
	ck_assert_int_eq(csp_rtable_set(0, 0, &if_a, CSP_NO_VIA_ADDRESS), CSP_ERR_NONE);

	ck_assert_str_eq(forward(&if_a, 1, 5), "B ");
	ck_assert_str_eq(forward(&if_b, 5, 1), "A ");
	ck_assert_str_eq(forward(&if_b, 5, 9), "A ");
	ck_assert_str_eq(forward(&if_a, 1, 9), "");
}
END_TEST
#endif

Suite * forward_suite(void) {
	Suite * s = suite_create("forward");
	TCase * tc = tcase_create("split_horizon");

	tcase_add_test(tc, test_forward_one_address_netmask_zero_861);
	tcase_add_test(tc, test_forward_one_address_netmask_host_bits_861);
	tcase_add_test(tc, test_redundant_segment_no_echo_336);
	tcase_add_test(tc, test_redundant_segment_and_point_to_point);
	tcase_add_test(tc, test_same_interface_dropped);
#if (CSP_USE_RTABLE)
	tcase_add_test(tc, test_forward_rtable_one_address_861);
#endif
	suite_add_tcase(s, tc);

	return s;
}
