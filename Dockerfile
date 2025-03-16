FROM ubuntu:latest
LABEL authors="rohittp"

WORKDIR /app

RUN mkdir /opt/toolchain

RUN apt update --allow-insecure-repositories
RUN apt install build-essential bison flex libgmp3-dev libmpc-dev libmpfr-dev texinfo wget \
        nasm mtools python3 python3-pip python3-parted dosfstools libguestfs-tools qemu-system-x86 libparted-dev -y

RUN apt install python3.12-venv pkg-config -y

COPY . .

RUN python3 -m venv venv
RUN venv/bin/pip install -r requirements.txt

RUN venv/bin/scons toolchain

CMD ["sleep", "infinity"]