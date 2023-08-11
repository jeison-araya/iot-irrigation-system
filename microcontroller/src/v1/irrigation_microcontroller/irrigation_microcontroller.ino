#include "custom_wifi_manager.h"
#include "reset_button.h"

#define ledPin LED_BUILTIN
#define resetButtonPin D3

WiFiManager wifiManager;
ResetButton resetButton;

void checkWiFiMode() {
  WiFiModes mode = wifiManager.getCurrentMode();
  
  Serial.print("Mode: ");
  Serial.println(mode);

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
}

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
  checkWiFiMode();
  resetButton.checkStatus(); 
}
