// Sketch to clear flash memory:
#include <EEPROM.h>


void setup() {
  Serial.begin(115200);
  delay(100);
  while (!Serial) {}

  Serial.println("Clearing flash memory...");

  EEPROM.begin(96);

  for (int i = 0; i < 96; i++) {
    EEPROM.write(i, 0);
  }

  EEPROM.commit();

  Serial.println("Flash memory cleared.");
}

void loop() {
}