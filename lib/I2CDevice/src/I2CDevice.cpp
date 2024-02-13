#include "I2CDevice.h"


I2CDevice::I2CDevice(uint8_t addr, TwoWire *theWire) {
    _addr = addr;
    _wire = theWire;
    _begun = false;
    #ifdef ARDUINO_ARCH_SAMD
    _maxBufferSize = 250; // as defined in Wire.h's RingBuffer
    #elif defined(ESP32)
    _maxBufferSize = I2C_BUFFER_LENGTH;
    #else
    _maxBufferSize = 32;
    #endif
};

bool I2CDevice::begin(bool addr_detect) {
    _wire->begin();
    _begun = true;
    if (addr_detect) {
        return detected();
    }
    return true;
};

/*!
*    @brief  De-initialize device, turn off the Wire interface
*/
void I2CDevice::end(void) {
    // Not all port implement Wire::end(), such as
    // - ESP8266
    // - AVR core without WIRE_HAS_END
    // - ESP32: end() is implemented since 2.0.1 which is latest at the moment.
    // Temporarily disable for now to give time for user to update.
    #if !(defined(ESP8266) ||                                                      \
    (defined(ARDUINO_ARCH_AVR) && !defined(WIRE_HAS_END)) ||                 \
    defined(ARDUINO_ARCH_ESP32))
    _wire->end();
    _begun = false;
    #endif
};

/*!
*    @brief  Scans I2C for the address - note will give a false-positive
*    if there's no pullups on I2C
*    @return True if I2C initialized and a device with the addr found
*/
bool I2CDevice::detected(void) {
    // Init I2C if not done yet
    if (!_begun && !begin()) {
        return false;
    }

// A basic scanner, see if it ACK's
    _wire->beginTransmission(_addr);
    #ifdef DEBUG_I2DEVICE_DEBUG_I2DEVICE_SERIAL
    DEBUG_I2DEVICE_DEBUG_I2DEVICE_SERIAL.print(F("Address 0x"));
    DEBUG_I2DEVICE_DEBUG_I2DEVICE_SERIAL.print(_addr);
    #endif
    if (_wire->endTransmission() == 0) {
        #ifdef DEBUG_I2DEVICE_DEBUG_I2DEVICE_SERIAL
        DEBUG_I2DEVICE_DEBUG_I2DEVICE_SERIAL.println(F(" Detected"));
        #endif
        return true;
    }
    #ifdef DEBUG_I2DEVICE_DEBUG_I2DEVICE_SERIAL
    DEBUG_I2DEVICE_DEBUG_I2DEVICE_SERIAL.println(F(" Not detected"));
    #endif
    return false;
};

bool I2CDevice::write(const uint8_t *buffer, size_t len, bool stop,
                    const uint8_t *prefix_buffer,
                    size_t prefix_len) {
    if ((len + prefix_len) > maxBufferSize()) {
        // currently not guaranteed to work if more than 32 bytes!
        // we will need to find out if some platforms have larger
        // I2C buffer sizes :/
        #ifdef DEBUG_I2DEVICE_DEBUG_I2DEVICE_SERIAL
        DEBUG_I2DEVICE_DEBUG_I2DEVICE_SERIAL.println(F("\tI2CDevice could not write such a large buffer"));
        #endif
        return false;
    }

    _wire->beginTransmission(_addr);

    // Write the prefix data (usually an address)
    if ((prefix_len != 0) && (prefix_buffer != nullptr)) {
        if (_wire->write(prefix_buffer, prefix_len) != prefix_len) {
            #ifdef DEBUG_I2DEVICE_DEBUG_I2DEVICE_SERIAL
            DEBUG_I2DEVICE_DEBUG_I2DEVICE_SERIAL.println(F("\tI2CDevice failed to write"));
            #endif
            return false;
        }
    }
    // Write the data itself
    if (_wire->write(buffer, len) != len) {
        #ifdef DEBUG_I2DEVICE_DEBUG_I2DEVICE_SERIAL
        DEBUG_I2DEVICE_DEBUG_I2DEVICE_SERIAL.println(F("\tI2CDevice failed to write"));
        #endif
        return false;
    }
    #ifdef DEBUG_I2DEVICE_DEBUG_I2DEVICE_SERIAL
    DEBUG_I2DEVICE_DEBUG_I2DEVICE_SERIAL.print(F("\tI2CWRITE @ 0x"));
    DEBUG_I2DEVICE_DEBUG_I2DEVICE_SERIAL.print(_addr, HEX);
    DEBUG_I2DEVICE_DEBUG_I2DEVICE_SERIAL.print(F(" :: "));
    if ((prefix_len != 0) && (prefix_buffer != nullptr)) {
        for (uint16_t i = 0; i < prefix_len; i++) {
            DEBUG_I2DEVICE_DEBUG_I2DEVICE_SERIAL.print(F("0x"));
            DEBUG_I2DEVICE_DEBUG_I2DEVICE_SERIAL.print(prefix_buffer[i], HEX);
            DEBUG_I2DEVICE_DEBUG_I2DEVICE_SERIAL.print(F(", "));
        }
    }
    for (uint16_t i = 0; i < len; i++) {
        DEBUG_I2DEVICE_DEBUG_I2DEVICE_SERIAL.print(F("0x"));
        DEBUG_I2DEVICE_DEBUG_I2DEVICE_SERIAL.print(buffer[i], HEX);
        DEBUG_I2DEVICE_DEBUG_I2DEVICE_SERIAL.print(F(", "));
        if (i % 32 == 31) {
            DEBUG_I2DEVICE_DEBUG_I2DEVICE_SERIAL.println();
        }
    }
    if (stop) {
        DEBUG_I2DEVICE_DEBUG_I2DEVICE_SERIAL.print("\tSTOP");
    }
    #endif
    if (_wire->endTransmission(stop) == 0) {
        #ifdef DEBUG_I2DEVICE_DEBUG_I2DEVICE_SERIAL
        DEBUG_I2DEVICE_DEBUG_I2DEVICE_SERIAL.println();
        // DEBUG_I2DEVICE_DEBUG_I2DEVICE_SERIAL.println("Sent!");
        #endif
        return true;
    } else {
        #ifdef DEBUG_I2DEVICE_DEBUG_I2DEVICE_SERIAL
        DEBUG_I2DEVICE_DEBUG_I2DEVICE_SERIAL.println("\tFailed to send!");
        #endif
        return false;
    }
};

