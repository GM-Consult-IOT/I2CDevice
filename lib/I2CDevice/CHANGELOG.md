<!-- I2CDevice -->

## 1.0.3

* Added function `bool I2CDevice::isInitialized()`.
* Minor changes to `I2CDevice::begin` definition.

## 1.0.2

* Added utility read and write functions to `I2Device` class.
* Renamed function `I2Device::read` to `I2Device::readLen`.
* Renamed function `I2Device::write` to `I2Device::writeLen`.

## 1.0.1

* Added function `uint8_t I2Device::listDevices(byte * devices)`. 
* Added static function `String I2Device::getByteString(byte b)`.

## 1.0.0

* Initial version, adapted from [Adafruit Bus IO Library](https://github.com/adafruit/Adafruit_BusIO).