# esp-homekit-demo
Demo of [Apple HomeKit accessory server
library](https://github.com/maximkulkin/esp-homekit).

## Usage

See [build instructions](https://github.com/maximkulkin/esp-homekit-demo/wiki/Build-instructions)


# homekit
0. WSL must be 1 for com port access
x. need v6.7.6 of si labs drivers, 10.x.x will not work with WSL 
    https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers
    https://github.com/microsoft/WSL/issues/3795
1. https://github.com/maximkulkin/esp-homekit-demo/wiki/Build-instructions-ESP8266
    note: when installing esp-open-sdk, need to do "git submodule init" and "git submodule update"
2. https://github.com/pfalcon/esp-open-sdk
    note build failed on ubuntu 20.04 i use with spyglass, works on fresh 18.04
3. add this to .bashrc
    export PATH="$HOME/xtensa-lx106-elf/bin:$PATH"
    export SDK_PATH=$HOME/esp-open-rtos
    export ESPPORT=/dev/ttyS<port number>
4. esptool run with sudo or it won't put it int PATH
    sudo pip install esptool


