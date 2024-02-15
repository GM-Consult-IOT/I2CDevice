#include "I2CDevice.h"
#include <algorithm>
#include <string>


I2CDevice::I2CDevice(uint8_t addr, TwoWire *theWire) {
    _addr = addr;
    _wire = theWire;
    _begun = false;
    #ifdef ARDUINO_ARCH_SAMD
    _maxBufferSize = 250; // as defined in _wire->h's RingBuffer
    #elif defined(ESP32)
    _maxBufferSize = I2C_BUFFER_LENGTH;
    #else
    _maxBufferSize = 32;
    #endif
};

bool I2CDevice::begin(bool addr_detect, 
            int sda, 
            int scl, 
            uint32_t frequency) {
    if (!_wire->begin(sda, scl, frequency)) return false;
    _begun = true;
    if (addr_detect) {
        _begun = detected();
    }
    return _begun;
};

TwoWire * I2CDevice::wire(){
    return _wire;
};

bool I2CDevice::isInitialized(){
    return _begun;
};

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
    #ifdef DEBUG_I2DEVICE_SERIAL
    DEBUG_I2DEVICE_SERIAL.print(F("Address 0x"));
    DEBUG_I2DEVICE_SERIAL.print(_addr);
    #endif
    if (_wire->endTransmission() == 0) {
        #ifdef DEBUG_I2DEVICE_SERIAL
        DEBUG_I2DEVICE_SERIAL.println(F(" Detected"));
        #endif
        return true;
    }
    #ifdef DEBUG_I2DEVICE_SERIAL
    DEBUG_I2DEVICE_SERIAL.println(F(" Not detected"));
    #endif
    return false;
};

bool I2CDevice::write(uint8_t val, 
                      bool stop){
    _wire->beginTransmission(_addr);
    _wire->write(val);
    if( _wire->endTransmission(stop) != 0 ) {
        return false;
    }    
    return true;
};


// bool I2CDevice::write(uint16_t val, 
//                       bool bigEndian, 
//                       bool stop){
//     byte buf[2];
//     buf[0] = bigEndian? highByte(val): lowByte(val);
//     buf[1] = bigEndian? lowByte(val): highByte(val);
//     _wire->beginTransmission(_addr);
//     if( _wire->write(buf,2) == 2) {
//         return  _wire->endTransmission(stop) == 0;
//     }    
//     return false;
// };


// bool I2CDevice::write(uint32_t val, bool bigEndian, 
//                bool stop){
//     byte buf[4];
//     buf[0] = bigEndian? highByte(val): lowByte(val);
//     buf[1] = bigEndian? lowByte(val): highByte(val);
//     _wire->beginTransmission(_addr);
//     buf[0] = bigEndian? (val >> 24) & 0xFF: (val) & 0xFF;
//     buf[1] = bigEndian? (val >> 16) & 0xFF: (val >> 8) & 0xFF;
//     buf[2] = bigEndian? (val >> 8) & 0xFF: (val >> 16) & 0xFF;
//     buf[3] = bigEndian? (val) & 0xFF: (val >> 24) & 0xFF;
//     if( _wire->write(buf, 4) == 4) {
//         return  _wire->endTransmission(stop) == 0;
//     }    
//     return false;
// };

// bool I2CDevice::write(uint8_t reg, 
//         const uint8_t * buf, 
//         size_t len , 
//         bool stop){
//     uint8_t pref[1] = {reg};
//     return write(buf, len, stop, pref, 1);
// };

