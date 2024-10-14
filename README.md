# ESP8266 Development Setup and Monitoring

Provided is step-by-step instructions for setting up the ESP8266 development environment and monitoring its serial output.

## Prerequisites

Before you start, ensure you have the following:

- **ESP8266 Module** (e.g., NodeMCU, Wemos D1 Mini)
- **USB to Serial Adapter** (if not built-in)
- **Docker**
- **Arduino IDE** or **Visual Studio Code** with PlatformIO (optional)

## Setup Instructions

### 1. Install Required Tools

- Python
- Docker (if not already installed): https://www.docker.com/
- esptool (bootloader utility for espressif chips) https://github.com/espressif/esptool

### 2. Build environment

In root directory with dockerfile, build image
```
docker build --platform=linux/amd64 -t esp8266-dev-debian .
```

To start container use command.
```
docker run --platform=linux/amd64 -it --rm \
--name esp8266-dev-debian \
-v $(pwd):/workspace \
esp8266-dev-debian
```

### 3. Build Project

Navigate to project directory mounted at `/workspace` and build using parallel jobs.
```
make -j$(nproc)
``` 

### 4. Flash Firmware from Host
Exit container and run command (replace `/dev/ttyUSB0` with actual device path

```
esptool.py --port /dev/ttyUSB0 write_flash 0x00000 $(pwd)/hello_world/build/hello_world.bin
```

### 5. Monitoring ESP8266
(MacOS) To use screen, run:
```
screen /dev/ttyUSB0 115200
```
(Windows) You can use PuTTY:
1. Open PuTTY, select "Serial" as the connection type.
2. Enter the serial line (e.g., COM3) and set the speed to 115200.
3. Click "Open" to start monitoring. 
