# LIBVCSP Change Log - Fork libcsp v2.1  
Every change merged into `v2.1-vinspace`, newest first.
See [VCSP.md](VCSP.md) 

## Unreleased
| Commit | Area | Source | Detail | 
|---|---|---|---| 
| #3 | csp_id | upstream 0744f49 + own C99 fix | Outgoing source port check never fired; now checked at build time, compiles with C99 competible |
| #2 | rtable | upstream 5f8f0d0 | Insert index clamped one past the end of the route table when full |
| #1 | service | upstream e4000a2 | CSP_PS freed the packet then replied with it: `rps` corrupted the buffer pool |



## Base 
- libcsp `v2.1`, `48f7fb0b57f610bf65bab1aa2d1357c3b9722782`.  