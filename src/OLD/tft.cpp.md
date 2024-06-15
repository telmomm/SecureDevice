/**************************************************************************
  This is a library for several Adafruit displays based on ST77* drivers.

  Works with the Adafruit ESP32-S2 TFT Feather
    ----> http://www.adafruit.com/products/5300

  Check out the links above for our tutorials and wiring diagrams.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 **************************************************************************/

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7789
#include <SPI.h>
#include "heltec_tracker_v11.h"

// Use dedicated hardware SPI pins
Adafruit_ST7735 tft = Adafruit_ST7735(ST7735_CS_Pin, ST7735_DC_Pin, ST7735_MOSI_Pin, ST7735_SCLK_Pin, ST7735_REST_Pin);


void setup(){
    pinMode(VEXT_CTRL, OUTPUT);
    digitalWrite(VEXT_CTRL, HIGH); // Set Vext to high to enable the display    

    tft.initR(INITR_MINI160x80_PLUGIN);   // initialize a ST7735S chip, mini display
    tft.setRotation(1);
    tft.fillScreen(ST77XX_BLACK);
    tft.setCursor(0, 0);
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(2);
    tft.print("SECURE GUARDIAN");
   
}

void loop(){


}