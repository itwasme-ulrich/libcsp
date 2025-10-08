/*
 * Interface: CAN0 @ 500.000
 * EXP-SATELLITE ESAT93
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <sys/sysinfo.h>

#include <csp/csp.h>
#include <csp/arch/csp_thread.h>
#include <csp/drivers/can_socketcan.h>
/* ========== HOST ========== */
#define OBC_ADDRESS            1       // OBC server address

/* ========== EXP-SAT ========== */
#define EXP_ADDRESS            2       
#define CAN_INTERFACE          "can0"
#define CAN_BITRATE            500000

/* ========== CSP Standard Ports ========== */
#define CSP_CMD                0
#define CSP_PING               1
#define CSP_PS                 2
#define CSP_MEM_FREE           3
#define CSP_REBOOT             4
#define CSP_BUF_FREE           5
#define CSP_UPTIME             6

/* ========== BEE-PROJECT Ports ========== */
#define BEE_PARAMS             7 

/* ========== Global Variables ========== */
static csp_iface_t *can_iface = NULL;
static uint32_t boot_time = 0;
static uint32_t request_count = 0;

/* ========== Helper Functions ========== */

/**
 * Get system uptime in seconds
 */
uint32_t get_uptime(void) {
    struct sysinfo info;
    if (sysinfo(&info) == 0) {
        return (uint32_t)info.uptime;
    }
    return 0;
}

/**
 * Get free memory
 */
void get_memory_info(uint32_t *total, uint32_t *free_mem) {
    struct sysinfo info;
    if (sysinfo(&info) == 0) {
        *total = info.totalram * info.mem_unit;
        *free_mem = info.freeram * info.mem_unit;
    } else {
        *total = 0;
        *free_mem = 0;
    }
}

/* ========== Port Handlers ========== */

/**
 * Handler for CSP_CMD (Port 0)
 */
void handle_cmd(csp_conn_t *conn, csp_packet_t *packet) {
    printf("[PORT 0 - CMD] Received command request from node %u\n", csp_conn_src(conn));
    
    // Prepare response
    const char *response = "CMD_OK";
    snprintf((char *)packet->data, csp_buffer_data_size(), "%s", response);
    packet->length = strlen(response) + 1;
    
    if (!csp_send(conn, packet, 1000)) {
        csp_log_error("[CMD] Failed to send response");
        csp_buffer_free(packet);
    } else {
        printf("[CMD] Sent response: %s\n", response);
    }
}

/**
 * Handler for CSP_PING (Port 1) - Handled by CSP ?
 */

void handle_ping(csp_conn_t *conn, csp_packet_t *packet) {
    printf("[PORT 1 - PING] Received PING request from node %u\n", csp_conn_src(conn));
    
    // Send as ASCII string for testing
    snprintf((char *)packet->data, csp_buffer_data_size(), 
             "200 OK");
    packet->length = strlen((char *)packet->data) + 1;
    
    if (!csp_send(conn, packet, 1000)) {
        csp_log_error("[PING] Failed to send response");
        csp_buffer_free(packet);
    } else {
        printf("[PING] Sent response: %s\n", (char *)packet->data);
    }
}

/**
 * Handler for CSP_PS (Port 2) - Process list
 */
void handle_ps(csp_conn_t *conn, csp_packet_t *packet) {
    printf("[PORT 2 - PS] Received PS request from node %u\n", csp_conn_src(conn));
    
    // Prepare response with process info
    const char *response = "PS_INFO";
    snprintf((char *)packet->data, csp_buffer_data_size(), 
             "%s: PID=1234 CPU=5%% MEM=10MB", response);
    packet->length = strlen((char *)packet->data) + 1;
    
    if (!csp_send(conn, packet, 1000)) {
        csp_log_error("[PS] Failed to send response");
        csp_buffer_free(packet);
    } else {
        printf("[PS] Sent response: %s\n", (char *)packet->data);
    }
}

/**
 * Handler for CSP_MEM_FREE (Port 3) - Free memory
 */
