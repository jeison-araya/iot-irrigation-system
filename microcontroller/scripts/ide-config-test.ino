/*
  Blink LED and Serial Output - ESP8266

  This script blinks an LED connected to GPIO5 (D1 pin) on the ESP8266 board, such as the NodeMCU.
  It also sends a "Configuration test passed!" message to the serial monitor when the board is powered up.

  Hardware Setup:
  - Connect an LED with a current-limiting resistor (e.g., 220-330 ohms) to GPIO5 (D1 pin) on the NodeMCU.

  Behavior:
  - The LED will blink on and off every second, creating a blinking effect.
  - The message "Configuration test passed!" will be displayed on the serial monitor at startup.

  Notes:
  - Make sure to select the appropriate board and port in the Arduino IDE before uploading the sketch.
  - To view the serial output, open the Arduino IDE's serial monitor at 115200 baud.
*/
const int ledPin = 5;

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
  while (!Serial) {}

  Serial.println("Configuration test passed!");
}

void loop() {
  digitalWrite(ledPin, HIGH);
  delay(1000);
  digitalWrite(ledPin, LOW);
  delay(1000);
}
