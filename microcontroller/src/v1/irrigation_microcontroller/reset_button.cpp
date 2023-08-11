#include "reset_button.h"

extern ResetButton::ResetButton()
  : lastPressTime(0), resetTriggered(false), resetCallback(nullptr) {}

void ResetButton::begin(uint8_t buttonPin, unsigned long resetDuration = 5000) {
  this->buttonPin = buttonPin;
  this->resetDuration = resetDuration;
  pinMode(buttonPin, INPUT_PULLUP);
}

void ResetButton::setResetCallback(ResetCallback callback) {
  resetCallback = callback;
}


void ResetButton::checkStatus() {
  if (digitalRead(buttonPin) == LOW) {
    if (!resetTriggered) {
      lastPressTime = millis();
      resetTriggered = true;
    } else if (millis() - lastPressTime >= resetDuration) {
      if (resetCallback) {
        resetCallback();
      }
      resetTriggered = false;
    }
  } else {
    resetTriggered = false;
  }
}
