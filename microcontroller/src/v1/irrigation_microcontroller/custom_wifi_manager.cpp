#include "custom_wifi_manager.h"
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>


extern WiFiManager::WiFiManager()
  : server(80) {}

void WiFiManager::begin() {
  EEPROM.begin(END_EEPROM_ADDR);
  if (savedCredentials()) {
    enableConnectionMode();
  } else {
    enablePairingMode();
  }
}

void WiFiManager::forgetCredentials() {
  for (int i = 0; i < END_EEPROM_ADDR; i++) {
    EEPROM.write(i, 0);
  }

  EEPROM.commit();

  enablePendingRestart();
}

void WiFiManager::loop() {
  switch (getCurrentMode()) {
    case CONNECTION_MODE:
      if (connectionModeCallback) {
        connectionModeCallback();
      }
      break;
    case PAIRING_MODE:
      if (pairingModeCallback) {
        pairingModeCallback();
      }
      server.handleClient();
      break;
    case PENDING_RESTART:
      ESP.restart();
      break;
  }
}


bool WiFiManager::savedCredentials() {
  String ssid = getSSID();
  String password = getPassword();

  return ssid.length() > 0 && password.length() > 0;
}

void WiFiManager::enableConnectionMode() {
  setCurrentMode(CONNECTION_MODE);
  String ssid = getSSID();
  String password = getPassword();

  WiFi.begin(ssid, password);

  for (int i = 0; i < RECONNECTION_ATTEMPTS; i++) {
    if (WiFi.status() != WL_CONNECTED) {
      break;
    }

    delay(500);
  }
}

void WiFiManager::handleWiFiCredentials() {
  if (server.hasArg("ssid") && server.hasArg("password")) {
    String ssid = server.arg("ssid");
    String password = server.arg("password");

    setSSID(ssid);
    setPassword(password);

    server.send(200, "text/plain", "Credentials saved. Restarting...");

    enablePendingRestart();
  }
}

void WiFiManager::enablePairingMode() {
  setCurrentMode(PAIRING_MODE);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASS);

  server.on("/wifi-credentials/", HTTP_POST, std::bind(&WiFiManager::handleWiFiCredentials, this));
  server.begin();
}

void WiFiManager::enablePendingRestart() {
  setCurrentMode(PENDING_RESTART);
}

void WiFiManager::setCurrentMode(WiFiModes mode) {
  this->currentMode = mode;
}

void WiFiManager::setSSID(String &ssid) {
  int ssidLength = ssid.length();

  for (int i = 0; i < ssidLength; ++i) {
    EEPROM.write(SSID_EEPROM_ADDR + i, ssid[i]);
  }
  EEPROM.write(SSID_EEPROM_ADDR + ssidLength, '\0');

  EEPROM.commit();
}

void WiFiManager::setPassword(String &password) {
  int passLength = password.length();

  for (int i = 0; i < passLength; ++i) {
    EEPROM.write(PASS_EEPROM_ADDR + i, password[i]);
  }
  EEPROM.write(PASS_EEPROM_ADDR + passLength, '\0');

  EEPROM.commit();
}

void WiFiManager::setConnectionModeCallback(ConnectionModeCallback callback) {
  connectionModeCallback = callback;
}

void WiFiManager::setPairingModeCallback(PairingModeCallback callback) {
  pairingModeCallback = callback;
}


bool WiFiManager::connected() {
  return WiFi.status() == WL_CONNECTED;
}

String WiFiManager::getSSID() {
  char value[PASS_EEPROM_ADDR];

  for (int i = 0; i < PASS_EEPROM_ADDR; ++i) {
    value[i] = EEPROM.read(SSID_EEPROM_ADDR + i);
    if (value[i] == '\0') {
      break;
    }
  }
  return String(value);
}

String WiFiManager::getPassword() {
  char value[END_EEPROM_ADDR];

  for (int i = 0; i < END_EEPROM_ADDR; ++i) {
    value[i] = EEPROM.read(PASS_EEPROM_ADDR + i);
    if (value[i] == '\0') {
      break;
    }
  }
  return String(value);
}

WiFiModes WiFiManager::getCurrentMode() {
  return this->currentMode;
}