bool I2CDevice::write(const uint8_t *buffer, size_t len, bool stop,
                    const uint8_t *prefix_buffer,
                    size_t prefix_len) {
    if ((len + prefix_len) > maxBufferSize()) {
        // currently not guaranteed to work if more than 32 bytes!
        // we will need to find out if some platforms have larger
        // I2C buffer sizes :/
        #ifdef DEBUG_I2DEVICE_SERIAL
        DEBUG_I2DEVICE_SERIAL.println(F("\tI2CDevice could not write such a large buffer"));
        #endif
        return false;
    }
    _wire->beginTransmission(_addr);
    // Write the prefix data (usually an address)
    if ((prefix_len != 0) && (prefix_buffer != nullptr)) {
        if (_wire->write(prefix_buffer, prefix_len) != prefix_len) {
            #ifdef DEBUG_I2DEVICE_SERIAL
            DEBUG_I2DEVICE_SERIAL.println(F("\tI2CDevice failed to write"));
            #endif
            return false;
        }
    }
    // Write the data itself
    if (_wire->write(buffer, len) != len) {
        #ifdef DEBUG_I2DEVICE_SERIAL
        DEBUG_I2DEVICE_SERIAL.println(F("\tI2CDevice failed to write"));
        #endif
        return false;
    }
    #ifdef DEBUG_I2DEVICE_SERIAL
    DEBUG_I2DEVICE_SERIAL.print(F("\tI2CWRITE @ 0x"));
    DEBUG_I2DEVICE_SERIAL.print(_addr, HEX);
    DEBUG_I2DEVICE_SERIAL.print(F(" :: "));
    if ((prefix_len != 0) && (prefix_buffer != nullptr)) {
        for (uint16_t i = 0; i < prefix_len; i++) {
            DEBUG_I2DEVICE_SERIAL.print(F("0x"));
            DEBUG_I2DEVICE_SERIAL.print(prefix_buffer[i], HEX);
            DEBUG_I2DEVICE_SERIAL.print(F(", "));
        }
    }
    for (uint16_t i = 0; i < len; i++) {
        DEBUG_I2DEVICE_SERIAL.print(F("0x"));
        DEBUG_I2DEVICE_SERIAL.print(buffer[i], HEX);
        DEBUG_I2DEVICE_SERIAL.print(F(", "));
        if (i % 32 == 31) {
            DEBUG_I2DEVICE_SERIAL.println();
        }
    }
    if (stop) {
        DEBUG_I2DEVICE_SERIAL.print("\tSTOP");
    }
    #endif
    if (_wire->endTransmission(stop) == 0) {
        #ifdef DEBUG_I2DEVICE_SERIAL
        DEBUG_I2DEVICE_SERIAL.println();
        // DEBUG_I2DEVICE_SERIAL.println("Sent!");
        #endif
        return true;
    } else {
        #ifdef DEBUG_I2DEVICE_SERIAL
        DEBUG_I2DEVICE_SERIAL.println("\tFailed to send!");
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
        #ifdef DEBUG_I2DEVICE_SERIAL
        DEBUG_I2DEVICE_SERIAL.print(F("\tI2CDevice did not receive enough data: "));
        DEBUG_I2DEVICE_SERIAL.println(recv);
        #endif
        return false;
    }
    for (uint16_t i = 0; i < len; i++) {
        buffer[i] = _wire->read();
    }
    #ifdef DEBUG_I2DEVICE_SERIAL
    DEBUG_I2DEVICE_SERIAL.print(F("\tI2CREAD  @ 0x"));
    DEBUG_I2DEVICE_SERIAL.print(_addr, HEX);
    DEBUG_I2DEVICE_SERIAL.print(F(" :: "));
    for (uint16_t i = 0; i < len; i++) {
        DEBUG_I2DEVICE_SERIAL.print(F("0x"));
        DEBUG_I2DEVICE_SERIAL.print(buffer[i], HEX);
        DEBUG_I2DEVICE_SERIAL.print(F(", "));
        if (len % 32 == 31) {
            DEBUG_I2DEVICE_SERIAL.println();
        }
    }
    DEBUG_I2DEVICE_SERIAL.println();
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
        #ifdef DEBUG_I2DEVICE_SERIAL
        DEBUG_I2DEVICE_SERIAL.println(F("I2C.setSpeed too high."));
        #endif
        return false;
    }
    uint32_t atwbr = ((F_CPU / desiredclk) - 16) / 2;
    if (atwbr > 16320) {
        #ifdef DEBUG_I2DEVICE_SERIAL
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

    #ifdef DEBUG_I2DEVICE_SERIAL
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

uint8_t I2CDevice::listDevices(uint8_t * devices, 
                               bool verbose){
    uint8_t error, address;    
    uint8_t nDevices;
    if(verbose) {
        Serial.println("Scanning I2C bus");
    }
    nDevices = 0;
    // I2c address is 7 bits, so up to 0x7f addresses on the bus.
    for(address = 1; address < 0x7f; address++ ) {     
        _wire->beginTransmission(address);
        error = _wire->endTransmission();
        if (error == 0) {
            devices[nDevices]=address;
            nDevices++;
        }
        if(verbose) {
            String addressStr = getByteString(address, HEX);               
            if (error == 0) {
                Serial.printf("\nFound I2C device at %s\n", addressStr);
            } else if (error != 0x02) {        
                Serial.printf("\nError [%s] occurred while polling address %s\n",
                    getByteString(error, HEX), addressStr);
            } else {          
                Serial.print(".");           
            }
        }
    }
    if(verbose) {
        if (nDevices == 0) {
            Serial.println("\nNO devices found on I2C bus.");
        }
        if (nDevices == 1){ 
            Serial.println("\nFound ONE device on the I2C bus.");
        }  
        if (nDevices > 1){ 
            Serial.printf("\nFound %i devices on the I2C bus.\n",
                    String(nDevices));
        }
    }
    return nDevices;
};

String I2CDevice::getByteString(uint8_t b, 
                                uint8_t format, 
                                bool addPrefix){
    // prefix with "0x"
    String bStr;
    String str = String(b, format);
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    switch (format){
        case BIN:
            bStr = String(addPrefix? "0B": "") +
                   (b < 0b10? "0000000":
                    b < 0b100? "000000":
                    b < 0b1000? "00000":
                    b < 0b10000? "0000":
                    b < 0b100000? "000":
                    b < 0b1000000? "00":
                    b < 0b10000000? "0":
                    "") ;
        break;
        case HEX:
            bStr = String(addPrefix? "0X": "") +
                   (b < 0x10? "0":"");    
        break;
        default:
        break;
    }
    if (format == BIN){
        
    }
      // add the b and return the string
      return bStr+str;
};

// void I2CDevice::write8(uint8_t reg, uint8_t value) {
//     uint8_t buf[1] = {value};
//     this->write(reg, buf, 1);
// };

// uint8_t I2CDevice::read8(uint8_t reg) {
//     write(reg);
//     uint8_t buf[0];
//     this->read(0, buf, 0x20, true);
//     return buf[reg];
// };

// uint32_t I2CDevice::read32(uint8_t reg) {
//     uint8_t ret[4];
//     uint32_t ret32;
//     this->read(reg, ret, 4);
//     ret32 = ret[3];
//     ret32 |= (uint32_t)ret[2] << 8;
//     ret32 |= (uint32_t)ret[1] << 16;
//     ret32 |= (uint32_t)ret[0] << 24;
//     return ret32;
// };

// uint16_t I2CDevice::read16(uint8_t reg) {
//     uint8_t ret[2];
//     this->read(reg, ret, 2);
//     return (ret[0] << 8) | ret[1];
// };

// uint16_t I2CDevice::read16Reversed(uint8_t reg) {
//     uint8_t ret[2];
//     this->read(reg, ret, 2);
//     return (ret[1] << 8) | ret[0];
// };

// uint8_t I2CDevice::readRegister(uint8_t reg, 
//                  uint8_t *buf, 
//                  uint8_t num,
//                  bool verbose) {
    
//     // buf[0] = reg;
//     if (write(reg)){
//         if (read(buf,num, true))
//     // write_then_read(buf, 1, buf, num);
//         if(verbose) {    
//             Serial.println("______________________________");    
//             Serial.println("REGISTER               VALUE");
//             Serial.println("------------------------------");
//             for (uint8_t i = 0; i < num; i++){
//                 uint8_t regI = i + reg;
//                 Serial.printf( " %s              %s\n", 
//                     I2CDevice::getByteString(regI, HEX), 
//                     I2CDevice::getByteString(buf[i], BIN));  
//             }
//         }   
//     }
//     return num;
// }

// void I2CDevice::write(uint8_t reg, uint8_t *buf, uint8_t num) {
//     uint8_t prefix[1] = {reg};
//     writeLen(buf, num, true, prefix, 1);
// };

// void I2CDevice::readAllRegisters(uint8_t * buf,
//                         uint8_t len, 
//                         uint8_t startReg,
//                         bool verbose){
//     if(verbose) {    
//     Serial.println("______________________________");    
//     Serial.println("REGISTER               VALUE");
//     Serial.println("------------------------------");
//     }   
//     for (uint8_t i = 0; i < len; i++){
//         uint8_t reg = i + startReg;
//         buf[i] = read8(i + startReg);
//         Serial.printf("buf[i] = %s\n", String(buf[i], BIN) );
//         if(verbose) { 
//         Serial.printf( " %s              %s\n", 
//             I2CDevice::getByteString(reg, HEX), 
//             I2CDevice::getByteString(buf[i], BIN));  

//         } 
//     }
// };
