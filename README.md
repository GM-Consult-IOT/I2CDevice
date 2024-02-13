<!-- ![](https://github.com/<OWNER>/<REPOSITORY>/workflows/<WORKFLOW_NAME>/badge.svg) -->
![](https://github.com/GM-Consult-IOT/I2CDevice/workflows/Build/badge.svg)
![](https://github.com/GM-Consult-IOT/I2CDevice/workflows/Release/badge.svg)

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

```C++
#include <Arduino.h>

// include the library in your main.cpp
#include <I2CDevice.h>

// specify the SDA and SCL pins if not standard
#define I2C_SDA 21 // default SDA pin on the ESP32
#define I2C_SCL 22 // default SCL pin on the ESP32
#define APDS_ADDR 0x39 // I2C address for an APDS9930 sensor.
#define LED_OUTPUT_PIN 26 // Attach an LED to GPIO26, with a current limiting resistor.

/// @brief List of connected I2C device addresses.
byte devices[9]; 

/// @brief Buffer for reading all the register values from the
/// APDS9930 sensor.
byte regValues[0x1F];

/// @brief The I2CDevice instance to be tested.
I2CDevice i2c(APDS_ADDR, &Wire);

/// @brief The current LED state.
bool ledStateOn = true;

/// @brief Initialize the LED pin and turn on the LED.
void powerUp();

void setup() {
  // wait 50ms
  delay(50);

  // initialize the LED and turn on
  powerUp();

  // initialize the debugging port
  Serial.begin(115200);  
  while(!Serial){
    vTaskDelay(50/portTICK_PERIOD_MS);
  }
  Serial.println("Serial port is initialized."); 

  // initialize the [I2CDevice].
  if (i2c.begin(true, I2C_SDA, I2C_SCL)){    
    Serial.printf("I2C Device initialized with "
        "address 0x%s\n", String(i2c.address(), HEX)); 
    i2c.listDevices(devices); 
    i2c.readAllRegisters(regValues, 0x1F);
  } else {    
    Serial.println("I2C Device FAILED to initialize!"); 
  }
}
void loop() {
  // let's flash the LED
  ledStateOn = !ledStateOn;
  digitalWrite(LED_OUTPUT_PIN, ledStateOn);

  delay(500);
}

void powerUp(){    
    // set the LED pin mode
    pinMode(LED_OUTPUT_PIN, OUTPUT);
    // turn on the LED
    digitalWrite(LED_OUTPUT_PIN, HIGH);
}
```

## References
* [Adafruit_BusIO](https://github.com/adafruit/Adafruit_BusIO)
* [CI for Arduino](https://mirzafahad.github.io/2021-03-09-github-cicd-for-arduino-projects/)
* [ESP32 and ESP8266 continuous integration with PlatformIO demo using GitHub Actions and badges](https://github.com/kaizoku-oh/pio-ci-example/tree/master/.github/workflows)

