#include <Adafruit_NeoPixel.h>
#include <MD_MAX72xx.h>
#include <MD_Parola.h>
#include <SPI.h>

// NeoPixel Configuration
#define NP_PIN 7
#define NUMPIXELS 40
#define BRIGHTNESS 230
Adafruit_NeoPixel pixels(NUMPIXELS, NP_PIN, NEO_GRB + NEO_KHZ800);

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define USE_HW_SPI 0
#define DATA_PIN 30  // DIN (MOSI)
#define CLK_PIN 32   // CLK (SCK)
#define CS_PIN 31    // CS
MD_Parola myDisplay = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// System States
enum SystemState {
  SYS_BOOT,
  SYS_PROJECT,
  SYS_READY,
  SYS_ON_THE_WAY,
  SYS_LID_OPEN,
  SYS_LID_CLOSE
};
SystemState currentState = SYS_BOOT;

// NeoPixel Sections
#define FRONT_LEFT_START 0
#define FRONT_LEFT_END 4
#define LEFT_SIDE_START 5
#define LEFT_SIDE_END 14
#define BACK_LEFT_START 15
#define BACK_LEFT_END 19
#define BACK_RIGHT_START 20
#define BACK_RIGHT_END 24
#define RIGHT_SIDE_START 25
#define RIGHT_SIDE_END 34
#define FRONT_RIGHT_START 35
#define FRONT_RIGHT_END 39

// Animation Tracking
unsigned long stateStartTime = 0;
int leftChain[20], rightChain[20];
bool matrixInTransition = false;

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  // Initialize NeoPixels
  pixels.begin();
  pixels.setBrightness(BRIGHTNESS);
  pixels.show();
  createChains();

  // Initialize Dot Matrix
  myDisplay.begin();
  myDisplay.setIntensity(10);
  myDisplay.displayClear();

  // Start boot sequence
  startBootSequence();
}

void loop() {
  static unsigned long lidTimer = 0;
  bool matrixDone = myDisplay.displayAnimate();
  
  switch(currentState) {
    case SYS_BOOT:      handleBoot(matrixDone); break;
    case SYS_PROJECT:   handleProject(matrixDone); break;
    case SYS_READY:     handleReady(matrixDone); break;
    case SYS_ON_THE_WAY: handleOnTheWay(matrixDone); break;
    case SYS_LID_OPEN:  if(millis() - lidTimer > 2000) currentState = SYS_ON_THE_WAY; break;
    case SYS_LID_CLOSE: if(millis() - lidTimer > 2000) currentState = SYS_ON_THE_WAY; break;
  }

  updateNeoPixels();
}

void createChains() {
  int idx = 0;
  for(int i=FRONT_LEFT_START; i<=FRONT_LEFT_END; i++) leftChain[idx++] = i;
  for(int i=LEFT_SIDE_START; i<=LEFT_SIDE_END; i++) leftChain[idx++] = i;
  for(int i=BACK_LEFT_START; i<=BACK_LEFT_END; i++) leftChain[idx++] = i;

  idx = 0;
  for(int i=FRONT_RIGHT_START; i<=FRONT_RIGHT_END; i++) rightChain[idx++] = i;
  for(int i=RIGHT_SIDE_START; i<=RIGHT_SIDE_END; i++) rightChain[idx++] = i;
  for(int i=BACK_RIGHT_START; i<=BACK_RIGHT_END; i++) rightChain[idx++] = i;
}

void startBootSequence() {
  myDisplay.displayText("The Cargobot Project", PA_CENTER, 50, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
  stateStartTime = millis();
}

void handleBoot(bool matrixDone) {
  static bool neoBootDone = false;
  
  // NeoPixel Boot Animation
  if(millis() - stateStartTime < 2000) {
    for(int i=0; i<20; i++) {
      if(millis() - stateStartTime > i*30) {
        pixels.setPixelColor(leftChain[i], pixels.Color(0, 0, 255));
        pixels.setPixelColor(rightChain[i], pixels.Color(0, 0, 255));
      }
    }
    pixels.show();
  } else if(!neoBootDone) {
    pixels.fill(pixels.Color(0, 255, 0));
    pixels.show();
    neoBootDone = true;
  }

  // Matrix transition
  if(matrixDone && neoBootDone) {
    currentState = SYS_PROJECT;
   myDisplay.displayText("Cargobot", PA_CENTER, 50, 2000, PA_MESH, PA_GROW_DOWN);
  }
}

void handleProject(bool matrixDone) {
  if(matrixDone) {
    currentState = SYS_READY;
    myDisplay.displayText("READY TO DELIVER!", PA_CENTER, 50, 2000, PA_SCROLL_LEFT, PA_SCROLL_RIGHT);
  }
}

void handleReady(bool matrixDone) {
  if(matrixDone) {
    currentState = SYS_ON_THE_WAY;
    stateStartTime = millis();
  }
}

void handleOnTheWay(bool matrixDone) {
  static bool showCargobot = true;
  
  if(matrixDone) {
    showCargobot = !showCargobot;
    myDisplay.displayText(showCargobot ? "Cargobot" : "On the way", 
                         PA_LEFT, 50, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
  }
}

void updateNeoPixels() {
  static unsigned long lastBreath = 0;
  static int breathValue = 0;
  static bool breathingUp = true;

  switch(currentState) {
    case SYS_ON_THE_WAY:
      if(millis() - lastBreath > 20) {
        breathValue += breathingUp ? 5 : -5;
        if(breathValue >= 255) breathingUp = false;
        if(breathValue <= 0) breathingUp = true;
        
        uint32_t color = ((millis() - stateStartTime)/5000 % 2 == 0) ? 
                         pixels.Color(0, 0, breathValue) : 
                         pixels.Color(breathValue, 0, 0);
        
        pixels.fill(color);
        pixels.show();
        lastBreath = millis();
      }
      break;

    case SYS_LID_OPEN:
      pixels.fill(pixels.Color(0, 0, 255));
      pixels.show();
      break;

    case SYS_LID_CLOSE:
      pixels.fill(pixels.Color(0, 255, 0));
      pixels.show();
      break;
  }
}

// Call these functions when lid state changes
void openLid() {
  currentState = SYS_LID_OPEN;
  myDisplay.displayText("LID OPEN", PA_CENTER, 50, 2000, PA_GROW_UP, PA_GROW_UP);
  stateStartTime = millis();
}

void closeLid() {
  currentState = SYS_LID_CLOSE;
  myDisplay.displayText("LID CLOSE", PA_CENTER, 50, 2000, PA_GROW_DOWN, PA_GROW_DOWN);
  stateStartTime = millis();
}
