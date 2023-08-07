# Configure WiFi Credentials

## Requirements

- ESP8266 NodeMCU
- Postman

## Setup

1. Set the device in Pairing mode. To do this, press the reset button on for 5 seconds.
2. Connect to the WiFi network `ISM-XXXXXXXX` with password `Garden123`.
3. Make POST request to `http://192.168.4.1/wifi-credentials/` with the following body:

```json
{
  "ssid": "SSID",
  "password": "PASSWORD"
}
```

4. The ESP8266 NodeMCU will restart and connect to the WiFi network with the credentials provided.
