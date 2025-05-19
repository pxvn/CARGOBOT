#include <ESP32Servo.h>

Servo myServo;

const int servoPin = 4; // GPIO pin for PWM signal

void setup() {
  Serial.begin(115200);       // Start Serial Monitor
  myServo.attach(servoPin);   // Attach servo to pin
  Serial.println("Enter angle (0 to 180):");
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');  // Read input line
    int angle = input.toInt();                    // Convert to integer

    // Validate angle range
    if (angle >= 0 && angle <= 180) {
      myServo.write(angle);
      Serial.print("Servo moved to ");
      Serial.print(angle);
      Serial.println(" degrees.");
    } else {
      Serial.println("Invalid angle! Enter value between 0 and 180.");
    }
  }
}
