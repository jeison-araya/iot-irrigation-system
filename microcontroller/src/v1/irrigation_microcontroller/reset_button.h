#ifndef RESET_BUTTON_H
#define RESET_BUTTON_H

#include <Arduino.h>

class ResetButton {
public:
  typedef void (*ResetCallback)();

  ResetButton();

  void begin(uint8_t buttonPin, unsigned long resetDuration);
  void setResetCallback(ResetCallback callback);
  void checkStatus();

private:
  int buttonPin;
  unsigned long resetDuration;
  unsigned long lastPressTime;
  bool resetTriggered;
  ResetCallback resetCallback;
};

#endif
