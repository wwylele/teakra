#!/usr/bin/env python3

import sys
import socket
import struct

UDP_IP = "10.42.0.241"
UDP_PORT = 8888
MESSAGE = struct.pack('<H', 0xD590)

for i in range(1, len(sys.argv)):
    MESSAGE += struct.pack('<H', int(sys.argv[i], 16))

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.sendto(MESSAGE, (UDP_IP, UDP_PORT))

