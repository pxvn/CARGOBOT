#include <MD_MAX72xx.h>
#include <MD_Parola.h>
#include <SPI.h>

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CS_PIN 8

MD_Parola myDisplay = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

void setup() {
  myDisplay.begin();
  myDisplay.setIntensity(2); 
  myDisplay.displayClear();

  myDisplay.displayText("......", PA_CENTER, 50, 1500, PA_BLINDS, PA_NO_EFFECT);
  myDisplay.displayReset();
}

void loop() {
  if (myDisplay.displayAnimate()) {
    static int displayStage = 0;

    myDisplay.displayClear();
    switch (displayStage) {
      case 0:
        myDisplay.displayText("<3", PA_CENTER, 50, 1000, PA_GROW_UP, PA_GROW_DOWN);
        break;
      case 1:
        myDisplay.displayText("hey you", PA_CENTER, 50, 2000, PA_MESH, PA_MESH);
        break;
      case 2:
        myDisplay.displayText("hiii", 
                             PA_LEFT, 50, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
        break;
        case 3:
        myDisplay.displayText("???", 
                             PA_LEFT, 50, 0, PA_SCROLL_LEFT, PA_MESH);
        break;
      case 4:
        myDisplay.displayText("1234", PA_CENTER, 50, 2000, PA_SCROLL_LEFT, PA_WIPE);
        break;
    }

    displayStage = (displayStage + 1) % 5;  
    myDisplay.displayReset();
  }
}
