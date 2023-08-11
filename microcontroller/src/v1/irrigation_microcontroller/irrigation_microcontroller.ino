#include "custom_wifi_manager.h"

#define ledPin LED_BUILTIN
#define resetButtonPin D3


class ResetButton {
public:
  typedef void (*ResetCallback)();

  ResetButton();

  void begin(uint8_t buttonPin, unsigned long resetDuration);
  void setResetCallback(ResetCallback callback);
  void checkStatus();

private:
  int buttonPin;
  unsigned long resetDuration;
  unsigned long lastPressTime;
  bool resetTriggered;
  ResetCallback resetCallback;
};

ResetButton::ResetButton()
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




WiFiManager wifiManager;
ResetButton resetButton;


void resetCallback() {
  Serial.println("RESET CALLBACK TRIGGERED...");
  delay(3000);
  wifiManager.forgetCredentials();
  ESP.restart();
}
void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);
  wifiManager.begin();
  resetButton.begin(resetButtonPin, 3000);
  resetButton.setResetCallback(resetCallback);
}

void loop() {

  WiFiModes mode = wifiManager.getCurrentMode();

  switch (mode) {
    case CONNECTION_MODE:
      Serial.println("Connection mode");
      digitalWrite(ledPin, HIGH);
      break;
    case PAIRING_MODE:
      Serial.println("Pairing Mode");
      wifiManager.handleClient();
      digitalWrite(ledPin, LOW);
      break;
    case PENDING_RESTART:
      Serial.println("Restarting...");
      ESP.restart();
      break;
  }
  delay(500);
  resetButton.checkStatus();
  Serial.print("Mode: ");
  Serial.println(mode);
}
