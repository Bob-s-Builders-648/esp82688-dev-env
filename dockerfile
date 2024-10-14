# use the official Debian 11 (Bullseye) as the base image
FROM debian:11

# set non-interactive frontend for apt
ENV DEBIAN_FRONTEND=noninteractive

# update and install necessary packages
RUN apt-get update && apt-get install -y \
    gcc \
    git \
    vim \
    wget \
    make \
    libncurses-dev \
    flex \
    bison \
    gperf \
    python3 \
    python3-serial \
    python3-pip \
    && rm -rf /var/lib/apt/lists/*

# symbolic link for python3
RUN ln -s /usr/bin/python3 /usr/bin/python

# set environment variables and directories
ENV IDF_PATH=/opt/esp8266
ENV PATH=$PATH:/opt/esp8266/tools/xtensa-lx106-elf/bin
RUN mkdir -p /opt/esp8266

# clone the ESP8266_RTOS_SDK
RUN git clone --recursive https://github.com/espressif/ESP8266_RTOS_SDK.git /opt/esp8266

# set up Xtensa tool chain for linux
RUN wget -O /tmp/xtensa-lx106-elf.tar.gz \
    https://dl.espressif.com/dl/xtensa-lx106-elf-gcc8_4_0-esp-2020r3-linux-amd64.tar.gz && \
    tar -xzf /tmp/xtensa-lx106-elf.tar.gz -C /opt/esp8266/tools && \
    rm /tmp/xtensa-lx106-elf.tar.gz

RUN /usr/bin/python -m pip install --user -r /opt/esp8266/requirements.txt

# set work directory
WORKDIR /workspace

# default command
CMD ["/bin/bash"]
