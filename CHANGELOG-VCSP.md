# LIBVCSP Change Log - Fork libcsp v2.1  
Every change merged into `v2.1-vinspace`, newest first.
See [VCSP.md](VCSP.md) 

## Unreleased
| Commit | Area | Source | Detail | 
|---|---|---|---| 
| #6 | rdp | upstream 192568a | OPEN connection never timed out: sender hung after a lost pass |
| #5 | rdp | upstream 2d226f8 | Scanning the shared queue moved other connections' packets to the scanning connection |
| #4 | rdp | upstream 9163081, dfbcd0f, cba1c7a | Receiver never sent a standalone ACK: uploads stalled after 6 packets |
| #3 | csp_id | upstream 0744f49 + own C99 fix | Outgoing source port check never fired; now checked at build time, compiles with C99 competible |
| #2 | rtable | upstream 5f8f0d0 | Insert index clamped one past the end of the route table when full |
| #1 | service | upstream e4000a2 | CSP_PS freed the packet then replied with it: `rps` corrupted the buffer pool |



## Base 
- libcsp `v2.1`, `48f7fb0b57f610bf65bab1aa2d1357c3b9722782`.  