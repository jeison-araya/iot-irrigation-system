#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

enum Modes {
  PAIRING_MODE,
  WORKING_MODE,
  PENDING_RESTART
};

const int resetButtonPin = D3;
const unsigned long resetDuration = 5000;
unsigned long buttonPressStartTime = 0;

const int EEPROM_SIZE = 96;
const int SSID_EEPROM_ADDR = 0;
const int PASS_EEPROM_ADDR = 32;

const char* AP_SSID = "ISM-XXXXXXXX";
const char* AP_PASS = "Garden123";

ESP8266WebServer server(80);
Modes currentMode;

void saveCredentialsToEEPROM(const String& ssid, const String& password);
void readCredentialsFromEEPROM(String& ssid, String& password);
void handleWifiCredentials();
void enableAPMode();
void enableClientMode(const String& ssid, const String& password);
void enableSerial();
void setupPins();
void setupWiFi();
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

void enableSerial() {
  Serial.begin(115200);
  while (!Serial) {}
}

void setupPins() {
  pinMode(resetButtonPin, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
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