bool I2CDevice::read(uint8_t *buffer, size_t len, bool stop) {
    size_t pos = 0;
    while (pos < len) {
        size_t read_len =
        ((len - pos) > maxBufferSize()) ? maxBufferSize() : (len - pos);
        bool read_stop = (pos < (len - read_len)) ? false : stop;
        if (!_read(buffer + pos, read_len, read_stop))
            return false;
        pos += read_len;
    }
    return true;
};

bool I2CDevice::_read(uint8_t *buffer, size_t len, bool stop) {
    #if defined(TinyWireM_h)
    size_t recv = _wire->requestFrom((uint8_t)_addr, (uint8_t)len);
    #elif defined(ARDUINO_ARCH_MEGAAVR)
    size_t recv = _wire->requestFrom(_addr, len, stop);
    #else
    size_t recv = _wire->requestFrom((uint8_t)_addr, (uint8_t)len, (uint8_t)stop);
    #endif
    if (recv != len) {
        // Not enough data available to fulfill our obligation!
        #ifdef DEBUG_I2DEVICE_DEBUG_I2DEVICE_SERIAL
        DEBUG_I2DEVICE_DEBUG_I2DEVICE_SERIAL.print(F("\tI2CDevice did not receive enough data: "));
        DEBUG_I2DEVICE_DEBUG_I2DEVICE_SERIAL.println(recv);
        #endif
        return false;
    }
    for (uint16_t i = 0; i < len; i++) {
        buffer[i] = _wire->read();
    }
    #ifdef DEBUG_I2DEVICE_DEBUG_I2DEVICE_SERIAL
    DEBUG_I2DEVICE_DEBUG_I2DEVICE_SERIAL.print(F("\tI2CREAD  @ 0x"));
    DEBUG_I2DEVICE_DEBUG_I2DEVICE_SERIAL.print(_addr, HEX);
    DEBUG_I2DEVICE_DEBUG_I2DEVICE_SERIAL.print(F(" :: "));
    for (uint16_t i = 0; i < len; i++) {
        DEBUG_I2DEVICE_DEBUG_I2DEVICE_SERIAL.print(F("0x"));
        DEBUG_I2DEVICE_DEBUG_I2DEVICE_SERIAL.print(buffer[i], HEX);
        DEBUG_I2DEVICE_DEBUG_I2DEVICE_SERIAL.print(F(", "));
        if (len % 32 == 31) {
            DEBUG_I2DEVICE_DEBUG_I2DEVICE_SERIAL.println();
        }
    }
    DEBUG_I2DEVICE_DEBUG_I2DEVICE_SERIAL.println();
    #endif
    return true;
};

