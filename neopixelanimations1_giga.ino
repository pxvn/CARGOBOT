#include <Adafruit_NeoPixel.h>

#define PIN 7          // NeoPixel data pin (D7)
#define NUMPIXELS 40   // Total number of LEDs
#define BRIGHTNESS 230 // Brightness (0-255, set to 230 per requirement)

Adafruit_NeoPixel strip(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// Color definitions
uint32_t WHITE = strip.Color(230, 230, 230);
uint32_t BLUE = strip.Color(0, 0, 230);
uint32_t RED = strip.Color(230, 0, 0);
uint32_t YELLOW = strip.Color(230, 230, 0);
uint32_t OFF = strip.Color(0, 0, 0);

// State enum for robot states
enum RobotState {
  BOOT,
  IDLE,
  BACKWARD,
  FORWARD,
  STOP,
  LID_OPEN,
  LID_CLOSE,
  LID_CLOSED
};

void setup() {
  strip.begin();
  strip.setBrightness(BRIGHTNESS);
  strip.show(); // Initialize all pixels to off
}

void loop() {
  // Simulate state changes for demo (replace with actual state logic)
  animateBoot();
  delay(2000);
  animateIdle(10000); // Run idle for 10s
  animateBackward(5000);
  animateForward(5000);
  animateStop();
  animateLidOpen();
  animateLidClose();
  animateLidClosed();
}

void animateBoot() {
  strip.fill(OFF);
  strip.show();
  
  // Random blue fill
  int lit[NUMPIXELS];
  for (int i = 0; i < NUMPIXELS; i++) lit[i] = 0;
  int count = 0;
  while (count < NUMPIXELS) {
    int idx = random(NUMPIXELS);
    if (lit[idx] == 0) {
      strip.setPixelColor(idx, BLUE);
      lit[idx] = 1;
      count++;
      strip.show();
      delay(30);
    }
  }
  
  // Soft flash (3 pulses)
  for (int i = 0; i < 3; i++) {
    for (int b = BRIGHTNESS; b >= 150; b -= 10) {
      strip.setBrightness(b);
      strip.show();
      delay(20);
    }
    for (int b = 150; b <= BRIGHTNESS; b += 10) {
      strip.setBrightness(b);
      strip.show();
      delay(20);
    }
  }
  strip.setBrightness(BRIGHTNESS);
}

void animateIdle(unsigned long duration) {
  unsigned long startTime = millis();
  int auraPos = 0;
  unsigned long lastSparkle = 0;
  
  while (millis() - startTime < duration) {
    // Set white base
    strip.fill(WHITE);
    
    // Blue aura (3 LEDs wide)
    for (int i = 0; i < 3; i++) {
      strip.setPixelColor((auraPos + i) % NUMPIXELS, BLUE);
    }
    
    // Random sparkle
    if (millis() - lastSparkle > random(200, 500)) {
      int idx = random(NUMPIXELS);
      strip.setPixelColor(idx, strip.Color(0, 0, 255));
      strip.show();
      delay(50);
      lastSparkle = millis();
    } else {
      strip.show();
    }
    
    auraPos = (auraPos + 1) % NUMPIXELS;
    delay(40);
  }
}

void animateBackward(unsigned long duration) {
  unsigned long startTime = millis();
  int wavePos = 0;
  
  while (millis() - startTime < duration) {
    strip.fill(WHITE);
    
    // Dual waves from back (15-19 and 20-24) to front (0-4 and 35-39)
    for (int i = 0; i < 4; i++) {
      int leftIdx = (15 - wavePos + i) % NUMPIXELS;
      int rightIdx = (20 - wavePos + i) % NUMPIXELS;
      if (leftIdx < 0) leftIdx += NUMPIXELS;
      if (rightIdx < 0) rightIdx += NUMPIXELS;
      strip.setPixelColor(leftIdx, BLUE);
      strip.setPixelColor(rightIdx, BLUE);
    }
    
    strip.show();
    wavePos = (wavePos + 1) % NUMPIXELS;
    delay(30);
  }
}

void animateForward(unsigned long duration) {
  unsigned long startTime = millis();
  int wavePos = 0;
  
  while (millis() - startTime < duration) {
    strip.fill(WHITE);
    
    // Dual waves from front (0-4 and 35-39) to back (15-19 and 20-24)
    for (int i = 0; i < 4; i++) {
      int leftIdx = (wavePos + i) % NUMPIXELS;
      int rightIdx = (35 + wavePos + i) % NUMPIXELS;
      strip.setPixelColor(leftIdx, BLUE);
      strip.setPixelColor(rightIdx, BLUE);
    }
    
    strip.show();
    wavePos = (wavePos + 1) % NUMPIXELS;
    delay(30);
  }
}

void animateStop() {
  // 3 cycles of red fade in/out
  for (int i = 0; i < 3; i++) {
    // Fade in
    for (int b = 0; b <= BRIGHTNESS; b += 10) {
      strip.fill(strip.Color(b, 0, 0));
      strip.show();
      delay(20);
    }
    delay(200); // Hold
    // Fade out
    for (int b = BRIGHTNESS; b >= 0; b -= 10) {
      strip.fill(strip.Color(b, 0, 0));
      strip.show();
      delay(20);
    }
  }
}

void animateLidOpen() {
  strip.fill(OFF);
  // Yellow fill from sides (mid down: 5-14, 25-34) to front/back
  for (int i = 0; i <= 5; i++) {
    for (int j = 0; j < i; j++) {
      // Left side (5-9, 10-14)
      if (5 + j < 10) strip.setPixelColor(5 + j, YELLOW);
      if (14 - j >= 10) strip.setPixelColor(14 - j, YELLOW);
      // Right side (25-29, 30-34)
      if (25 + j < 30) strip.setPixelColor(25 + j, YELLOW);
      if (34 - j >= 30) strip.setPixelColor(34 - j, YELLOW);
      // Front/back
      if (j < 5) {
        strip.setPixelColor(j, YELLOW);           // Front left
        strip.setPixelColor(35 + j, YELLOW);      // Front right
        strip.setPixelColor(15 + j, YELLOW);      // Back left
        strip.setPixelColor(20 + j, YELLOW);      // Back right
      }
    }
    strip.show();
    delay(100); // 500ms total
  }
  delay(200);
}

void animateLidClose() {
  strip.fill(YELLOW);
  // Yellow fade from front/back to sides
  for (int i = 5; i >= 0; i--) {
    strip.fill(OFF);
    for (int j = 0; j < i; j++) {
      // Left side (5-9, 10-14)
      if (5 + j < 10) strip.setPixelColor(5 + j, YELLOW);
      if (14 - j >= 10) strip.setPixelColor(14 - j, YELLOW);
      // Right side (25-29, 30-34)
      if (25 + j < 30) strip.setPixelColor(25 + j, YELLOW);
      if (34 - j >= 30) strip.setPixelColor(34 - j, YELLOW);
      // Front/back
      if (j < 5) {
        strip.setPixelColor(j, YELLOW);           // Front left
        strip.setPixelColor(35 + j, YELLOW);      // Front right
        strip.setPixelColor(15 + j, YELLOW);      // Back left
        strip.setPixelColor(20 + j, YELLOW);      // Back right
      }
    }
    strip.show();
    delay(100); // 500ms total
  }
  delay(200);
}

void animateLidClosed() {
  strip.fill(WHITE);
  // Random 8 LEDs turn off briefly
  for (int i = 0; i < 8; i++) {
    int idx = random(NUMPIXELS);
    strip.setPixelColor(idx, OFF);
    strip.show();
    delay(50);
    strip.setPixelColor(idx, WHITE);
  }
  // Transition to idle
  animateIdle(2000);
}
