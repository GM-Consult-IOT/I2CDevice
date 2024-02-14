
// specify the SDA and SCL pins if not standard
#define I2C_SDA 21 // default SDA pin on the ESP32
#define I2C_SCL 22 // default SCL pin on the ESP32
#define APDS_ADDR 0x39 // I2C address for an APDS9930 sensor.
#define LED_OUTPUT_PIN 26 // Attach an LED to GPIO26, with a current limiting resistor.
  
#define REG_COUNT 32 // number of registers on the device
#define READ_CMD 0xA0 // prefix for read commands to the APDS9930
#define ID_REG_ADDR 0x12 // register address for "ID" on the APDS9930

#include <Arduino.h>

// include the library in your main.cpp
#include <I2CDevice.h>

/// @brief List of connected I2C device addresses.
byte devices[9]; 

/// @brief The current on-state of the LED
bool ledStateOn = true;

/// @brief The I2CDevice instance to be tested.
I2CDevice i2c(APDS_ADDR, &Wire);

/// @brief Provide power to the I2C bus.
void powerUp();

/// @brief Prints the value in register 0X12, the ID address.
void printReg0x12();

/// @brief Prints the values of all the registers from 0X00 to 0x1F.
void printRegisters();

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

    // list all the devices on the bus
    i2c.listDevices(devices);    

    // print the value of register 0x12
    printReg0x12();

    // print the values in registers 0x00 - 0x1F
    printRegisters();

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


void printReg0x12(){ 
  byte regValues[1];  
  byte pref[1] = {ID_REG_ADDR | READ_CMD};   
  i2c.write_then_read(pref, 1, regValues, 1, true);
  Serial.printf("Returned 0X%02X from register 0X%02X (ID)\n",
     regValues[0], ID_REG_ADDR);

}

void printRegisters(){  
  byte regValues[REG_COUNT];  
  byte pref[1] = {READ_CMD};   
  i2c.write_then_read(pref, 1, regValues, REG_COUNT, true); 
  Serial.println("___________________________________");    
  Serial.println("REGISTER                    VALUE");
  Serial.println("-----------------------------------");
  for (uint8_t i = 0; i < REG_COUNT; i++){      
      Serial.printf( " %s              %s (%s)\n", 
          I2CDevice::getByteString(i, HEX), 
          I2CDevice::getByteString(regValues[i], BIN), 
          I2CDevice::getByteString(regValues[i], HEX));  
  }
}
