#!/usr/bin/env python3

import struct
import sys
import time

import crcmod
import serial



port = '/dev/ttyUSB0'


ESCAPE_CHAR       = 0xFC

COMMAND_PING      = 0x01
COMMAND_INFO      = 0x03
COMMAND_ID        = 0x04
COMMAND_RESET     = 0x05
COMMAND_ERASE_PAGE = 0x06
COMMAND_WRITE_PAGE = 0x07
COMMAND_XEBLOCK   = 0x08
COMMAND_XWPAGE    = 0x09
COMMAND_CRCRX     = 0x10
COMMAND_RRANGE    = 0x11
COMMAND_XRRANGE   = 0x12
COMMAND_SATTR     = 0x13
COMMAND_GATTR     = 0x14
COMMAND_CRC_INTERNAL_FLASH     = 0x15
COMMAND_CRCEF     = 0x16
COMMAND_XEPAGE    = 0x17
COMMAND_XFINIT    = 0x18
COMMAND_CLKOUT    = 0x19
COMMAND_WUSER     = 0x20

RESPONSE_OVERFLOW  = 0x10
RESPONSE_PONG      = 0x11
RESPONSE_BADADDR   = 0x12
RESPONSE_INTERROR  = 0x13
RESPONSE_BADARGS   = 0x14
RESPONSE_OK        = 0x15
RESPONSE_UNKNOWN   = 0x16
RESPONSE_XFTIMEOUT = 0x17
RESPONSE_XFEPE     = 0x18
RESPONSE_CRCRX     = 0x19
RESPONSE_RRANGE    = 0x20
RESPONSE_XRRANGE   = 0x21
RESPONSE_GATTR     = 0x22
RESPONSE_CRC_INTERNAL_FLASH     = 0x23
RESPONSE_CRCXF     = 0x24
RESPONSE_INFO      = 0x25

# Tell the bootloader to reset its buffer to handle a new command.
SYNC_MESSAGE = bytes([0x00, ESCAPE_CHAR, COMMAND_RESET])




sp = serial.Serial(port=port, baudrate=115200, bytesize=8, parity=serial.PARITY_NONE, stopbits=1, xonxoff=0, rtscts=0, timeout=0.5)

# Enter bootloader mode


# Reset the chip and assert the bootloader select pin to enter bootloader
# mode.
def enter_bootloader_mode ():
	# Reset the SAM4L
	sp.setDTR(1)
	# Set RTS to make the SAM4L go into bootloader mode
	sp.setRTS(1)
	# Wait for the reset to take effect
	time.sleep(0.1)
	# Let the SAM4L startup
	sp.setDTR(0)
	# Wait for 500 ms to make sure the bootloader enters bootloader mode
	time.sleep(0.5)
	# The select line can go back high
	sp.setRTS(0)

# Reset the chip to exit bootloader mode
def exit_bootloader_mode ():
	# Reset the SAM4L
	sp.setDTR(1)
	# Make sure this line is de-asserted (high)
	sp.setRTS(0)
	# Let the reset take effect
	time.sleep(0.1)
	# Let the SAM4L startup
	sp.setDTR(0)

# Returns True if the device is there and responding, False otherwise
def ping_bootloader_and_wait_for_response ():
	for i in range(30):
		# Try to ping the SAM4L to ensure it is in bootloader mode
		ping_pkt = bytes([0xFC, 0x01])
		sp.write(ping_pkt)

		ret = sp.read(2)

		if len(ret) == 2 and ret[1] == RESPONSE_PONG:
			return True
	return False


