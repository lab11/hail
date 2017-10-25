Commissioning
=============

This folder contains resources for setting up new Hail boards.

Commissioning Overview
----------------------

The commissioning process covers a few things:

1. Set the FTDI device name and serial number.
2. Flashes the boatloader on to the SAM4L.
3. Sets the `board` and `id` attributes in the flash of the SAM4L.
4. Flashes the `hail` TockOS kernel on the SAM4L.
5. Flashes the "hail" app on the SAM4L.
6. Flashes the serialization code on the nRF51822 and programs the ID.

Initial software installation
-----------------------------

### Pre-Requisites:

  - You need the `libftdi-dev` package (or equivalent).
  - [JLinkExe](https://www.segger.com/downloads/jlink#J-LinkSoftwareAndDocumentationPack)

-------------

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


Troubleshooting
---------------

### Permissions

    ftdi_usb_open() failed for 0403:6015: inappropriate permissions on device!

You need to add a udev rule for FTDI devices:

    sudo cp udev/99-ftdi.rules /etc/udev/rules.d/

Then unplug and replug attached FTDI devices.


### Device not found

    ftdi_usb_open() failed for 0403:6015: device not found

This seems to occasionally happen. Just run the script again.
