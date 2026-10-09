# LIBVCSP Change Log - Fork libcsp v2.1
Every change merged into `v2.1-vinspace`, newest first.
See [VCSP.md](VCSP.md) for the rules.

## Unreleased
| No. | PR | Area | Source | Detail |
|---|---|---|---|---|

## v2.1-vcsp.2 (2026-10-09)
| No. | Commit | Area | Source | Detail |
|---|---|---|---|---|
| 19 | #20 | route, rdp | Space Inventor fork f0b4b14 (edvard) | A plain packet with an RDP connection's identifier reached the application past RDP; an RDP packet with a plain connection's identifier made the router close the application's connection |
| 18 | #19 | freertos | upstream PR #1033 (open) 6c12344, adapted | csp_bin_sem on task notifications: the router woke whichever task last waited on a connection slot, so a task's second RDP connect failed early and a deleted task's freed TCB was written; now a binary semaphore (about 76 B more per connection) |
| 17 | #18 | can | own (upstream issue #962) | CAN reassembly with the buffer pool empty hung in csp_panic()'s while(1); now drops the frame and counts iface->drop |
| 16 | #17 | zmqhub | upstream 0abf547 (issues #952, #971) | A ZMQ frame longer than the packet buffer was copied unchecked: buffers overwritten, crash on very long frames; now dropped and counted in rx_error |
| 15 | #16 | kiss | own (upstream issue #962) | RX with the buffer pool empty hung in csp_panic()'s while(1) (default csp_panic() returns); now drops the frame and counts iface->drop |
| 14 | #15 | service | own | Service replies (ping, CMP, uptime, ...) over RDP had no RDP header; csp_service_reply() lets a connection server reply on the connection |
| 13 | #14 | csp_io | forks endurosat/csp-es a839a253, kfsw 0322607c | A request to the node's own address left with source 0, so its reply went to node 0 |
| 12 | #13 | can, eth | upstream 13037a7, aeb547a, 46bd82c (#956) | Timed-out CAN/ETH reassembly cleanup read freed buffers: list corruption, endless loop in csp_can_pbuf_free() |

## v2.1-vcsp.1 (2026-10-08)
| No. | Commit | Area | Source | Detail |
|---|---|---|---|---|
| 11 | #10 | csp_io | upstream 2626f38 | csp_accept() on a connection-less socket now returns NULL instead of misbehaving |
| 10 | #10 | route | upstream cd07298, c9efc26, addcf14, cf30f10, 8f3dee9, 3932210 (#764) | A packet for a socket bound but not yet listening crashed the router (NULL rx_queue); now dropped until csp_listen() |
| 9 | #9 | kiss | upstream issue #854 + own | A frame whose data exactly filled the packet buffer was dropped at its closing FEND |
| 8 | #8 | kiss | upstream a4ecaa5 + own | TX errors were discarded: tx_error never counted, and a packet with no room for the CRC32 was sent without one |
| 7 | #7 | rdp | upstream PR #970, issues #963 #966 | A packet shorter than the RDP header, or a short SYN, made the router read and write outside the buffer |
| 6 | #6 | rdp | upstream 192568a | OPEN connection never timed out: sender hung after a lost pass |
| 5 | #5 | rdp | upstream 2d226f8 | Scanning the shared queue moved other connections' packets to the scanning connection |
| 4 | #4 | rdp | upstream 9163081, dfbcd0f, cba1c7a | Receiver never sent a standalone ACK: uploads stalled after 6 packets |
| 3 | #3 | csp_id | upstream 0744f49 + own C99 fix | Outgoing source port check never fired; now checked at build time, compiles with C99 compatible |
| 2 | #2 | rtable | upstream 5f8f0d0 | Insert index clamped one past the end of the route table when full |
| 1 | #1 | service | upstream e4000a2 | CSP_PS freed the packet then replied with it: `rps` corrupted the buffer pool |



## Base
- libcsp `v2.1`, `48f7fb0b57f610bf65bab1aa2d1357c3b9722782`.