# Tell the bootloader to save the binary blob to an address in internal
# flash.
#
# This will pad the binary as needed, so don't worry about the binary being
# a certain length.
#
# Returns False if there is an error.
def flash_binary (binary, address):
	# Enter bootloader mode to get things started
	enter_bootloader_mode();

	# Make sure the bootloader is actually active and we can talk to it.
	alive = ping_bootloader_and_wait_for_response()
	if not alive:
		print('Error connecting to bootloader. No "pong" received.')
		print('Things that could be wrong:')
		print('  - The bootloader is not flashed on the chip')
		print('  - The DTR/RTS lines are not working')
		print('  - The serial port being used is incorrect')
		print('  - The bootloader API has changed')
		print('  - There is a bug in this script')
		return False

	# Make sure the binary is a multiple of 512 bytes by padding 0xFFs
	if len(binary) % 512 != 0:
		remaining = 512 - (len(binary) % 512)
		binary += bytes([0xFF]*remaining)

	# Time the programming operation
	then = time.time()

	# Loop through the binary 512 bytes at a time until it has been flashed
	# to the chip.
	for i in range(len(img) // 512):
		# First we write the sync string to make sure the bootloader is ready
		# to receive this page of the binary.
		sp.write(SYNC_MESSAGE)

		# Now create the packet that we send to the bootloader. First four
		# bytes are the address of the page.
		pkt = struct.pack('<I', address + (i*512))

		# Next are the 512 bytes that go into the page.
		pkt += binary[i*512: (i+1)*512]

		# Escape any bytes that need to be escaped
		pkt = pkt.replace(bytes([ESCAPE_CHAR]), bytes([ESCAPE_CHAR, ESCAPE_CHAR]))

		# Add the ending escape that ends the packet and the command byte
		pkt += bytes([ESCAPE_CHAR, COMMAND_WRITE_PAGE])

		# Send this page to the bootloader
		sp.write(pkt)

		# We expect a two byte response
		ret = sp.read(2)

		# Check that we get the RESPONSE_OK return code
		if ret[0] != ESCAPE_CHAR:
			print('Error: Invalid response from bootloader when flashing page')
			return False

		if ret[1] != RESPONSE_OK:
			print('Error: Error when flashing page')
			if ret[1] == RESPONSE_BADADDR:
				print('Error: RESPONSE_BADADDR: Invalid address for page to write (address: 0x{:X}'.format(address + (i*512)))
			elif ret[1] == RESPONSE_INTERROR:
				print('Error: RESPONSE_INTERROR: Internal error when writing flash')
			elif ret[1] == RESPONSE_BADARGS:
				print('Error: RESPONSE_BADARGS: Invalid length for flash page write')
			else:
				print('Error: 0x{:X}'.format(ret[1]))
			return False

	# How long did that take
	now = time.time()
	print('Wrote {} bytes in {:0.3f} seconds'.format(len(binary), now-then))

	# Check the CRC
	# Start by doing a sync
	sp.write(SYNC_MESSAGE)
	# Generate the message to send to the bootloader
	pkt = struct.pack('<II', address, len(binary)) + bytes([ESCAPE_CHAR, COMMAND_CRC_INTERNAL_FLASH])
	sp.write(pkt)

	# Response has a two byte header, then four byte CRC
	ret = sp.read(6)

	# Check that we got a valid return code
	if ret[0] != ESCAPE_CHAR:
		print('Error: Invalid response from bootloader when asking for CRC')
		return False

	if ret[1] != RESPONSE_CRC_INTERNAL_FLASH:
		print('Error: Error when flashing page')
		if ret[1] == RESPONSE_BADADDR:
			print('Error: RESPONSE_BADADDR: Invalid address for CRC (address: 0x{:X}'.format(address))
		elif ret[1] == RESPONSE_BADARGS:
			print('Error: RESPONSE_BADARGS: Invalid length for CRC check')
		else:
			print('Error: 0x{:X}'.format(ret[1]))
		return False

	# Now interpret the last four bytes as the CRC
	crc_bootloader = struct.unpack("<I", ret[2:6])[0]

	# Calculate the CRC locally
	crc_function = crcmod.mkCrcFun(0x104c11db7, initCrc=0, xorOut=0xFFFFFFFF)
	crc_loader = crc_function(binary, 0)

	if crc_bootloader != crc_loader:
		print('Error: CRC check failed. Expected: 0x{:04x}, Got: 0x{:04x}')
		return False

	# All done, now run the application
	exit_bootloader_mode()




# ret = ping_bootloader_and_wait_for_response()
# if ret < 0:
# 	print('Error connecting to bootloader. No "pong" received.')
# 	sys.exit(1)



binary_filename = sys.argv[1]
print(binary_filename)


with open(binary_filename, 'rb') as f:
	img = f.read()

# Append trailing zeros so we know where the end of valid apps are
img += bytes([0]*8)



flash_binary(img, 0x30000)



# print(len(img))

# # Fill in the binary with 0xFFs so that it is a multiple of 512
# if len(img) % 512 != 0:
# 	remaining = 512 - (len(img) % 512)
# 	print(remaining)
# 	img += bytes([0xFF]*remaining)
# # print(img)
# # print(len(img))

# address = 0x30000

# for i in range(len(img) // 512):

# 	# i = 0

# 	#sync string
# 	sp.write(bytes([0, 0xFC, 0x05]))

# 	pkt = struct.pack('<I', address)
# 	pkt += img[i*512: (i+1)*512]

# 	# Escape any bytes that need to be escaped
# 	pkt = pkt.replace(bytes([0xFC]), bytes([0xFC, 0xFC]))

# 	# Add the ending escape that ends the packet and the command byte
# 	pkt += bytes([0xFC, 0x07])

# 	print(pkt)
# 	print(len(pkt))

# 	sp.write(pkt)
# 	ret = sp.read(2)
# 	print(ret)

# 	address += 512








