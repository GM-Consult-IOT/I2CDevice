
#include <Arduino.h>

// include the library in your main.cpp
#include <I2CDevice.h>

// specify the SDA and SCL pins if not standard
#define I2C_SDA 21 // default SDA pin on the ESP32
#define I2C_SCL 22 // default SCL pin on the ESP32
#define APDS_ADDR 0x39 // I2C address for and APDS9930 sensor.
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

