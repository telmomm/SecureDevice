#include <Arduino.h>

// DISPLAY
#include <U8g2lib.h>
#include <Wire.h>

// GPS
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>

// MPU6050
#include "I2Cdev.h"
#include "MPU6050.h"


///***-----------***///
///***   BOARD   ***///
///***-----------***///
void wirelessTracker1Setup(){
  pinMode(VEXT, OUTPUT);
  pinMode(BOOT_MODE,OUTPUT);
	
  digitalWrite(VEXT, HIGH);
  digitalWrite(BOOT_MODE, LOW);
}

void showVersions(){
  DEBUG_PRINTLN("[LIBRARY VERSIONS]");
  DEBUG_PRINTLN("   ARDUINO " + String(ARDUINO));
  DEBUG_PRINTLN("   TINY GPS " + String(TinyGPSPlus::libraryVersion()));
}


float readBatteryLevel(int VBATPIN){
  uint32_t raw = analogRead(VBATPIN);
  float voltage = raw * 3.3 / 4095.0;
  float maxVoltage = 3.6;
  float minVoltage = 2.55;
  float percentage = (voltage - minVoltage) / (maxVoltage - minVoltage) * 100;
  return percentage;
}

///***-----------------------***///
///***   DISPLAY FUNCTIONS   ***///
///***-----------------------***///


/// @brief: Initialize the display
/// @param: U8G2 &u8g2 - The display object
/// @return: void
void initDisplay(U8G2 &u8g2){
  
  DEBUG_PRINT("[DISPLAY] Initializing ... ");
  if(u8g2.begin()){
    DEBUG_PRINTLN("success!");
  } else {
    DEBUG_PRINTLN("failed!"); 
  }
}

/// @brief  Display a message on the screen
/// @param: U8G2 &u8g2 - The display object
/// @param: String message - The message to display
/// @return: void
void displayMessage(U8G2 &u8g2, String message){
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_10x20_tf);

  int screenHeight = u8g2.getDisplayHeight();
  int textHeight = u8g2.getFontAscent() - u8g2.getFontDescent();
  int yPos = (screenHeight - textHeight) / 2;

  int screenWidth = u8g2.getDisplayWidth();
  int textWidth = u8g2.getStrWidth(message.c_str());
  int xPos = (screenWidth - textWidth) / 2;

  u8g2.drawStr(xPos, yPos, message.c_str());
  u8g2.sendBuffer();
}

///***--------------------***///
///***   LORA FUNCTIONS   ***///
///***--------------------***///
typedef struct {
  uint32_t devAddr;
  uint8_t nwkSKey[16];
  uint8_t appSKey[16];
} TTNVars;

TTNVars TTNVariableConversion(char* devAddrStr, char* nwkSKeyStr, char* appSKeyStr){
  printf("[TTNVariableConversion] initializing ... ");
  TTNVars vars;

  //devAddr variable conversion
  sscanf(devAddrStr, "%x", &vars.devAddr);

  //nwkSKey variable conversion
  for(int i = 0; i < 16; i++) {
    unsigned int value;
    sscanf(nwkSKeyStr + 3*i, "%2x", &value);
    vars.nwkSKey[i] = value;
  }

  //appSKey variable conversion
  for(int i = 0; i < 16; i++) {
    unsigned int value;
    sscanf(appSKeyStr + 3*i, "%2x", &value);
    vars.appSKey[i] = value;
  }

  return vars;
}


/// @brief: Initialize the radio module
/// @param: SX1262 &radio - The radio object
/// @param: LoRaWANNode &node - The LoRaWAN node object
/// @param: char* devAddrStr - The device address string
/// @param: char* nwkSKeyStr - The network session key string
/// @param: char* appSKeyStr - The application session key string
/// @param: uint32_t devAddr - The device address
/// @param: uint8_t nwkSKey[16] - The network session key
/// @param: uint8_t appSKey[16] - The application session key
/// @return: void
void initRadio(SX1262 &radio, LoRaWANNode &node, TTNVars TTNVars, int retryCount){
  // initialize SX1262 with default settings
  DEBUG_PRINT(F("[SX1262] Initializing ... "));
  int state;
  for(int i = 0; i < retryCount; i++) {
    state = radio.begin();
    if(state == RADIOLIB_ERR_NONE) {
      DEBUG_PRINTLN(F("success!"));
      return;
    } else {
      DEBUG_PRINT(F("failed, code "));
      DEBUG_PRINTLN(state);
    }
  }
  // If we reach this point, all retries have failed
  while(true);

  DEBUG_PRINT(F("[SX1262] Sleeping module ... "));
  //state = radio.sleep();
  if(state == RADIOLIB_ERR_NONE) {
    DEBUG_PRINTLN(F("success!"));
  } else {
    DEBUG_PRINT(F("failed, code "));
    DEBUG_PRINTLN(state);
    while(true);
  }
;}