void handle_memfree(csp_conn_t *conn, csp_packet_t *packet) {
    printf("[PORT 3 - MEMFREE] Received MEMFREE request from node %u\n", csp_conn_src(conn));
    
    uint32_t total, free_mem;
    get_memory_info(&total, &free_mem);
    
    // Send as ASCII string for testing
    snprintf((char *)packet->data, csp_buffer_data_size(), 
             "MEMFREE: Total=%u Free=%u", total, free_mem);
    packet->length = strlen((char *)packet->data) + 1;
    
    if (!csp_send(conn, packet, 1000)) {
        csp_log_error("[MEMFREE] Failed to send response");
        csp_buffer_free(packet);
    } else {
        printf("[MEMFREE] Sent response: %s\n", (char *)packet->data);
    }
}

/**
 * Handler for CSP_REBOOT (Port 4) - Reboot command
 */
void handle_reboot(csp_conn_t *conn, csp_packet_t *packet) {
    printf("[PORT 4 - REBOOT] Received REBOOT request from node %u\n", csp_conn_src(conn));
    
    const char *response = "REBOOT_ACK";
    snprintf((char *)packet->data, csp_buffer_data_size(), "%s", response);
    packet->length = strlen(response) + 1;
    
    if (!csp_send(conn, packet, 1000)) {
        csp_log_error("[REBOOT] Failed to send response");
        csp_buffer_free(packet);
    } else {
        printf("[REBOOT] Sent response: %s\n", response);
    }
}

/**
 * Handler for CSP_BUF_FREE (Port 5) - Buffer free info
 */
void handle_buffree(csp_conn_t *conn, csp_packet_t *packet) {
    printf("[PORT 5 - BUFFREE] Received BUFFREE request from node %u\n", csp_conn_src(conn));
    
    uint32_t free_buffers = csp_buffer_remaining();
    
    snprintf((char *)packet->data, csp_buffer_data_size(), 
             "BUFFREE: %u", free_buffers);
    packet->length = strlen((char *)packet->data) + 1;
    
    if (!csp_send(conn, packet, 1000)) {
        csp_log_error("[BUFFREE] Failed to send response");
        csp_buffer_free(packet);
    } else {
        printf("[BUFFREE] Sent response: %s\n", (char *)packet->data);
    }
}

/**
 * Handler for CSP_UPTIME (Port 6) - System uptime
 */
void handle_uptime(csp_conn_t *conn, csp_packet_t *packet) {
    printf("[PORT 6 - UPTIME] Received UPTIME request from node %u\n", csp_conn_src(conn));
    
    uint32_t uptime = get_uptime();
    uint32_t days = uptime / 86400;
    uint32_t hours = (uptime % 86400) / 3600;
    uint32_t minutes = (uptime % 3600) / 60;
    
    snprintf((char *)packet->data, csp_buffer_data_size(), 
             "UPTIME: %ud %uh %um (%u seconds)", days, hours, minutes, uptime);
    packet->length = strlen((char *)packet->data) + 1;
    
    if (!csp_send(conn, packet, 1000)) {
        csp_log_error("[UPTIME] Failed to send response");
        csp_buffer_free(packet);
    } else {
        printf("[UPTIME] Sent response: %s\n", (char *)packet->data);
    }
}

/**
 * Handler for BEE_PARAMS (Port 7)
 */
void handle_bee_params(csp_conn_t *conn, csp_packet_t *packet) {
    printf("[PORT 7 - BEE_PARAMS] Received BEE_PARAMS request from node %u\n", csp_conn_src(conn));
    
    if (packet->length > 0) {
        printf("[BEE_PARAMS] Payload from client (%u bytes): %.*s\n",
               packet->length, packet->length, (char *)packet->data);
    } else {
        printf("[BEE_PARAMS] No payload received from client.\n");
    }

    snprintf((char *)packet->data, csp_buffer_data_size(),
             "BEE_PARAMS: Voltage=3.3V Current=150mA Temp=25C Status=OK Packets=%u",
             request_count);
    packet->length = strlen((char *)packet->data) + 1;
    
    if (!csp_send(conn, packet, 1000)) {
        csp_log_error("[BEE_PARAMS] Failed to send response");
        csp_buffer_free(packet);
    } else {
        printf("[BEE_PARAMS] Sent response: %s\n", (char *)packet->data);
    }
}


