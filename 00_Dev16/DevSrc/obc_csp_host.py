#!/usr/bin/python3
import sys
import time
import libcsp_py3 as libcsp

# ==============================
#  CONFIGURATION
# ==============================
OBC_ADDRESS = 1
EXP_ADDRESS = 2
CAN_INTERFACE = "can1" 
TIMEOUT = 1000          

# ==============================
#  MENU COMMANDS
# ==============================
PORTS = {
    0: "CSP_CMD",
    1: "CSP_PING",
    2: "CSP_PS",
    3: "CSP_MEM_FREE",
    4: "CSP_REBOOT",
    5: "CSP_BUF_FREE",
    6: "CSP_UPTIME",
    7: "BEE_PARAMS"
}

def send_request(port, payload=None):
    if payload is None:
        payload = b"REQ"

    print(f"\n[INFO] Sending request to node {EXP_ADDRESS} port {port} ({PORTS.get(port, 'UNKNOWN')})")

    outbuf = bytearray(payload)
    inbuf = bytearray(256)

    try:
        result = libcsp.transaction(
            libcsp.CSP_PRIO_NORM,
            EXP_ADDRESS,
            port,
            TIMEOUT,
            outbuf,
            inbuf
        )
        print(f"[DEBUG] Transaction result: {result}")
        print(f"[DEBUG] Raw inbuf: {inbuf[:64]}")
        if result:
            reply_str = inbuf.decode(errors="ignore").strip("\x00")
            print(f"[REPLY] {reply_str}\n")
        else:
            print("[ERROR] Transaction failed or timeout.\n")
    except Exception as e:
        print(f"[EXCEPTION] {e}\n")



def main():
    print("=======================================")
    print("  OBC Interactive Menu - CSP Commander")
    print("=======================================\n")
    print(f"OBC Node: {OBC_ADDRESS}")
    print(f"Target Node (EXP): {EXP_ADDRESS}")
    print(f"Interface: {CAN_INTERFACE}\n")

    # Init CSP
    libcsp.init(OBC_ADDRESS, "OBC", "Ground", "1.0", 10, 300)
    libcsp.can_socketcan_init(CAN_INTERFACE)
    libcsp.rtable_load("0/0 CAN")
    libcsp.route_start_task()
    time.sleep(0.2)

    while True:
        print("\nAvailable Commands:")
        for port, name in PORTS.items():
            print(f"  {port}: {name}")
        print("  q: Quit")

        choice = input("\nSelect port (0-7 or q): ").strip().lower()
        if choice == 'q':
            print("Exiting...")
            break

        if not choice.isdigit() or int(choice) not in PORTS:
            print("Invalid choice, try again.")
            continue

        port = int(choice)

        # For port 7 (BEE_PARAMS)
        payload = None
        if port == 7:
            custom = input("Enter custom payload (press enter to skip): ").strip()
            if custom:
                payload = custom.encode()

        send_request(port, payload)

if __name__ == "__main__":
    main()
