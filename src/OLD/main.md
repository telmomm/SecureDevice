///***--------------------------***///
///***   LIBRARIES DEFINITION   ***///
///***--------------------------***///
#include <Arduino.h>

// RADIO (Lora)
#include <SPI.h>
#include <RadioLib.h>

// DISPLAY
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7789
#include <SPI.h>
#include "heltec_tracker_v11.h"
#include <U8g2lib.h>
#include <Wire.h>

// GPS
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>

// MPU6050
#include "I2Cdev.h"
#include "MPU6050.h"

// INTERNAL LIBRARIES
#include "VARS.h"
#include "functions.h"


///***-----------------------***///
///***   OBJECT DEFINITION   ***///
///***-----------------------***///
// Lora

SPIClass radioSPI(HSPI);
SPISettings radioSPISettings(8E6, MSBFIRST, SPI_MODE0);
SX1262 radio = new Module(RADIO_NSS, RADIO_DIO_1, 1, RADIOLIB_NC);

//Config the radio spi pins


LoRaWANNode node(&radio, &EU868);
  //device address - this number can be anything
uint32_t devAddr = 0x260B34C5;
  //network key is the ASCII string "topSecretKey1234"
uint8_t nwkSKey[] = { 0xF4, 0x73, 0x50, 0xCC, 0xF4, 0x94, 0xE3, 0x03, 0xA5, 0xC9, 0x6F, 0xD5, 0x06, 0x4A, 0x3F, 0x10 };
  //application key is the ASCII string "aDifferentKeyABC"
uint8_t appSKey[] = {0xA2, 0xE2, 0x00, 0xFE, 0x54, 0x08, 0x95, 0xE6, 0x14, 0x60, 0x8E, 0x7A, 0xE0, 0x06, 0xD3, 0x23};

// DISPLAY
//U8G2_SSD1306_128X64_NONAME_F_HW_I2C display(U8G2_R0, RST_PIN, SCL_PIN, SDA_PIN);
//U8G2_SSD1306_160X80_NONAME_F_HW_I2C display(U8G2_R0, RST_PIN, SCL_PIN, SDA_PIN);
U8G2_SSD1320_160X80_F_HW_I2C display (U8G2_R0, RST_PIN, SCL_PIN, SDA_PIN);
// GPS
// The TinyGPSPlus object
//TinyGPSPlus gps;
// The serial connection to the GPS device
//SoftwareSerial ss(RXPin, TXPin);
char nmeaBuffer[85];
MicroNMEA nmea(nmeaBuffer, sizeof(nmeaBuffer));


// MPU6050 (Accelerometer)
// class default I2C address is 0x68
MPU6050 accelgyro(0x68);

///***------------------------***///
///***   INTERNAL FUNCTIONS   ***///
///***------------------------***///
void mpuWakeUp() {
    //Wake up MPU module
    accelgyro.setSleepEnabled(false);
}

void handleButtonPress() {
  int state;
  DEBUG_PRINTLN("Button pressed");
  sendMessage("Hello", radio, node);

}

///***--------------------***///
///***   SETUP FUNCTION   ***///
///***--------------------***///
void setup(){
  delay(initialDelay);
  showVersions();
  wirelessTracker1Setup();
  Serial.begin(serialBaudrate);
  Serial.println("[SERIAL] Serial communication started");

    //Display
  //initDisplay(display);
  //displayMessage(display, "Sec Guardian");

  //Radio module (Lora)
  //TTNVars TTNVars = TTNVariableConversion(devAddrStr, nwkSKeyStr, appSKeyStr);
  TTNVars vars;
  uint32_t devAddr1 = 0x260B34C5;
  uint8_t nwkSKey1[] = { 0xF4, 0x73, 0x50, 0xCC, 0xF4, 0x94, 0xE3, 0x03, 0xA5, 0xC9, 0x6F, 0xD5, 0x06, 0x4A, 0x3F, 0x10 };
  uint8_t appSKey1[] = {0xA2, 0xE2, 0x00, 0xFE, 0x54, 0x08, 0x95, 0xE6, 0x14, 0x60, 0x8E, 0x7A, 0xE0, 0x06, 0xD3, 0x23};

  vars.devAddr = devAddr;
  memcpy(vars.nwkSKey, nwkSKey, sizeof(nwkSKey));
  memcpy(vars.appSKey, appSKey, sizeof(appSKey));
  //initRadio(radio, node, vars, 1);
 
  //GPS Module
  //initGPS(1);
  //init_gnss();
  
  //MPU6050 (Accelerometer)
  //initMPU6050(accelgyro);

  //Set Interrupts
  pinMode(MPUInterruptPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(MPUInterruptPin), mpuWakeUp, RISING);

  pinMode(ButtonInteruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ButtonInteruptPin), handleButtonPress, FALLING);

  DEBUG_PRINTLN(radio.begin());

}
int count = 0;

Coordinates coordinates;

void loop(){
  //sendMessage(String(count), radio, node);
  //gps_test();


} 
