# Configure Arduino IDE for ESP8266 NodeMCU

## Requirements

- ESP8266 NodeMCU
- Arduino IDE

## Setup

1. Install Arduino IDE [here](https://www.arduino.cc/en/software).
2. Install ESP8266 NodeMCU board:

   1. Open Arduino IDE.
   2. Go to `File > Preferences`.
   3. In `Additional Boards Manager URLs` add the following URL: `http://arduino.esp8266.com/stable/package_esp8266com_index.json`.
   4. Go to `Tools > Board > Boards Manager`.
   5. Search for `esp8266` and install the board.

3. Connect the ESP8266 NodeMCU to your computer.
4. Go to `Tools > Port` and select the port where the ESP8266 NodeMCU is connected.
5. Go to `Tools > Board` and select `NodeMCU 1.0 (ESP-12E Module)`.
6. Go to `Tools > Upload Speed` and select `115200`.
7. Go to `Tools > CPU Frequency` and select `80 MHz`.
8. Go to `Tools > Flash Size` and select `4M (1M SPIFFS)`.

## Test

1. Open the [ide-config-test.ino](/microcontroller/scripts/ide-config-test.ino) file in Arduino IDE.
2. Go to `Tools > Serial Monitor`.
3. Upload the code to the ESP8266 NodeMCU.
4. Go to `Tools > Serial Monitor` and check that the following message is displayed: `Configuration test passed!`.
5. Connect LED to pin `D1` and check if is blinking.
