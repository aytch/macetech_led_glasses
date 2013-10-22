
// AS1130 LED Shades
// Designed for 8MHz 3.3V ATMega328P

#include <Wire.h>            // I2C library
#include <avr/pgmspace.h>    // Flash storage of long strings and fonts

// I2C addresses for AS1130 chips
#define glassesRight        0x37
#define glassesLeft         0x30

// Initial brightness value
#define startbrightness     153  

// Buffers for graphics generation
byte GlassesBits[24][2] = {0};    // 24 column x 8 row bit arrays (on/off frame)
byte GlassesPWM[24][9] = {};      // 24 column x 8 row byte array (PWM frame)

// Global variables
boolean patternInit = false;

// Data tables stored in flash memory
#include "messages.h"          // Message strings
#include "cie1931.h"           // CIE 1931 brightness lookup table
#include "quartersine.h"        // sine lookup table
#include "pcbiosfont.h"        // 8x8 bitmap font

// Helper functions for AS1130 interfacing and graphics generation
#include "AS1130functions.h"   // Generic AS1130 functions
#include "glassesfunctions.h"  // Glasses-specifc functions
#include "glassespatterns.h"   // Graphical patterns

// Begin program
void setup()
{

  Wire.begin();                // Start I2C
  TWBR = 2;                    // Set to 400KHz bus speed (on 8MHz device)

  glassesInit();               // Preconfigure AS1130 chips

  pinMode(3, INPUT_PULLUP);    // Prepare button input
  pinMode(4, INPUT_PULLUP);   // Prepare button input
  
  //Serial.begin(9600);
  
  setupTimerInterrupt();       // Start 5ms interrupt for button debouncing

}

boolean autoCycle = true;
boolean lastHeld = false;
unsigned long autoTimer = 0;
byte currentPattern = 0;

int brightact = 0;
byte charShow = 0;

// Main loop
// Incomplete
void loop() 
{

  if (((millis() - autoTimer > 15000) && autoCycle == true) || onButtonPressed(0)) {
    currentPattern++;
    if (currentPattern > 13) currentPattern = 0;
    autoTimer = millis();
    patternInit = false;
  }
  
  if (onButtonPressed(1)) cycleBrightness();

  if (!lastHeld && onButtonHeld(0)) {
    autoCycle = !autoCycle;
    lastHeld = true;
    if (autoCycle) {
      switchDrawType(0,0);
      displayChar('A');
      delay(500);
      patternInit = false;
    } else {
      switchDrawType(0,0);
      displayChar('M');
      delay(500);
      patternInit = false;
    }
  } else if (lastHeld && !onButtonHeld(0)) {
    lastHeld = false;
  }

  switch(currentPattern) {

  case 0:
      sideRain(0);
    break;

  case 1:
    fire();
    break;
   
  case 2:
    scrollMessage(0, SCROLL2X);
    break;
  
  case 3:
    starField();
    break;
    
  case 4:
    sines();
    break;
    
  case 5:
    scrollMessage(1, SCROLL2X);
    break;
    
  case 6:
    slantBars();
    break;
  
  case 7:
    sparkles();
    break;
    
  case 8:
    scrollMessage(2, SCROLL2X);
    break;

  case 9:
    rain();
    break;
    
  case 10:
    scrollMessage(3, SCROLL2X);
    break;

  case 11:
    Plasma();
    break;
  
  case 12:
    rider();
    break;
    
  case 13:
    scrollMessage(4, SCROLL2X);
    break;

  }

  /*
  readBrightness();
  brightact++;
  if (brightact > 10) {
    brightact = 0;
    //Serial.println(smoothedBrightness);
    
    int autoBright = smoothedBrightness;
    if (autoBright < 50) autoBright = 50;
    if (autoBright > 255) autoBright = 255;
    setBrightness(glassesRight, autoBright);
    setBrightness(glassesLeft, autoBright);
    
  }
  */


  delay(1);
}
