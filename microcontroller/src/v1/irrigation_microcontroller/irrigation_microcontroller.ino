#include "custom_wifi_manager.h"
#include "reset_button.h"

#define ledPin LED_BUILTIN
#define resetButtonPin D3

WiFiManager wifiManager;
ResetButton resetButton;


void connectionModeCallback() {
  Serial.println("Connection mode");
  digitalWrite(ledPin, HIGH);
}

void pairingModeCallback() {
  Serial.println("Pairing Mode");
  digitalWrite(ledPin, LOW);
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
  wifiManager.setConnectionModeCallback(connectionModeCallback);
  wifiManager.setPairingModeCallback(pairingModeCallback);

  resetButton.begin(resetButtonPin, 3000);
  resetButton.setResetCallback(resetCallback);
}

void loop() {
  wifiManager.loop();
  resetButton.checkStatus(); 
}
