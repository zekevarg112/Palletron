#include <Arduino.h>
#include <FastLED.h>

// changes made using GPT

// Define the pins
const int pin1 = 8;           // Set pin 1
const int pin2 = 9;           // Set pin 2
const int extendedPin = 2;    // Pin to read the actuator's extended signal (input)
const int retractedPin = 3;   // Pin to read the actuator's retracted signal (input)

const int responseExtended = 1;
const int responeRetracted = 2;
const int responseStopped = 0;

// Define states
enum ActuatorState { IDLE, EXTENDING, RETRACTING } state;

// LED strip configuration
#define NUM_LEDS 60           // Adjust based on your LED strip
#define DATA_PIN 6            // Pin connected to LED strip
CRGB leds[NUM_LEDS];

// LED effect variables
enum LEDMode { OFF, RUNNING_LIGHTS, RAINBOW_CYCLE } ledMode;
unsigned long previousMillis = 0;  // Time tracking for non-blocking updates
int ledPosition = 0;               // For Running Lights
int rainbowStep = 0;               // For Rainbow Cycle
int waveDelay = 50;                // Default delay for Running Lights
int rainbowSpeed = 20;             // Default speed for Rainbow Cycle
int brightness = 255;

void setup() {
  Serial.begin(115200); // Initialize serial communication at a baud rate of 9600
  // Initialize pins as outputs
  pinMode(pin1, OUTPUT);
  pinMode(pin2, OUTPUT);

  // Initialize pins as inputs for actuator position signals
  pinMode(extendedPin, INPUT);
  pinMode(retractedPin, INPUT);

  // Set initial states to LOW
  digitalWrite(pin1, LOW);
  digitalWrite(pin2, LOW);

  state = IDLE; // Set initial state to IDLE

  // Initialize LED strip
  FastLED.addLeds<WS2813, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.clear();
  FastLED.show();

  ledMode = OFF; // Start with LEDs off
}

void stopActuator() {
  digitalWrite(pin1, LOW);
  digitalWrite(pin2, LOW);
  state = IDLE;
  Serial.println(responseStopped);
}

void extendActuator() {
  if (digitalRead(extendedPin) == LOW) {
    digitalWrite(pin1, HIGH);
    state = EXTENDING;
  } else {
    // Already fully extended
    stopActuator();
    Serial.println(responseExtended);
  }
}

void retractActuator() {
  if (digitalRead(retractedPin) == LOW) {
    digitalWrite(pin2, HIGH);
    state = RETRACTING;
  } else {
    // Already fully retracted
    stopActuator();
    Serial.println(responeRetracted);
  }
}


// Non-blocking Running Lights effect
void updateRunningLights(byte hue) {
  if (millis() - previousMillis >= waveDelay) {
    previousMillis = millis();
    ledPosition++;
    for (int i = 0; i < NUM_LEDS; i++) {
      float waveBrightness = (sin((i + ledPosition)/6.0) * 127 + 128) / 255.0; // Sine wave for brightness modulation
      leds[i] = CHSV(hue, 255, brightness*waveBrightness);            // HSV with dynamic brightness
    
    }
    FastLED.show();
  }
}

// Non-blocking Rainbow Cycle effect
void updateRainbowCycle() {
  if (millis() - previousMillis >= rainbowSpeed) {
    previousMillis = millis();
    for (int i = 0; i < NUM_LEDS; i++) {
      byte hue = (i * 255 / NUM_LEDS + rainbowStep) % 255; // Spread hues across the strip
      leds[i] = CHSV(hue, 255, brightness);               // Apply HSV with specified brightness
    }
    rainbowStep = (rainbowStep + 1) % (256 * 5);
    FastLED.show();
  }
}

void loop() {
  // Handle commands from Serial
  if (Serial.available() > 0) {
    int command = Serial.parseInt();

    // If a movement command is received while in motion, stop the actuator
    if ((state == EXTENDING || state == RETRACTING) && (command == 0 || command == 1 || command == 3)) {
      stopActuator();
      Serial.println("New command received during motion. Stopping actuator.");
      
    }
    else{
    // Process new command
    if (command == 1) {
      Serial.println("Command received: Extend");
      extendActuator();
    } else if (command == 2) {
      Serial.println("Command received: Retract");
      retractActuator();
    } else if (command == 0) {
      Serial.println("Command received: Stop");
      stopActuator();
    } else if (command == 10) {
      Serial.println("Command received: Turn off LEDs");
      ledMode = OFF;
      FastLED.clear();
      FastLED.show();
    } else if (command == 11) {
      Serial.println("Command received: Running Lights");
      ledMode = RUNNING_LIGHTS;
    } else if (command == 12) {
      Serial.println("Command received: Rainbow Cycle");
      ledMode = RAINBOW_CYCLE;
    } else {
      Serial.println("Invalid command received.");
    }}
    Serial.read();

  }

  // Handle actuator motion based on state
  if (state == EXTENDING) {
    if (digitalRead(extendedPin) == HIGH) {
      // Reached fully extended position
      stopActuator();
      Serial.println(responseExtended);
      Serial.println("Motion complete: Actuator fully extended.");
    }
  } else if (state == RETRACTING) {
    if (digitalRead(retractedPin) == HIGH) {
      // Reached fully retracted position
      stopActuator();
      Serial.println(responeRetracted);
      Serial.println("Motion complete: Actuator fully retracted.");
    }
  }
  
  // Handle LED effects
  if (ledMode == RUNNING_LIGHTS) {
    updateRunningLights(HUE_ORANGE);
  } else if (ledMode == RAINBOW_CYCLE) {
    updateRainbowCycle();
  }

}

