
#include <Arduino.h>
#include <I2CDevice.h>

#define I2C_SDA 21
#define I2C_SCL 22
#define APDS_ADDR 0x39
#define LED_OUTPUT_PIN 26   

/// @brief List of connected I2C device addresses.
byte devices[9]; 

byte regValues[0x1F];

/// @brief The I2CDevice instance to be tested.
I2CDevice i2c(APDS_ADDR, &Wire);

/// @brief Provide power to the I2C bus.
void powerUp();

void setup() {
  delay(50);
  powerUp();
  Serial.begin(115200);  
  while(!Serial){
    vTaskDelay(50/portTICK_PERIOD_MS);
  }
  Serial.println("Serial port is initialized."); 
  if (i2c.begin(true, I2C_SDA, I2C_SCL)){    
    Serial.printf("I2C Device initialized with "
        "address 0x%s\n", String(i2c.address(), HEX)); 
    i2c.listDevices(devices); 
    i2c.readAllRegisters(regValues, 0x1F);
  } else {    
    Serial.println("I2C Device FAILED to initialize!"); 
  }
}

bool ledStateOn = true;
uint8_t reg = 0;
void loop() {
  // let's flash the LED
  ledStateOn = !ledStateOn;
  digitalWrite(LED_OUTPUT_PIN, ledStateOn);
  delay(500);
}

void powerUp(){    
    pinMode(LED_OUTPUT_PIN, OUTPUT);
    // turn on the LED
    digitalWrite(LED_OUTPUT_PIN, HIGH);
}

