#!/usr/bin/env bash

me=`basename "$0"`

if [[ $# -ne 1 ]]; then
	echo "You must supply the Hail ID as the argument to this script."
	echo "Example: $me c098e5130001"
	exit 1
fi


echo "Configuring this Hail with ID: $1"
echo ""

echo "Configuring the FTDI with Hail parameters..."
./ftx_prog --manufacturer Lab11 --product "Hail IoT Module - TockOS" --new-serial-number $1 > /dev/null
rc=$?;
if [[ $rc != 22 ]]; then
	echo "Error programming FTDI"
	exit 2
fi
echo "done"

echo "Flashing the bootloader using JTAG..."
pushd ../bootloader/build
make flash-bootloader
popd > /dev/null
echo "done"


echo ""
echo "Now attempting to flash the nRF51822."
echo "This requires moving the JTAG."
echo "Move the tag connect header to the bottom of the board."
read -n1 -r -p "Press any key to continue..." key
echo ""

pwd

