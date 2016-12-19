Commissioning
=============

This folder contains resources for setting up new Hail boards.

Initial software installation
-----------------------------

The `commission_new_hail.sh` script sets the FTDI device name, flashes the
bootloader on the SAM4L, and loads the BLE serialization code onto the
nRF51822. The script also sets the BLE address for the device, which is also
written as the device serial number to the FTDI chip.

To initialize a Hail board:

 * Connect USB to the board
 * Connect a JTAG programmer to the SAM4L
 * Run `./commission_new_hail.sh c098e513XXXX` where the `XXXX` should be
   replaced with a unique device ID
 * Connect a JTAG programmer to the nRF51822
 * Hit `any key` to continue the script

Be sure to label the board with a sticker so that the user knows what the
device's BLE address is.


`ftx_prog`
----------

The `ftx_prog` executable comes from here:
https://github.com/richardeoin/ftx-prog.
It was built for linux amd64, so if the executable doesn't work for you,
you will have to recompile it for your platform.