void initLora(LoRaWANNode &node){
  int state;
// if using EU868 on ABP in TTN, you need to set the SF for RX2 window manually
  //node.rx2.drMax = 3;

  // start the device by directly providing the encryption keys and device address
  uint32_t devAddr = 0x260B34C5;
  uint8_t nwkSKey[] = { 0xF4, 0x73, 0x50, 0xCC, 0xF4, 0x94, 0xE3, 0x03, 0xA5, 0xC9, 0x6F, 0xD5, 0x06, 0x4A, 0x3F, 0x10 };
  uint8_t appSKey[] = { 0xA2, 0xE2, 0x00, 0xFE, 0x54, 0x08, 0x95, 0xE6, 0x14, 0x60, 0x8E, 0x7A, 0xE0, 0x06, 0xD3, 0x23 };

  DEBUG_PRINT(F("[LoRaWAN] Initializing ... "));
  state = node.beginAPB(devAddr, nwkSKey, appSKey);
  if(state == RADIOLIB_ERR_NONE) {
    DEBUG_PRINTLN(F("success!"));
  } else {
    DEBUG_PRINT(F("failed, code "));
    DEBUG_PRINTLN(state);
    //while(true);
  }
  //Status Codes: https://jgromes.github.io/RadioLib/group__status__codes.html
}

/// @brief: Send a message via LoRaWAN
/// @param: String message - The message to send
/// @param: SX1262 &radio - The radio object
/// @param: LoRaWANNode &node - The LoRaWAN node object
/// @return: void
void sendMessage(String message, SX1262 &radio, LoRaWANNode &node){
  DEBUG_PRINT(F("[LoRaWAN] Sending uplink packet ... "));
  String strDown;
  //int state = node.sendReceive(strUp, 10, strDown);
  int state = node.uplink(message, 10);
  if(state == RADIOLIB_ERR_NONE) {
    DEBUG_PRINTLN(F("received a downlink!"));

    // print data of the packet (if there are any)
    DEBUG_PRINT(F("[LoRaWAN] Data:\t\t"));
    if(strDown.length() > 0) {
      DEBUG_PRINTLN(strDown);
    } else {
      DEBUG_PRINTLN(F("<MAC commands only>"));
    }

    // print RSSI (Received Signal Strength Indicator)
    DEBUG_PRINT(F("[LoRaWAN] RSSI:\t\t"));
    DEBUG_PRINT(radio.getRSSI());
    DEBUG_PRINTLN(F(" dBm"));

    // print SNR (Signal-to-Noise Ratio)
    DEBUG_PRINT(F("[LoRaWAN] SNR:\t\t"));
    DEBUG_PRINT(radio.getSNR());
    DEBUG_PRINTLN(F(" dB"));

    // print frequency error
    DEBUG_PRINT(F("[LoRaWAN] Frequency error:\t"));
    DEBUG_PRINT(radio.getFrequencyError());
    DEBUG_PRINTLN(F(" Hz"));
  
  } else if(state == RADIOLIB_ERR_RX_TIMEOUT) {
    DEBUG_PRINTLN(F("no downlink!"));
  
  } else {
    DEBUG_PRINT(F("failed, code "));
    DEBUG_PRINTLN(state);
  }
}

///***-----------------------------***///
///***   MPU6050 (Accelerometer)   ***///
///***-----------------------------***///

void initMPU6050(MPU6050 &accelgyro){
  DEBUG_PRINT("[Wire] Initializing ... ");
  if(Wire.begin()){
    DEBUG_PRINTLN("success!");
  } else {
    DEBUG_PRINTLN("failed!");
  };

  DEBUG_PRINT("[MPU6050] Initializing ... ");
  accelgyro.initialize();
  DEBUG_PRINTLN(accelgyro.testConnection() ? "success" : "failed");
  //Configure MPU in sleep mode
  accelgyro.setSleepEnabled(true);
  // Configura el MPU6050 para que genere una interrupción cuando detecte movimiento
    accelgyro.setMotionDetectionThreshold(2); // Configura el umbral de detección de movimiento
    accelgyro.setZeroMotionDetectionDuration(2); // Configura la duración de la detección de movimiento
    accelgyro.setInterruptMode(true); // Habilita la interrupción de detección de movimiento
}

///***----------***///
///***   GPS   ***///
///***----------***///
struct Coordinates {
  double latitude;
  double longitude;
};


/// @brief Initialize the GPS module
/// @param ss - SoftwareSerial object
/// @param GPSBaud - GPS Baudrate
/// @return void 
void initGPS(SoftwareSerial &ss, int GPSBaud){
  
  ss.begin(GPSBaud);
  DEBUG_PRINT("[GPS] Initializing ... ");

  // Espera un poco para que el GPS comience a enviar datos
  delay(2000);

  if(ss.available()){
    DEBUG_PRINTLN("success!");
  } else {
    DEBUG_PRINTLN("failed, GPS not detected");
  }
}

/// @brief Read the GPS coordinates
/// @param ss - SoftwareSerial object
/// @param gps - GPS object
/// @return Coordinates struct (longitude, latitude)
Coordinates readGPS(SoftwareSerial &ss, TinyGPSPlus &gps) {
  while (ss.available() > 0) {
    if (gps.encode(ss.read())) {
      if (gps.location.isUpdated()) {
        Coordinates coordinates = {gps.location.lat(), gps.location.lng()};
        DEBUG_PRINTLN("Latitude: " + String(coordinates.latitude) + " Longitude: " + String(coordinates.longitude));
        return coordinates;
      }
    }
  }
  return {0, 0};
}