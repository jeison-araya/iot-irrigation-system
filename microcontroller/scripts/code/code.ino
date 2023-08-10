#include <Arduino.h>
#include <wifi_manager.h>
#include <EEPROM.h> 

const int resetButtonPin = D3;
const unsigned long resetDuration = 5000;
unsigned long buttonPressStartTime = 0;

void enableSerial();
void setupPins();
void resetFlash();
void checkResetButtonState();

void setup() {
  enableSerial();
  setupPins();
  setupWiFi();
}

void loop() {
  checkResetButtonState();

  switch (currentMode) {
    case PAIRING_MODE:
       server.handleClient();
      break;
    case PENDING_RESTART:
      ESP.restart();
      break;
  }
}

void enableSerial() {
  Serial.begin(115200);
  while (!Serial) {}
}

void setupPins() {
  pinMode(resetButtonPin, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
}

void resetFlash() {
  for (int i = 0; i < EEPROM_SIZE; i++) {
    EEPROM.write(i, 0);
  }

  EEPROM.commit();
}

void checkResetButtonState() {
  if (digitalRead(resetButtonPin) == HIGH) {
    buttonPressStartTime = millis();
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    unsigned long buttonPressDuration = millis() - buttonPressStartTime;
    if (buttonPressDuration >= resetDuration) {
      digitalWrite(LED_BUILTIN, LOW);
      currentMode = PENDING_RESTART;
      resetFlash();
      delay(500);
    }
   }
}