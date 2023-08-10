#include "wifi_manager.h"
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

Modes currentMode = PAIRING_MODE;

const int EEPROM_SIZE = 96;
const int SSID_EEPROM_ADDR = 0;
const int PASS_EEPROM_ADDR = 32;

const char* AP_SSID = "ISM-XXXXXXXX";
const char* AP_PASS = "Garden123";

ESP8266WebServer server(80);


void saveCredentialsToEEPROM(const String& ssid, const String& password) {
  int ssidLength = ssid.length();
  int passLength = password.length();

  for (int i = 0; i < ssidLength; ++i) {
    EEPROM.write(SSID_EEPROM_ADDR + i, ssid[i]);
  }
  EEPROM.write(SSID_EEPROM_ADDR + ssidLength, '\0');

  for (int i = 0; i < passLength; ++i) {
    EEPROM.write(PASS_EEPROM_ADDR + i, password[i]);
  }
  EEPROM.write(PASS_EEPROM_ADDR + passLength, '\0');

  EEPROM.commit();
}

void readCredentialsFromEEPROM(String& ssid, String& password) {
  int ssidLength = 0;
  int passLength = 0;

  char eepromData[64];
  for (int i = 0; i < 32; ++i) {
    eepromData[i] = EEPROM.read(SSID_EEPROM_ADDR + i);
    if (eepromData[i] == '\0') {
      ssidLength = i;
      break;
    }
  }
  ssid = String(eepromData);

  for (int i = 0; i < 64; ++i) {
    eepromData[i] = EEPROM.read(PASS_EEPROM_ADDR + i);
    if (eepromData[i] == '\0') {
      passLength = i;
      break;
    }
  }
  password = String(eepromData);
}


void handleWifiCredentials() {
  String ssid = server.arg("ssid");
  String password = server.arg("password");

  saveCredentialsToEEPROM(ssid, password);

  currentMode = PENDING_RESTART;

  server.send(200, "text/plain", "Wi-Fi credentials updated");
}

void enableAPMode() {
  Serial.println("Starting as Access Point (AP)...");
  currentMode = PAIRING_MODE;

  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASS);

  Serial.print("> IP address: ");
  Serial.println(WiFi.softAPIP());

  server.on("/wifi-credentials/", HTTP_POST, handleWifiCredentials);
  server.begin();
}

void enableClientMode(const String& ssid, const String& password) {
  currentMode = WORKING_MODE;
  Serial.println("Starting as Client...");

  Serial.print("Connecting to ");
  Serial.println(ssid.c_str());

  WiFi.begin(ssid.c_str(), password.c_str());
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1500);
    Serial.print(".");
    attempts += 1;

    if (attempts > 5) {
      break;
    }
  }
}

void setupWiFi() {
  String ssid, password;

  EEPROM.begin(EEPROM_SIZE);

  readCredentialsFromEEPROM(ssid, password);

  if (ssid.length() == 0 || password.length() == 0) {
    enableAPMode();
  } else {
    enableClientMode(ssid, password);
  }
}
