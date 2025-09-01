#include "Arduino.h"
extern "C" {
  #include "TCC_Timer.h"
}

const uint8_t ledPin = 17; // PA17

void maxPeriodReached() {
  Serial.println("Max period reached. LED stopped.");
}

void setup() {
  Serial.begin(115200);
  while (!Serial);

  TCC_Timer_setLED(0, ledPin);
  TCC_Timer_setCallback(maxPeriodReached);
  TCC_Timer_init();
  TCC_Timer_start();
}

void loop() {

}
