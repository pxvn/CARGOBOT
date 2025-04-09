#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <CytronMotorDriver.h>
#include <Preferences.h>

Preferences preferences;
#define CONFIG_NAMESPACE "roverConfig"

// WiFi Credentials
const char* ssid = "0000";
const char* password = "12121212";

// Motor Pins (4-wheel configuration)
CytronMD frontRight(PWM_DIR, 5, 4);   // Front Right
CytronMD frontLeft(PWM_DIR, 18, 19);  // Front Left
CytronMD rearRight(PWM_DIR, 21, 22);  // Rear Right
CytronMD rearLeft(PWM_DIR, 23, 25);   // Rear Left

// Motor Parameters
struct MotorConfig {
  int maxSpeed = 200;
  int turnSpeed = 255;
  int acceleration = 30;
  bool invertMotors = false;
} config;

// Runtime Variables
int targetSpeed = 0;  // Removed volatile
int currentSpeed = 0;
int turnDirection = 0; 
unsigned long lastUpdate = 0;

AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <title>Rover Control</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { 
      display: flex;
      flex-direction: column;
      align-items: center;
      font-family: Arial;
      background: #111;
      color: white;
      -webkit-user-select: none;
      user-select: none;
    }
    .control-pad {
      display: grid;
      grid-template-areas:
        "fwd    fwd   fwd"
        "left   stop  right"
        "bwd    bwd   bwd";
      gap: 10px;
      margin: 20px;
    }
    button {
      padding: 25px;
      font-size: 24px;
      border: none;
      border-radius: 10px;
      background: #333;
      color: white;
      touch-action: manipulation;
    }
    #btnFwd { grid-area: fwd; }
    #btnLeft { grid-area: left; }
    #btnRight { grid-area: right; }
    #btnBwd { grid-area: bwd; }
    #btnStop { 
      grid-area: stop;
      background: #d32f2f; 
    }
  </style>
</head>
<body>
  <h1>Rover Control</h1>
  
  <div class="control-pad">
    <button id="btnFwd" ontouchstart="move('fwd')" ontouchend="release()">FWD</button>
    <button id="btnLeft" ontouchstart="turn('left')" ontouchend="release()">LEFT</button>
    <button id="btnStop" ontouchstart="estop()">STOP</button>
    <button id="btnRight" ontouchstart="turn('right')" ontouchend="release()">RIGHT</button>
    <button id="btnBwd" ontouchstart="move('bwd')" ontouchend="release()">BWD</button>
  </div>

  <script>
    let activeCommand = null;
    
    function move(direction) {
      activeCommand = direction;
      fetch(`/control?cmd=${direction}`);
    }

    function turn(direction) {
      activeCommand = direction;
      fetch(`/control?cmd=${direction}`);
    }

    function release() {
      if(activeCommand) {
        fetch('/control?cmd=stop');
        activeCommand = null;
      }
    }

    function estop() {
      fetch('/control?cmd=estop');
      activeCommand = null;
    }
  </script>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);
  loadConfig();
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  server.on("/control", HTTP_GET, [](AsyncWebServerRequest *request){
    if(request->hasParam("cmd")) {
      String command = request->getParam("cmd")->value();
      handleCommand(command);
    }
    request->send(200, "text/plain", "OK");
  });

  server.begin();
}

void loop() {
  unsigned long now = millis();
  if(now - lastUpdate >= 20) {
    updateMotors();
    lastUpdate = now;
  }
}

void updateMotors() {
  // Acceleration control with proper min/max handling
  if(currentSpeed < targetSpeed) {
    currentSpeed += config.acceleration;
    if(currentSpeed > targetSpeed) currentSpeed = targetSpeed;
  } 
  else if(currentSpeed > targetSpeed) {
    currentSpeed -= config.acceleration;
    if(currentSpeed < targetSpeed) currentSpeed = targetSpeed;
  }

  // Motor control logic
  int baseSpeed = config.invertMotors ? -currentSpeed : currentSpeed;
  int leftSpeed = baseSpeed;
  int rightSpeed = baseSpeed;

  if(turnDirection != 0) {
    int turnMultiplier = (currentSpeed != 0) ? 1 : 2;
    leftSpeed = config.turnSpeed * turnDirection * turnMultiplier;
    rightSpeed = -config.turnSpeed * turnDirection * turnMultiplier;
  }

  frontRight.setSpeed(rightSpeed);
  rearRight.setSpeed(rightSpeed);
  frontLeft.setSpeed(leftSpeed);
  rearLeft.setSpeed(leftSpeed);
}

void handleCommand(String command) {
  if(command == "fwd") {
    targetSpeed = config.maxSpeed;
    turnDirection = 0;
  }
  else if(command == "bwd") {
    targetSpeed = -config.maxSpeed;
    turnDirection = 0;
  }
  else if(command == "left") {
    turnDirection = -1;
  }
  else if(command == "right") {
    turnDirection = 1;
  }
  else if(command == "stop") {
    targetSpeed = 0;
    turnDirection = 0;
  }
  else if(command == "estop") {
    targetSpeed = 0;
    currentSpeed = 0;
    turnDirection = 0;
    frontRight.setSpeed(0);
    rearRight.setSpeed(0);
    frontLeft.setSpeed(0);
    rearLeft.setSpeed(0);
  }
}

void loadConfig() {
  preferences.begin(CONFIG_NAMESPACE, true);
  config.maxSpeed = preferences.getInt("maxSpeed", 200);
  config.turnSpeed = preferences.getInt("turnSpeed", 255);
  config.invertMotors = preferences.getBool("invert", false);
  preferences.end();
}

void saveConfig() {
  preferences.begin(CONFIG_NAMESPACE, false);
  preferences.putInt("maxSpeed", config.maxSpeed);
  preferences.putInt("turnSpeed", config.turnSpeed);
  preferences.putBool("invert", config.invertMotors);
  preferences.end();
}
