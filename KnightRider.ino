#include <Arduino.h>
extern "C" {
#include "KnightRider.h"
}

#define BUTTON_PIN 2  

void setup() {
  Serial.begin(115200);
  while (!Serial);

  KnightRider_init();

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonISR, FALLING);

  Serial.println("Knight Rider ready. Press D2: 1st=RUNNING, 2nd=STOPPED, 3rd=RESET");
}

void loop() {
  if (buttonPressed) {
    Serial.print("Button pressed. Current state: ");
    switch (currentMode) {
      case STOPPED: Serial.println("STOPPED"); break;
      case RUNNING: Serial.println("RUNNING"); break;
      case RESET:   Serial.println("RESET"); break;
    }
    buttonPressed = false;
  }
}