bool I2CDevice::write_then_read(const uint8_t *write_buffer,
                size_t write_len, uint8_t *read_buffer,
                size_t read_len, bool stop) {
    if (!write(write_buffer, write_len, stop)) {
        return false;
    }
    return read(read_buffer, read_len);
};

uint8_t I2CDevice::address(void) { 
    return _addr; 
};

bool I2CDevice::setSpeed(uint32_t desiredclk) {
    #if defined(__AVR_ATmega328__) ||                                              \
    defined(__AVR_ATmega328P__) // fix arduino core set clock
    // calculate TWBR correctly

    if ((F_CPU / 18) < desiredclk) {
        #ifdef DEBUG_I2DEVICE_DEBUG_I2DEVICE_SERIAL
        DEBUG_I2DEVICE_SERIAL.println(F("I2C.setSpeed too high."));
        #endif
        return false;
    }
    uint32_t atwbr = ((F_CPU / desiredclk) - 16) / 2;
    if (atwbr > 16320) {
        #ifdef DEBUG_I2DEVICE_DEBUG_I2DEVICE_SERIAL
        DEBUG_I2DEVICE_SERIAL.println(F("I2C.setSpeed too low."));
        #endif
        return false;
    }

    if (atwbr <= 255) {
        atwbr /= 1;
        TWSR = 0x0;
        } else if (atwbr <= 1020) {
        atwbr /= 4;
        TWSR = 0x1;
    } else if (atwbr <= 4080) {
        atwbr /= 16;
        TWSR = 0x2;
    } else { //  if (atwbr <= 16320)
        atwbr /= 64;
        TWSR = 0x3;
    }
    TWBR = atwbr;

    #ifdef DEBUG_I2DEVICE_DEBUG_I2DEVICE_SERIAL
    DEBUG_I2DEVICE_SERIAL.print(F("TWSR prescaler = "));
    DEBUG_I2DEVICE_SERIAL.println(pow(4, TWSR));
    DEBUG_I2DEVICE_SERIAL.print(F("TWBR = "));
    DEBUG_I2DEVICE_SERIAL.println(atwbr);
    #endif
    return true;
    #elif (ARDUINO >= 157) && !defined(ARDUINO_STM32_FEATHER) &&                   \
        !defined(TinyWireM_h)
        _wire->setClock(desiredclk);
    return true;

    #else
        (void)desiredclk;
        return false;
    #endif
};

uint8_t I2CDevice::listDevices(byte * devices){
    byte error, address;
    uint8_t len = sizeof(devices);
    uint8_t nDevices;
    #ifdef DEBUG_I2DEVICE_DEBUG_I2DEVICE_SERIAL
    DEBUG_I2DEVICE_DEBUG_I2DEVICE_SERIAL.println("Scanning I2C bus");
    #endif
    nDevices = 0;
    for(address = 1; address < len; address++ ) {     
        _wire->beginTransmission(address);
        error = _wire->endTransmission();
        String addressStr = getByteString(address);  
        if (error == 0) {
            #ifdef DEBUG_I2DEVICE_DEBUG_I2DEVICE_SERIAL
            DEBUG_I2DEVICE_DEBUG_I2DEVICE_SERIAL.println("Found I2C device at address " + addressStr);
            #endif
            devices[nDevices]=address;
            nDevices++;
        }
        else if (error==4) {
            #ifdef DEBUG_I2DEVICE_DEBUG_I2DEVICE_SERIAL
            DEBUG_I2DEVICE_SERIAL.println("Unknow error at address " + addressStr);
            #endif
        } else {
            #ifdef DEBUG_I2DEVICE_DEBUG_I2DEVICE_SERIAL
            DEBUG_I2DEVICE_SERIAL.println("Error occurred: "+ String(error, HEX));
            #endif
        }   
    }
    #ifdef DEBUG_I2DEVICE_DEBUG_I2DEVICE_SERIAL
    if (nDevices == 0) {
        DEBUG_I2DEVICE_SERIAL.println("No I2C devices found\n");
    }
    if (nDevices == 0) {
        DEBUG_I2DEVICE_SERIAL.println("No devices found on I2C bus");
    }
    if (nDevices == 1){ 
        DEBUG_I2DEVICE_SERIAL.println("Found one device on the I2C bus");
    }  
    if (nDevices > 1){ 
        DEBUG_I2DEVICE_SERIAL.println("Found " + String(nDevices) + " devices on the I2C bus");
    }
    #endif
    return nDevices;
};

String I2CDevice::getByteString(byte b){
  // prefix with "0x"
  String bStr = "0x";
      if (b<16) {
        // add "0" if less than 16
        bStr = bStr + "0";
      }
      // add the b and return the string
      return bStr+String(b, HEX);
}