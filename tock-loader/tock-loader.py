#!/usr/bin/env python3

import struct
import sys
import time

import serial



port = '/dev/ttyUSB0'


RESPONSE_PONG = 0x11


sp = serial.Serial(port=port, baudrate=115200, bytesize=8, parity=serial.PARITY_NONE, stopbits=1, xonxoff=0, rtscts=0, timeout=0.5)

# Enter bootloader mode

# Reset the SAM4L
sp.setDTR(1)

# Set RTS to make the SAM4L go into bootloader mode
sp.setRTS(1)

time.sleep(0.1)

# Let the SAM4L startup
sp.setDTR(0)

# Wait for 100 ms to make sure the bootloader enters bootloader mode
time.sleep(0.5)
sp.setRTS(0)


# Returns -1 on failure, 0 on success
def ping_bootloader_and_wait_for_response ():
	for i in range(30):
		# Try to ping the SAM4L to ensure it is in bootloader mode
		ping_pkt = bytes([0xFC, 0x01])
		sp.write(ping_pkt)

		ret = sp.read(2)

		if len(ret) == 2 and ret[1] == RESPONSE_PONG:
			return 0
	return -1


ret = ping_bootloader_and_wait_for_response()
if ret < 0:
	print('Error connecting to bootloader. No "pong" received.')
	sys.exit(1)



binary_filename = sys.argv[1]
print(binary_filename)


with open(binary_filename, 'rb') as f:
	img = f.read()

# Append trailing zeros so we know where the end of valid apps are
img += bytes([0]*8)

# print(len(img))

# Fill in the binary with 0xFFs so that it is a multiple of 512
if len(img) % 512 != 0:
	remaining = 512 - (len(img) % 512)
	print(remaining)
	img += bytes([0xFF]*remaining)
# print(img)
# print(len(img))

address = 0x30000

for i in range(len(img) // 512):

	# i = 0

	#sync string
	sp.write(bytes([0, 0xFC, 0x05]))

	pkt = struct.pack('<I', address)
	pkt += img[i*512: (i+1)*512]

	# Escape any bytes that need to be escaped
	pkt = pkt.replace(bytes([0xFC]), bytes([0xFC, 0xFC]))

	# Add the ending escape that ends the packet and the command byte
	pkt += bytes([0xFC, 0x07])

	print(pkt)
	print(len(pkt))

	sp.write(pkt)
	ret = sp.read(2)
	print(ret)

	address += 512




# Reset the SAM4L
sp.setDTR(1)

sp.setRTS(0)

time.sleep(0.1)

# Let the SAM4L startup
sp.setDTR(0)



