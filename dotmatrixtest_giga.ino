// this code is seccessfully ecxecuted on arduino giga r1 with dotmatrix
//for using spi with arduino giga we need to use spi.being function
//arduino giga have 2 spi : spi  1 (labled as spi5) and spi2 (labled as spi1) (headers) 
//spi1 uses 5v logic and spi2 uses 3.3v logic
//consider using  D prefix labling logic


#include <MD_MAX72xx.h>
#include <MD_Parola.h>
#include <SPI.h>

// Define hardware type
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4

// Set to 1 if you're using hardware SPI interface, 0 for software SPI
#define USE_HW_SPI 0

// Pin definitions for GIGA R1 SPI5 bus (explicit software SPI implementation)
#if USE_HW_SPI == 0
  #define DATA_PIN 11  // COPI/MOSI pin (D11)
  #define CLK_PIN 13   // SCK pin (D13)
  #define CS_PIN 10    // CS pin (D10)
  MD_Parola myDisplay = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);
#else
  #define CS_PIN 10    // CS pin (D10)
  MD_Parola myDisplay = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
#endif

void setup() {
  Serial.begin(115200);  // Start serial for debugging
  delay(2000);  // Longer delay for GIGA R1 stability
  Serial.println("Starting LED Matrix Display...");
  
  #if USE_HW_SPI == 1
    SPI.begin();  // Initialize hardware SPI
  #endif
  
  myDisplay.begin();
  myDisplay.setIntensity(10);  // Set brightness (0-15), reduced for stability
  myDisplay.displayClear();
  myDisplay.displayText("TEST", PA_CENTER, 100, 2000, PA_PRINT, PA_NO_EFFECT);
  myDisplay.displayReset();

  Serial.println("Display initialized");
}

void loop() {
  if (myDisplay.displayAnimate()) {
    static int displayStage = 0;
    myDisplay.displayClear();
    
    Serial.print("Display stage: ");
    Serial.println(displayStage);
    
    switch (displayStage) {
      case 0:
        myDisplay.displayText("<3", PA_CENTER, 50, 1000, PA_GROW_UP, PA_GROW_DOWN);
        break;
      case 1:
        myDisplay.displayText("hey you", PA_CENTER, 50, 2000, PA_MESH, PA_MESH);
        break;
      case 2:
        myDisplay.displayText("hello", PA_LEFT, 50, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
        break;
      case 3:
        myDisplay.displayText("hiii", PA_LEFT, 50, 0, PA_SCROLL_LEFT, PA_MESH);
        break;
      case 4:
        myDisplay.displayText("1234", PA_CENTER, 50, 2000, PA_SCROLL_LEFT, PA_WIPE);
        break;
    }
    displayStage = (displayStage + 1) % 5;  
    myDisplay.displayReset();
  }
}
