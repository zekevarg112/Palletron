#include <Arduino.h>

// changes made using GPT

// Define the pins
const int pin1 = 8;           // Set pin 1
const int pin2 = 9;           // Set pin 2
const int extendedPin = 2;    // Pin to read the actuator's extended signal (input)
const int retractedPin = 3;   // Pin to read the actuator's retracted signal (input)

// Define states
enum ActuatorState { IDLE, EXTENDING, RETRACTING } state;

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
  Serial.println("Actuator control ready.");
}

void stopActuator() {
  digitalWrite(pin1, LOW);
  digitalWrite(pin2, LOW);
  state = IDLE;
  Serial.println("Actuator stopped.");
}

void extendActuator() {
  if (digitalRead(extendedPin) == LOW) {
    digitalWrite(pin1, HIGH);
    state = EXTENDING;
  } else {
    // Already fully extended
    stopActuator();
    Serial.println("Actuator already fully extended.");
  }
}

void retractActuator() {
  if (digitalRead(retractedPin) == LOW) {
    digitalWrite(pin2, HIGH);
    state = RETRACTING;
  } else {
    // Already fully retracted
    stopActuator();
    Serial.println("Actuator already fully retracted.");
  }
}

void loop() {
  // Handle commands from Serial
  if (Serial.available() > 0) {
    int command = Serial.parseInt();

    // If a command is received while in motion, stop the actuator
    if (state == EXTENDING || state == RETRACTING) {
      stopActuator();
      Serial.println("New command received during motion. Stopping actuator.");
    }

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
    } else {
      Serial.println("Invalid command received.");
    }
  }

  // Handle actuator motion based on state
  if (state == EXTENDING) {
    if (digitalRead(extendedPin) == HIGH) {
      // Reached fully extended position
      stopActuator();
      Serial.println("Motion complete: Actuator fully extended.");
    }
  } else if (state == RETRACTING) {
    if (digitalRead(retractedPin) == HIGH) {
      // Reached fully retracted position
      stopActuator();
      Serial.println("Motion complete: Actuator fully retracted.");
    }
  }
}
