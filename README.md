# LanternOS
The Lantern OS is a Operating System for x86_64 UEFI device without graphics UI(but has a powerful graphics performance?)

# About the developers
The Lantern OS is made by three students in the third year of middle school from China (Our English level may not be very good, so please forgive the poor documents and code annotations:P)
Because our school work, the OS may not often update until June to July this year.

# Functions
We have completed the following functions so far:
1. Boot the system by UEFI
2. VGA I/O and serial I/O
3. Get RTC time and High-precision timer module
4. Some API from stdlib **(but the format of printf() is not as same as printf() from stdio, ask for detail, please look at [LanternOS stdlib](doc/LanternOS_stdlib.md))**
5. Keyboard I/O
6. Basic terminal
7. Basic memory manager

# Usage
First of all, clone this responsity:
`git clone https://github.com/ternaryop8479/LanternOS.git`

then compile `make.cpp` to `make`:
`g++ make.cpp -o make -O2`

and run `./make`

then, use `./run.sh` to start the LanternOS with your QEMU.

# TODO
1. Make the EDID support.
2. Make a font for LanternOS Terminal.
3. Port the stdlib and STL to LanternOS.
4. Make the disk support.
5. Make the network support.
6. Compelete the key-maps (key-board mapping).
