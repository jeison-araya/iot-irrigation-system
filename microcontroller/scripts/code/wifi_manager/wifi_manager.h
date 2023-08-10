#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <ESP8266WebServer.h>

enum Modes {
  PAIRING_MODE,
  WORKING_MODE,
  PENDING_RESTART
};

extern Modes currentMode;
extern const int EEPROM_SIZE;
extern ESP8266WebServer server;

void saveCredentialsToEEPROM(const String& ssid, const String& password);
void readCredentialsFromEEPROM(String& ssid, String& password);
void handleWifiCredentials();
void enableAPMode();
void enableClientMode(const String& ssid, const String& password);
void setupWiFi();

#endif
