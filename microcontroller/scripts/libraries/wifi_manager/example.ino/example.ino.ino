#include <Arduino.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#define ledPin LED_BUILTIN

enum WiFiModes {
  CONNECTION_MODE,
  PAIRING_MODE,
  PENDING_RESTART
};

class CustomWiFiManager {
public:
  CustomWiFiManager();

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


CustomWiFiManager::CustomWiFiManager()
  : server(80) {}

void CustomWiFiManager::begin() {
  EEPROM.begin(END_EEPROM_ADDR);
  if (savedCredentials()) {
    enableConnectionMode();
  } else {
    enablePairingMode();
  }
}

void CustomWiFiManager::forgetCredentials() {
  for (int i = 0; i < END_EEPROM_ADDR; i++) {
    EEPROM.write(i, 0);
  }

  EEPROM.commit();

  enablePendingRestart();
}

void CustomWiFiManager::retryConnection() {
  if (savedCredentials()) {
    enableConnectionMode();
  }
}

void CustomWiFiManager::handleClient() {
  server.handleClient();
}

bool CustomWiFiManager::savedCredentials() {
  String ssid = getSSID();
  String password = getPassword();

  return ssid.length() > 0 && password.length() > 0;
}

void CustomWiFiManager::enableConnectionMode() {
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

void CustomWiFiManager::handleWiFiCredentials() {
  if (server.hasArg("ssid") && server.hasArg("password")) {
    String ssid = server.arg("ssid");
    String password = server.arg("password");

    setSSID(ssid);
    setPassword(password);

    server.send(200, "text/plain", "Credentials saved. Restarting...");

    enablePendingRestart();
  }
}

void CustomWiFiManager::enablePairingMode() {
  setCurrentMode(PAIRING_MODE);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASS);

  server.on("/wifi-credentials/", HTTP_POST, std::bind(&CustomWiFiManager::handleWiFiCredentials, this));
  server.begin();
}

void CustomWiFiManager::enablePendingRestart() {
  setCurrentMode(PENDING_RESTART);
}

void CustomWiFiManager::setCurrentMode(WiFiModes mode) {
  this->currentMode = mode;
}

void CustomWiFiManager::setSSID(String &ssid) {
  int ssidLength = ssid.length();

  for (int i = 0; i < ssidLength; ++i) {
    EEPROM.write(SSID_EEPROM_ADDR + i, ssid[i]);
  }
  EEPROM.write(SSID_EEPROM_ADDR + ssidLength, '\0');

  EEPROM.commit();
}

void CustomWiFiManager::setPassword(String &password) {
  int passLength = password.length();

  for (int i = 0; i < passLength; ++i) {
    EEPROM.write(PASS_EEPROM_ADDR + i, password[i]);
  }
  EEPROM.write(PASS_EEPROM_ADDR + passLength, '\0');

  EEPROM.commit();
}

bool CustomWiFiManager::connected() {
  return WiFi.status() == WL_CONNECTED;
}

String CustomWiFiManager::getSSID() {
  char value[PASS_EEPROM_ADDR];

  for (int i = 0; i < PASS_EEPROM_ADDR; ++i) {
    value[i] = EEPROM.read(SSID_EEPROM_ADDR + i);
    if (value[i] == '\0') {
      break;
    }
  }
  return String(value);
}

String CustomWiFiManager::getPassword() {
  char value[END_EEPROM_ADDR];

  for (int i = 0; i < END_EEPROM_ADDR; ++i) {
    value[i] = EEPROM.read(PASS_EEPROM_ADDR + i);
    if (value[i] == '\0') {
      break;
    }
  }
  return String(value);
}

WiFiModes CustomWiFiManager::getCurrentMode() {
  return this->currentMode;
}

CustomWiFiManager wifiManager;

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);
  wifiManager.begin();
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
      delay(1000);
      ESP.restart();
      break;
  }
  delay(500);

  Serial.print("Mode: ");
  Serial.println(mode);
}
