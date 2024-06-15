
///***--------------------------***///
///***   LIBRARIES DEFINITION   ***///
///***--------------------------***///
#include <Arduino.h>


// RADIO (Lora)
#include <SPI.h>
#include <RadioLib.h>

// DISPLAY
#include <U8g2lib.h>
#include <Wire.h>

// GPS
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>

// MPU6050
#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"

// INTERNAL LIBRARIES
#include "VARS.h"
#include "functions.h"

///***-----------------------***///
///***   OBJECT DEFINITION   ***///
///***-----------------------***///
// Lora
SX1262 radio = new Module(SX1262_NSS, SX1262_DIO1, SX1262_NRST, SX1262_BUSY);
LoRaWANNode node(&radio, &EU868);
  //device address - this number can be anything
uint32_t devAddr = 0x260B34C5;
  //network key is the ASCII string "topSecretKey1234"
uint8_t nwkSKey[] = { 0xF4, 0x73, 0x50, 0xCC, 0xF4, 0x94, 0xE3, 0x03, 0xA5, 0xC9, 0x6F, 0xD5, 0x06, 0x4A, 0x3F, 0x10 };
  //application key is the ASCII string "aDifferentKeyABC"
uint8_t appSKey[] = {0xA2, 0xE2, 0x00, 0xFE, 0x54, 0x08, 0x95, 0xE6, 0x14, 0x60, 0x8E, 0x7A, 0xE0, 0x06, 0xD3, 0x23};

// DISPLAY
U8G2_SSD1306_128X64_NONAME_F_HW_I2C display(U8G2_R0, RST_PIN, SCL_PIN, SDA_PIN);
//U8G2_SSD1320_160X80_F_HW_I2C display (U8G2_R0, RST_PIN, SCL_PIN, SDA_PIN);

// GPS
// The TinyGPSPlus object
TinyGPSPlus gps;
// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

// MPU6050 (Accelerometer)
// class default I2C address is 0x68
MPU6050 accelgyro(mpuAddress);
TwoWire I2Cone = TwoWire(1);






///***-------------------***///
///***   INTERNAL VARS   ***///  
///***-------------------***///
float batterySOC = 100;
//LoraWAN
char* devAddrStr = getenv("devAddr");
char* nwkSKeyStr = getenv("nwkSKey");
char* appSKeyStr = getenv("appSKey");
//uint32_t devAddr;
//uint8_t nwkSKey[16];
//uint8_t appSKey[16];

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

///***-------------------***///
///***   SETUP FUNCTION   ***///
///***-------------------***///
void setup() {
  //Serial
  Serial.begin(serialBaudrate);
  Serial.println("Starting...");

  //Display
  initDisplay(display);
  displayMessage(display, "Sec Guardian");

  //Radio module (Lora) 
  //TTNVars TTNVars = TTNVariableConversion(devAddrStr, nwkSKeyStr, appSKeyStr);
  TTNVars vars;
  uint32_t devAddr1 = 0x260B34C5;
  uint8_t nwkSKey1[] = { 0xF4, 0x73, 0x50, 0xCC, 0xF4, 0x94, 0xE3, 0x03, 0xA5, 0xC9, 0x6F, 0xD5, 0x06, 0x4A, 0x3F, 0x10 };
  uint8_t appSKey1[] = {0xA2, 0xE2, 0x00, 0xFE, 0x54, 0x08, 0x95, 0xE6, 0x14, 0x60, 0x8E, 0x7A, 0xE0, 0x06, 0xD3, 0x23};

  vars.devAddr = devAddr;
  memcpy(vars.nwkSKey, nwkSKey, sizeof(nwkSKey));
  memcpy(vars.appSKey, appSKey, sizeof(appSKey));
  //initRadio(radio, node, vars, 2);
  //initLora(node);

  Serial.print(F("[SX1262] Initializing ... "));
  int state = radio.begin();
  if(state == RADIOLIB_ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    //while(true);
  }

  // if using EU868 on ABP in TTN, you need to set the SF for RX2 window manually
  //node.rx2.drMax = 3;

  // start the device by directly providing the encryption keys and device address
  Serial.print(F("[LoRaWAN] Attempting Activation Over Personalization ... "));
  state = node.beginAPB(devAddr, nwkSKey, appSKey);
  if(state == RADIOLIB_ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    //while(true);
  }


  initGPS(ss, GPSBaud);

  //MPU6050 (Accelerometer)
  //initMPU6050(accelgyro);

  //Set Interrupts
  pinMode(MPUInterruptPin, INPUT);
  //attachInterrupt(digitalPinToInterrupt(MPUInterruptPin), mpuWakeUp, RISING);

  pinMode(ButtonInteruptPin, INPUT_PULLUP);
  //attachInterrupt(digitalPinToInterrupt(ButtonInteruptPin), handleButtonPress, FALLING);
  Wire1.begin(I2CONE_SDA_PIN, I2CONE_SCL_PIN);

}
int count = 0;

///***-------------------***///
///***   LOOP FUNCTION   ***///
///***-------------------***///
void loop(){
byte error, address;
	int nDevices;
/*
	Serial.println("Scanning...");
  nDevices = 0;
	for(address = 1; address < 127; address++ )
	{
		Wire1.beginTransmission(address);
		error = Wire1.endTransmission();
*/
//		Wire1.beginTransmission(address);
//		error = Wire1.endTransmission();

/*
		if (error == 0)
		{
			Serial.print("I2C device found at address 0x");
			if (address<16)
			Serial.print("0");
			Serial.print(address,HEX);
			Serial.println("  !");

			nDevices++;
		}
		else if (error==4)
		{
			Serial.print("Unknown error at address 0x");
			if (address<16)
				Serial.print("0");
			Serial.println(address,HEX);
		}
	}
	if (nDevices == 0)
	Serial.println("No I2C devices found\n");
	else
	Serial.println("done\n");

	delay(5000);
*/

  //DEBUG_PRINTLN(count);

/*
  while(ss.available() > 0) {
    int data = ss.read();
    gps.encode(data);
    delay(500);
    if(gps.location.isValid()){
      DEBUG_PRINTLN("GPS Data");
      DEBUG_PRINTLN(gps.location.lat());
    }
    */
    /*
      if (gps.location.isValid()) {
        DEBUG_PRINTLN("GPS Data");
        DEBUG_PRINTLN(gps.location.lat());
      } else {
        DEBUG_PRINTLN("GPS location not valid");
      } 
      
  }

  //sendMessage(String(count), radio, node);
  //Coordinates coordinates = readGPS(ss, gps);

  // Si el MPU6050 está despierto, lee los datos
  /*
    if (!accelgyro.getSleepEnabled()) {
        int16_t ax, ay, az;
        accelgyro.getAcceleration(&ax, &ay, &az);

        // Pone el MPU6050 en modo sleep para ahorrar energía
        accelgyro.setSleepEnabled(true);
    }
    */

    sendMessage(String(count), radio, node);
    count++;
    delay(10000);

}

