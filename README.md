<!-- ![](https://github.com/<OWNER>/<REPOSITORY>/workflows/<WORKFLOW_NAME>/badge.svg) 
![](https://github.com/GM-Consult-IOT/I2CDevice/workflows/Build/badge.svg)
![](https://github.com/GM-Consult-IOT/I2CDevice/workflows/Release/badge.svg)-->

# [![Github Repo](https://github.com/GM-Consult-IOT/I2CDevice/blob/main/.img/119493932.png)](https://github.com/GM-Consult-IOT/I2CDevice)  I2CDevice

Helper library to abstract away I2C transactions and registers. It is an extension of the [Adafruit_BusIO library's](https://github.com/adafruit/Adafruit_BusIO) `I2CDevice` class.

Contents
- [  I2CDevice](#--i2cdevice)
  - [Description](#description)
  - [Usage](#usage)
  - [References](#references)

## Description

The `I2CDevice` class wraps a reference to a `TwoWire` instance linked to a specific address on the `I2C` bus. 

When the `I2CDevice` is initialized it will also initialize the `TwoWire`instance with the specified SDA and SCL ports. Optionally it will poll the device address and return false if the device did not respond.

The `I2CDevice` exposes the following public functions:

* `begin` initializes the `I2CDevice`.
* `detected` checks the bus for the presence of a device with a specific address and returns true if the address is detected on the bus.
* `readLength` reads from the device into a buffer.
* `read` reads a specified number of bytes from a register into a buffer.
* `write` writes num bytes from specified buffer into a given register.
* `write8` writes specified value to given register.
* `read8` reads 8 bits from specified register.
* `read16` reads 16 bits from specified register.
* `read16R ` reads 16 bits from specified register.
* `read32` reads 32 bits from specified register.
* `writeLen` writes a buffer to the I2C device. 
* `write_then_read` writes some data, then read some data from I2C into another buffer.
* `setSpeed` changes the I2C clock speed.
* `maxBufferSize` Returns the maximum number of bytes that can be read in a transaction.
* `listDevices` polls all addresses on the I2C bus and populates an array of the addresses that respond.
* `readAllRegisters` reads the register values from the device, starting at a specified register and reading a specified number of elements.
* `getByteString` is a static function that returns a formatted string from a byte value.

## Usage

Examples of usage are available in the [examples folder](https://github.com/GM-Consult-IOT/I2CDevice/tree/main/lib/I2CDevice/examples).

Code snippets below demonstrate simple operations.

```C++

// include the library in your main.cpp
#include <I2CDevice.h>

// initialize the [I2CDevice].
bool ready = i2c.begin(true, I2C_SDA, I2C_SCL);
    
// list all the devices on the bus and return the number found
uint8_t num = i2c.listDevices(devices);    
  

// Read one value from a register:
// *****************************************************

// the buffer to read to
byte regValues[1];  

// Assemble the control command, in this case an 8-bit 
// value with bits 7 and 5 set and bits 4-1 being the 
// register address
byte cmd[1] = {0x12 | 0xA0};

// write the command then read the value in register 0x12 
i2c.write_then_read(cmd, 1, regValues, 1, true);

```

## References
* [I2C-Bus Specification and user manual](https://www.nxp.com/docs/en/user-guide/UM10204.pdf)
* [I2C, Wikipedia]
* [Adafruit_BusIO](https://github.com/adafruit/Adafruit_BusIO)
* [CI for Arduino](https://mirzafahad.github.io/2021-03-09-github-cicd-for-arduino-projects/)
* [ESP32 and ESP8266 continuous integration with PlatformIO demo using GitHub Actions and badges](https://github.com/kaizoku-oh/pio-ci-example/tree/master/.github/workflows)