/* ========== Server Task ========== */
CSP_DEFINE_TASK(task_server) {
    
    csp_log_info("Server task started on address %u", EXP_ADDRESS);
    
    /* Create socket */
    csp_socket_t *sock = csp_socket(CSP_SO_NONE);
    if (sock == NULL) {
        csp_log_error("Failed to create socket");
        return CSP_TASK_RETURN;
    }
    
    /* Bind to all ports */
    csp_bind(sock, CSP_ANY);
    
    /* Create backlog */
    csp_listen(sock, 10);
    
    printf("\n");
    printf("=======================================\n");
    printf("  Server listening on all ports\n");
    printf("  Ready to accept connections...\n");
    printf("=======================================\n\n");
    
    /* Main server loop */
    while (1) {
        
        /* Wait for connection */
        csp_conn_t *conn = csp_accept(sock, 10000);
        if (conn == NULL) {
            continue;
        }
        
        printf("\n[CONNECTION] New connection from node %u\n", csp_conn_src(conn));
        
        /* Read packets on this connection */
        csp_packet_t *packet;
        while ((packet = csp_read(conn, 100)) != NULL) {
            
            uint8_t port = csp_conn_dport(conn);
            request_count++;
            
            printf("[PACKET] Received on port %u, length %u bytes\n", port, packet->length);
            
            switch (port) {
                
                case CSP_CMD:
                    handle_cmd(conn, packet);
                    break;

                case CSP_PING:
                    handle_ping(conn, packet);
                    break;

                case CSP_REBOOT:
                    handle_reboot(conn, packet);
                    break;

                case CSP_BUF_FREE:
                    csp_service_handler(conn, packet);
                    break;
                    
                case CSP_PS:
                    handle_ps(conn, packet);
                    break;
                    
                case CSP_MEM_FREE:
                    handle_memfree(conn, packet);
                    break;
                    
                case CSP_UPTIME:
                    handle_uptime(conn, packet);
                    break;
                    
                case BEE_PARAMS:
                    handle_bee_params(conn, packet);
                    break;
                    
                default:
                    printf("[WARNING] Unknown port %u, using default handler\n", port);
                    csp_service_handler(conn, packet);
                    break;
            }
        }
        
        /* Close connection */
        csp_close(conn);
        printf("[CONNECTION] Connection closed\n");
    }
    
    return CSP_TASK_RETURN;
}

/* ========== Statistics Task ========== */
CSP_DEFINE_TASK(task_stats) {
    
    while (1) {
        csp_sleep_ms(10000);
        
        printf("\n--- Statistics ---\n");
        printf("Total requests handled: %u\n", request_count);
        printf("Free buffers: %u\n", csp_buffer_remaining());
        printf("Uptime: %u seconds\n", get_uptime());
        printf("------------------\n\n");
    }
    
    return CSP_TASK_RETURN;
}

