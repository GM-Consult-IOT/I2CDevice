/*!
 *  @file I2CDevice.h
 *
 *  @mainpage Helper library for I2C transactions and registers.
 *
 *  @section author Author
 *
 *  Gerhard Malan for GM Consolidated Holdings Pty Ltd based on the 
 *  [Adafruit library](https://github.com/adafruit/Adafruit_BusIO).
 *
 *  @section license License
 * 
 *  Copyright (c) 2024, GM Consolidated Holdings Pty Ltd, all rights 
 *  reserved.
 * 
 *  This library is open-source under the BSD 3-Clause license and 
 *  redistribution and use in source and binary forms, with or without 
 *  modification, are permitted, provided that the license conditions are met.
 * 
 *  Original library Copyright (c) 2017, Adafruit Industries, all rights 
 *  reserved. The Adafruit license terms follow below.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *  notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *  notice, this list of conditions and the following disclaimer in the
 *  documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holders nor the
 *  names of its contributors may be used to endorse or promote products
 *  derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
 *  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
 *  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef IC2_DEVICE_H_
#define IC2_DEVICE_H_


// #define DEBUG_I2DEVICE_SERIAL Serial

#include <Arduino.h>
#include <Wire.h>


#define I2C_SDA 21
#define I2C_SCL 22
#define I2C_FREQ 0U

///< The class which defines how we will talk to this device over I2C
class I2CDevice {
public:

    /// @brief Default constructor instantiates an [I2CDevice] instance.
    /// @param addr The I2C address of the device on the bus.
    /// @param theWire Reference to the [TwoWire] instance used by this 
    /// instance. Defaults to [Wire].
    I2CDevice(uint8_t addr, TwoWire *theWire = &Wire);

    /// @brief Returns the I2C address of the device on the bus.
    /// @return The I2C address of the device on the bus
    uint8_t address(void);

    /// @brief Returns the [TwoWire] reference used by the I2CDevice.
    /// @return the [TwoWire] reference used by the I2CDevice.
    TwoWire * wire();

    /// @brief Call [begin(addr_detect)] to initialize the I2CDevice 
    /// instance
    /// @param addr_detect If true, [begin] will try to detect whether
    /// the device address is available on the bus.
    /// 99% of sensors/devices don't mind, but once in a while they
    /// don't respond well to a scan!
    /// @return true if the instance was properly initialized.
    bool begin(bool addr_detect = true, 
            int sda = I2C_SDA, 
            int scl = I2C_SCL, 
            uint32_t frequency = I2C_FREQ);

    /// @brief returns true if the [I2CDeivce] has been initialized;
    /// @return true if the [I2CDeivce] has been initialized;
    bool isInitialized();

    /// @brief Calls [wire.close()] and sets _begun = false.
    void end(void);

    /// @brief Checks the bus for the presence of a device with
    /// address [_addr].
    /// @return true if [_addr] is detected on the bus.
    bool detected(void);
   
    /// @brief  Read from I2C into a buffer from the I2C device.
    /// Cannot be more than maxBufferSize() bytes.
    /// @param  buffer Pointer to buffer of data to read into
    /// @param  len Number of bytes from buffer to read.
    /// @param  stop Whether to send an I2C STOP signal on read
    /// @return True if read was successful, otherwise false.
    bool read(uint8_t *buffer, size_t len, bool stop = true);

    // /// @brief  Reads num bytes from specified register into a given buffer
    // /// @param  reg Register
    // /// @param  buf Buffer
    // /// @param  num  Number of bytes
    // /// @param verbose Prints the results of the register poll 
    // /// to the serial port if true. Defaults to true.
    // /// @return Position after reading
    // uint8_t readRegister(uint8_t reg, 
    //              uint8_t *buf, 
    //              uint8_t num,
    //              bool verbose = false);

    // /// @brief Reads the register values from the device, starting
    // /// at [startReg] and reading [len] elements
    // /// @param buf The array to populate with the register values.
    // /// @param len The number of registers to read.
    // /// @param startReg The register starting addres to commence 
    // /// reading at.
    // /// @param verbose Prints the results of the register poll 
    // /// to the serial port if true. Defaults to true.
    // void readAllRegisters(byte * buf, 
    //                      uint8_t len, 
    //                       byte startReg = 0,
    //                       bool verbose = true);

    // /// @brief  Writes num bytes from specified buffer into a given register
    // /// @param  reg Register
    // /// @param  buf Buffer
    // /// @param  num Number of bytes
    // void write(uint8_t reg, uint8_t *buf, uint8_t num);

    // /// @brief  Writes specified value to given register
    // /// @param  reg Register to write to
    // /// @param  value Value to write
    // void write8(byte reg, byte value);

    // /// @brief  Reads 8 bits from specified register.
    // /// @param  reg Register to write from.
    // /// @return Value in register [reg].
    // uint8_t read8(byte reg);

    // /// @brief  Reads 32 bits from specified register.
    // /// @param  reg Register to write to
    // /// @return Value in register
    // uint32_t read32(uint8_t reg, bool bigEndian = true);

    // /// @brief  Reads 16 bits from specified register.
    // /// @param  reg Register to write to
    // /// @return Value in register
    // uint16_t read16(uint8_t reg, bool bigEndian = true);
    
    // /// @brief  Reads 16 bits from specified register.
    // /// @param  reg Register to write to
    // /// @return Value in register
    // uint16_t read16R(uint8_t reg);

    /// @brief  Write a buffer or two to the I2C device. Cannot be more than
    /// maxBufferSize() bytes.
    /// @param  buffer Pointer to buffer of data to write. This is const to
    ///  ensure the content of this buffer doesn't change.
    /// @param  len Number of bytes from buffer to write.
    /// @param  prefix_buffer Pointer to optional array of data to write before
    /// buffer. Cannot be more than maxBufferSize() bytes. This is const to
    /// ensure the content of this buffer doesn't change.
    /// @param  prefix_len Number of bytes from prefix buffer to write
    /// @param  stop Whether to send an I2C STOP signal on write
    /// @return True if write was successful, otherwise false.
    bool write(const uint8_t *buffer, 
               size_t len, 
               bool stop = true,
                const uint8_t *prefix_buffer = nullptr, 
                size_t prefix_len = 0);


    // /// @brief Writes the bytes in [buf] to the device starting
    // /// at regeister address [reg]
    // /// @param reg The 8-bit register address to write to.
    // /// @param buf Pointer to buffer of data to write. This is const to
    // ///  ensure the content of this buffer doesn't change.   
    // /// @param  stop Whether to send an I2C STOP signal on write
    // /// @param  len Number of bytes from buffer to write.
    // /// @return true if the data in [buf]  were written.
    // bool write(uint8_t reg, 
    //            const uint8_t * buf, 
    //            size_t len, 
    //            bool stop = true);

    /// @brief Writes a single byte to the I2C device.
    /// @param val The byte to write.
    /// @return true if the byte was written.
    bool write(uint8_t val, 
               bool stop = true);

    // /// @brief Writes a 16 bit value to the I2C device.
    // /// @param val The 16 bit value to write.
    // /// @param bigEndian If true the data is sent with the highest 
    // /// byte first
    // /// @return true if the 16 bit value was written.
    // bool write(uint16_t val, 
    //            bool bigEndian = true, 
    //            bool stop = true);

    // /// @brief Writes a 32 bit value to the I2C device.
    // /// @param val The 32 bit value to write.
    // /// @param bigEndian If true the data is sent with the highest 
    // /// byte first
    // /// @return true if the 32 bit value was written.
    // bool write(uint32_t val, 
    //            bool bigEndian = true, 
    //            bool stop = true);

    /// @brief  Write some data, then read some data from I2C into another buffer.
    /// Cannot be more than maxBufferSize() bytes. The buffers can point to
    /// same/overlapping locations.
    /// @param  write_buffer Pointer to buffer of data to write from
    /// @param  write_len Number of bytes from buffer to write.
    /// @param  read_buffer Pointer to buffer of data to read into.
    /// @param  read_len Number of bytes from buffer to read.
    /// @param  stop Whether to send an I2C STOP signal between the write and read
    /// @return True if write & read was successful, otherwise false.
    bool write_then_read(const uint8_t *write_buffer, size_t write_len,
                uint8_t *read_buffer, size_t read_len,
                bool stop = false);
    
    /// @brief  Change the I2C clock speed to desired. Relies on
    /// underlying Wire support!
    /// @param desiredclk The desired I2C SCL frequency
    /// @return True if this platform supports changing I2C speed.
    /// Not necessarily that the speed was achieved!
    bool setSpeed(uint32_t desiredclk);

    /*!   @brief Returns the maximum number of bytes that can be 
    /// read in a transaction..
    /// @return The size of the Wire receive/transmit buffer */
    size_t maxBufferSize() { return _maxBufferSize; }

    /// @brief Poll all addresses on [i2c_wire] and populate an 
    /// array of the addresses that respond.
    /// @param devices Array that will be populated with the 
    /// addresses of the attached I2C devices.
    /// @param verbose Prints the results of the device poll 
    /// to the serial port if true. Defaults to true.
    /// @return the number of devices that responded.
    uint8_t listDevices(byte * devices, 
                        bool verbose = true);

    /// @brief Returns a string from the byte [b].
    /// @param b The byte to stringify.
    /// @return A string from the byte [b].
    static String getByteString(byte b, 
        uint8_t format = HEX, 
        bool addPrefix = true);

private:
    
    /// @brief 
    uint8_t _addr;

    /// @brief 
    TwoWire *_wire;

    /// @brief 
    bool _begun;

    /// @brief 
    size_t _maxBufferSize;

    /// @brief 
    /// @param buffer 
    /// @param len 
    /// @param stop 
    /// @return 
    bool _read(uint8_t *buffer, size_t len, bool stop);

};

#endif // IC2_DEVICE_H_
