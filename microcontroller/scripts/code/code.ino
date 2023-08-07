#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// MODES //
#define AP_MODE 0
#define CLIENT_MODE 1

bool pendingRestart = false;
int currentMode;
// RESET BUTTON SETTINGS //
const int ledPin = LED_BUILTIN;
const int resetButtonPin = D3;
unsigned long buttonPressStartTime = 0;
const unsigned long resetDuration = 5000;
// CLIENT WIFI SETTINGS //
const int SSID_EEPROM_ADDR = 0;
const int PASSWORD_EEPROM_ADDR = 32;
const int MAX_ATTEMPTS = 5;
// AP WIFI SETTINGS //
const char* AP_SSID = "ISM-XXXXXXXX";
const char* AP_PASS = "Garden123";
// HTTP SERVER //
ESP8266WebServer server(80);


void saveCredentialsToEEPROM(const String& ssid, const String& password) {
  int ssidLength = ssid.length();
  int passLength = password.length();

  for (int i = 0; i < ssidLength; ++i) {
    EEPROM.write(SSID_EEPROM_ADDR + i, ssid[i]);
  }
  EEPROM.write(SSID_EEPROM_ADDR + ssidLength, '\0');

  for (int i = 0; i < passLength; ++i) {
    EEPROM.write(PASSWORD_EEPROM_ADDR + i, password[i]);
  }
  EEPROM.write(PASSWORD_EEPROM_ADDR + passLength, '\0');

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
  Serial.print("SSID: ");
  Serial.print(ssidLength);
  Serial.print(ssid);


  for (int i = 0; i < 64; ++i) {
    eepromData[i] = EEPROM.read(PASSWORD_EEPROM_ADDR + i);
    if (eepromData[i] == '\0') {
      passLength = i;
      break;
    }
  }
  password = String(eepromData);
  Serial.print("APSS: ");
  Serial.print(passLength);
  Serial.print(password);
}


void handleWifiCredentials() {
  String ssid = server.arg("ssid");
  String password = server.arg("password");

  Serial.print("Received SSID: ");
  Serial.println(ssid);
  Serial.print("Received Password: ");
  Serial.println(password);

  saveCredentialsToEEPROM(ssid, password);
  
  pendingRestart = true;

  server.send(200, "text/plain", "Wi-Fi credentials updated");
}

void enableAPMode() {
  Serial.println("Starting as Access Point (AP)...");
  currentMode = AP_MODE;

  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASS);

  Serial.println("AP mode started.");
  Serial.print("> IP address: ");
  Serial.println(WiFi.softAPIP());

  server.on("/wifi-credentials/", HTTP_POST, handleWifiCredentials);
  server.begin();
}

void enableClientMode(const String& ssid, const String& password) {
  currentMode = CLIENT_MODE;
  Serial.println("Starting as Client...");

  Serial.print("Connecting to ");
  Serial.println(ssid.c_str());

  WiFi.begin(ssid.c_str(), password.c_str());
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1500);
    Serial.print(".");
    attempts += 1;

    if (attempts > MAX_ATTEMPTS) {
      break;
    }
  }

  Serial.println("\nConnected to Wi-Fi!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void enableSerial() {
  Serial.begin(115200);
  delay(100);
  while (!Serial) {}
}

void setupPins() {
  pinMode(resetButtonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);
}

void setupWiFi() {
  String ssid, password;

  EEPROM.begin(96);

  readCredentialsFromEEPROM(ssid, password);

  if (ssid.length() == 0 || password.length() == 0) {
    enableAPMode();
  } else {
    enableClientMode(ssid, password);
  }
}

void setup() {
  enableSerial();
  setupPins();
  setupWiFi();
}

void erraseFlashMemory() {
  Serial.println("Clearing flash memory...");

  for (int i = 0; i < 96; i++) {
    EEPROM.write(i, 0);
  }

  EEPROM.commit();

  Serial.println("Flash memory cleared.");
}

void checkResetButtonState() {
  if (digitalRead(resetButtonPin) == HIGH) {
    buttonPressStartTime = millis();
    digitalWrite(ledPin, HIGH);
  } else {
    unsigned long buttonPressDuration = millis() - buttonPressStartTime;
    digitalWrite(ledPin, LOW);
    if (buttonPressDuration >= resetDuration) {
      delay(500);
      erraseFlashMemory();
      pendingRestart = true;
    }
  }
}
void loop() {
  checkResetButtonState();

  if (currentMode == AP_MODE) {
    server.handleClient();
  }

  if (pendingRestart) {
    ESP.restart();
  }
}
