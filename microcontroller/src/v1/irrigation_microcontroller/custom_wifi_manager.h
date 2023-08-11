#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <ESP8266WebServer.h>

enum WiFiModes {
  CONNECTION_MODE,
  PAIRING_MODE,
  PENDING_RESTART
};


class WiFiManager {
public:
  WiFiManager();

  void begin();
  void forgetCredentials();
  void handleClient();
  void retryConnection();
  WiFiModes getCurrentMode();

private:
  bool savedCredentials();
  void handleWiFiCredentials();
  void enableConnectionMode();
  void enablePairingMode();
  void enablePendingRestart();
  bool connected();
  void setSSID(String &ssid);
  void setPassword(String &password);
  void setCurrentMode(WiFiModes mode);
  String getSSID();
  String getPassword();

  WiFiModes currentMode;
  ESP8266WebServer server;
  const int SSID_EEPROM_ADDR = 0;
  const int PASS_EEPROM_ADDR = 32;
  const int END_EEPROM_ADDR = 64;

  const char *AP_SSID = "ISM-XXXXXXXX";
  const char *AP_PASS = "Garden123";

  const int RECONNECTION_ATTEMPTS = 5;
};

#endif