/* ========== Main Function ========== */
int main(int argc, char *argv[]) {
    
    int error;
    uint8_t my_address = EXP_ADDRESS;
    csp_debug_level_t debug_level = CSP_INFO;
    bool show_stats = false;
    
    /* Parse command line arguments */
    int opt;
    while ((opt = getopt(argc, argv, "a:d:sh")) != -1) {
        switch (opt) {
            case 'a':
                my_address = atoi(optarg);
                break;
            case 'd':
                debug_level = atoi(optarg);
                break;
            case 's':
                show_stats = true;
                break;
            case 'h':
            default:
                printf("BEE Project - CSP Server\n");
                printf("Usage: %s [options]\n", argv[0]);
                printf("Options:\n");
                printf("  -a <address>     My CSP address (default: %d)\n", EXP_ADDRESS);
                printf("  -d <level>       Debug level 0-6 (default: 4=INFO)\n");
                printf("  -s               Show periodic statistics\n");
                printf("  -h               Show this help\n");
                printf("\nAvailable Ports:\n");
                printf("  Port 0: CSP_CMD\n");
                printf("  Port 1: CSP_PING\n");
                printf("  Port 2: CSP_PS\n");
                printf("  Port 3: CSP_MEM_FREE\n");
                printf("  Port 4: CSP_REBOOT\n");
                printf("  Port 5: CSP_BUF_FREE\n");
                printf("  Port 6: CSP_UPTIME\n");
                printf("  Port 7: BEE_PARAMS (custom)\n");
                printf("\nExample:\n");
                printf("  %s -a 1 -d 5 -s\n", argv[0]);
                printf("\nSetup CAN first:\n");
                printf("  sudo ip link set can0 type can bitrate 500000\n");
                printf("  sudo ip link set can0 up\n");
                exit(opt == 'h' ? 0 : 1);
        }
    }
    
    /* Set debug levels */
    for (csp_debug_level_t i = 0; i <= CSP_LOCK; i++) {
        csp_debug_set_level(i, (i <= debug_level) ? true : false);
    }
    
    boot_time = (uint32_t)time(NULL);
    
    printf("\n");
    printf("=======================================\n");
    printf("  BEE Project - CSP Server\n");
    printf("=======================================\n");
    printf("Node Address: %u\n", my_address);
    printf("CAN Interface: %s @ %d bps\n", CAN_INTERFACE, CAN_BITRATE);
    printf("Debug Level: %d\n", debug_level);
    printf("=======================================\n\n");
    
    /* Initialize CSP */
    csp_log_info("Initializing CSP...");
    csp_conf_t csp_conf;
    csp_conf_get_defaults(&csp_conf);
    csp_conf.address = my_address;
    
    error = csp_init(&csp_conf);
    if (error != CSP_ERR_NONE) {
        csp_log_error("csp_init() failed, error: %d", error);
        return 1;
    }
    
    /* Start router task */
    csp_log_info("Starting router task...");
    csp_route_start_task(500, 0);
    
    /* Add CAN interface */
    csp_log_info("Adding CAN interface: %s", CAN_INTERFACE);
    error = csp_can_socketcan_open_and_add_interface(
        CAN_INTERFACE,
        CSP_IF_CAN_DEFAULT_NAME,
        0,      // node_id (0 = use CSP address)
        false,  // promisc mode
        &can_iface
    );
    
    if (error != CSP_ERR_NONE) {
        csp_log_error("Failed to add CAN interface [%s], error: %d", CAN_INTERFACE, error);
        printf("\n");
        printf("ERROR: Cannot initialize CAN interface!\n");
        printf("Please setup CAN first:\n");
        printf("  sudo ip link set can0 type can bitrate 500000\n");
        printf("  sudo ip link set can0 up\n");
        printf("  ip link show can0\n");
        return 1;
    }
    
    /* Set default route */
    csp_rtable_set(CSP_DEFAULT_ROUTE, 0, can_iface, CSP_NO_VIA_ADDRESS);
    
    /* Print routing info */
    printf("\nConnection table:\n");
    csp_conn_print_table();
    printf("\nInterfaces:\n");
    csp_route_print_interfaces();
    printf("\nRoute table:\n");
    csp_route_print_table();
    printf("\n");
    
    csp_log_info("CSP initialized successfully!");
    
    /* Start server task */
    csp_thread_handle_t server_handle;
    csp_thread_create(task_server, "SERVER", 2000, NULL, 0, &server_handle);
    
    /* Start statistics task if requested */
    if (show_stats) {
        csp_thread_handle_t stats_handle;
        csp_thread_create(task_stats, "STATS", 1000, NULL, 0, &stats_handle);
    }
    
    /* Keep running */
    printf("\nServer is running. Press Ctrl+C to stop.\n\n");
    
    while (1) {
        csp_sleep_ms(1000);
    }
    
    return 0;
}
