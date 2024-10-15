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
- Arduino IDE (optional), some terminals do not support UART communication at certain baud rates. Arduino IDE's serial monitor allows an easy way to interface with the ESP8266.

### 2. Build environment

In root directory with dockerfile, build image. This may take a while, as it is building the entire ESP8266 RTOS SDK and the Xtesna toolchain for Linux, as well as setting up other dependencies.
```
docker build --platform=linux/amd64 -t esp8266-dev-debian .
```

To start container use command in the root directory. Change `/dev/ttyUSB0` to the port your ESP8266 is located.
```
cd hello_world
docker run \
--platform=linux/amd64 \
-it --rm --privileged \
--name esp8266-dev-debian \
--device=/dev/ttyUSB0 \
-v $(pwd):/workspace \
esp8266-dev-debian

```
**Flags**
- `--platform=linux/amd64` this ensures compatability with the Xtensa-Lx06 toolchain which runs on 64 bit x86 architecture
- `-it --rm --privileged \` this flag keeps STDIN open when not attatched if we desire interactive input, allocates pesudo-TTY, removes the container when stopped, and grants extended priveleges to access the serial ports.
- `--device=/dev/ttyUSB0 \` Gives container access to specific device on host machine
- `-v $(pwd):/workspace \` Mounts volume to share between host and container. We will use this to share our project directory between the two.

### 3. Build Project

Navigate to project directory mounted at `/workspace` and build using parallel jobs. This may also take a while on the initial setup as the SDK will build all possible binaries that could be used in a hardware project, making subsequent compilations much faster. Still, because this is done through a container it takes time for the files to be written to our mounted directory
```
make -j$(nproc)
```

The first time you build the project, a configuration menu will open.
![image](https://github.com/user-attachments/assets/fb4d4354-6054-4540-890e-0437ab5391b3)

We will mostly be using the defaults for now. Simply note the flash offsets within the Bootloader config and Partition Table options. These will be important for when we finally begin flashing the puilt program to the ESP.


### 4. Flash Firmware from Host
Exit container and run command (replace `/dev/ttyUSB0` with actual device path.

```
esptool.py --port /dev/ttyUSB0 write_flash 0x00000 build/bootloader/bootloader.bin 0x10000 build/hello-world.bin 0x8000 ./build/partitions_singleapp.bin
```
As you can image `0x000000` `0x10000` and `0x8000` all correspond to addresses within memory, where we will directly flash the bootloader, partition table, and application binaries to the ESP8266.
| `0x00000 - 0x00FFF` | `0x01000 - 0x07FFF` | `0x08000` |
| ------------------- |-------------------- |---------- |
| Bootloader | Program Binaries | Partition Table 

The ESP8266 uses an external SPI flash memory chip to store these components in their corresponding addresses on chip. When booted it reads the boatloader firmware starting from `0x0`. It will reference the partition table at `0x08000` to determine where to find and how to manage program binaries at `0x10000`.


### 5. Monitoring ESP8266
(Arduino IDE)
1. Open the Arduino IDE and navigate to preferences/settings.
2. Go to `Additional Boards Manager URLS:` and add the following link https://arduino.esp8266.com/stable/package_esp8266com_index.json
3. Go to Tools > Board > Board Manager
4. Search for the esp8226 and install it
5. Go back to Tools > Board and select NodeMCU (either the ESP-12 or ESP-12E works).
6. Go to Port and select COM3
7. Finally open the Serial Monitor and select 74880 baud

(MacOS) To use screen, run: Replace `/dev/ttyUSB0` with the proper location of the device.
```
screen /dev/ttyUSB0 74880
```
(Windows) You can use PuTTY:
1. Open PuTTY, select "Serial" as the connection type.
2. Enter the serial line (e.g., COM3) and set the speed to 74880.
3. Click "Open" to start monitoring. 
