#include <Arduino.h>

///***----------------***///
///***   DEBUG MODE   ***///
///***----------------***///
#define DEBUG
#ifdef DEBUG
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTLN(x) Serial.println(x)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#endif

///***-----------***///
///***   BOARD   ***///
///***-----------***///
// Pin Out
#define VEXT 3
#define BOOT_MODE 37
// Various
int initialDelay = 3000;
int serialBaudrate = 115200;

///***-------------***///
///***   LORAWAN   ***///
///***-------------***///
// LoRaWAN Standard Definitions
#define DR_SF12 0
#define DR_SF7  7
#define DR_SF7B 8
#define BAND_CENTI 100

// Pin Out
#define SX1262_NSS 8//8
#define SX1262_DIO1 14//14
#define SX1262_NRST 12//12
#define SX1262_BUSY 13//13

///***-------------***///
///***   DISPLAY   ***///
///***-------------***///
// Pin Out
#define SDA_PIN 17  // Pin SDA (Serial Data) 17
#define SCL_PIN 18  // Pin SCL (Serial Clock) 18
#define RST_PIN 21  // Pin RST (Reset) 21


///***---------***///
///***   GPS   ***///
///***---------***///
// Pin Out
static const int RXPin = 46;//46
static const int TXPin = 45;//45

static const uint32_t GPSBaud = 9600;//9600;

///***-----------------------------***///
///***   MPU6050 (Accelerometer)   ***///
///***-----------------------------***///
// Pin Out
#define MPUInterruptPin 2
#define ButtonInteruptPin 0

#define I2CONE_SDA_PIN 19
#define I2CONE_SCL_PIN 20
long I2CONE_FREQ = 400000;

const int mpuAddress = 0x68;  