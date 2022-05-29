# PCSX2/HardwareTests
This is a collection of hardware tests that were used to validate the accuracy of PCSX2 

Each directory will contain a test and readme.

## What you need to know
These tests are built using the latest PS2 homebrew toolchain available [here](https://github.com/ps2dev/ps2toolchain).

These tests are written under the assumption that you are using [ps2client](https://github.com/ps2dev/ps2client) and [ps2link](https://github.com/ps2dev/ps2link/) with your real hardware.
The results for the majority of tests will be sent back to your host computer either by using the network `STDIO` (for messages), or using `host:` (for file i/o).

As of now there is no automated runner for these tests.