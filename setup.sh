#!/bin/bash
sudo mkdir /opt/toolchain
sudo chown $USER:$USER /opt/toolchain

sudo apt install build-essential bison flex libgmp3-dev libmpc-dev libmpfr-dev texinfo wget \
    nasm mtools python3 python3-pip python3-parted dosfstools libguestfs-tools qemu-system-x86 libparted-dev -y
