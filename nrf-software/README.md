nrf-software
============

This directory holds applications for the nRF51822 on Hail as well as the
Nordic softdevices, SDKs, and several Lab11 libraries for using the nRF.

`nrf5x-base` is a submodule of the
[nrf5x-base](https://github.com/lab11/nrf5x-base) repository. If it is not
already checked out, you can do so with the command:

```
git submodule update --init --recursive
```

The `apps` folder contains applications that can be run on the nRF51822. The
base application installed on Hail boards is `hail-radio-serialization`.
Instructions for compiling and flashing an application can be found in the
[nrf5x-base readme](https://github.com/lab11/nrf5x-base#program-a-nrf51822).

