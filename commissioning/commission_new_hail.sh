#!/usr/bin/env bash

me=`basename "$0"`

if [[ $# -ne 1 ]]; then
	echo "You must supply the Hail ID as the argument to this script."
	echo "Example: $me c098e5130001"
	echo ""
	echo "Script also expects JTAG attached to the SAM4L to start."
	exit 1
fi

idnocolon=$1
idcolon=`echo $1 | sed 's/\(..\)/\1:/g;s/:$//'`


echo "Configuring this Hail with ID: $1"
echo ""

echo "Configuring the FTDI with Hail parameters..."
./ftx_prog --manufacturer Lab11 --product "Hail IoT Module - TockOS" --new-serial-number $idnocolon > /dev/null
rc=$?;
if [[ $rc != 22 ]]; then
	echo "Error programming FTDI"
	exit 2
fi
echo "done"

echo "Flashing the bootloader using JTAG..."
tockloader flash hail_bootloader.bin --address 0 --board hail --jtag
echo "done"

echo "Setting the correct board attribute..."
tockloader set-attribute --jtag --board hail board hail
echo "done"

echo "Ensuring the submodule for Tock is checked out"
git submodule update --init --recursive tock

echo "Flashing the hail kernel"
pushd tock/boards/hail
make
tockloader flash target/sam4l/release/hail.bin --address 0x10000 --jtag --board hail
popd

echo "Flashing the hail app"
pushd tock/userland/examples/tests/hail
make
tockloader install --jtag --board hail
popd

echo "Ensuring the submodule for the nRF serialization is checked out"
git submodule update --init --recursive tock-nrf-serialization

echo ""
echo "Now attempting to flash the nRF51822."
echo "This requires moving the JTAG."
echo "Move the tag connect header to the bottom of the board."
read -n1 -r -p "Press any key to continue..." key

pushd tock-nrf-serialization/nrf51822/apps/tock-nrf51822-serialization-sdk11-s130-uart-conn
make hail flash ID=$idcolon
popd
echo "done"
