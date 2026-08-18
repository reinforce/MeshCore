#ifdef PIN_VIBRATION
#include "GenericVibration.h"

void GenericVibration::begin() {
  pinMode(PIN_VIBRATION, OUTPUT);
  digitalWrite(PIN_VIBRATION, LOW);
  count = 0;
  previousMillis = 0;
  pinState = LOW;
}

void GenericVibration::trigger() {
  if (_is_quiet) return;
  count = 3;
  previousMillis = millis();
  pinState = HIGH;
  digitalWrite(PIN_VIBRATION, pinState);
}

void GenericVibration::loop() {
  if (isVibrating()) {
    currentMillis = millis();
    if (currentMillis - previousMillis >= VIBRATION_INTERVAL) {
      previousMillis = currentMillis;
      pinState = !pinState;
      digitalWrite(PIN_VIBRATION, pinState);
      if (pinState == LOW) {
        count--;
        if (count == 0) {
          stop();
        }
      }
    }
  }
}

bool GenericVibration::isVibrating() {
  return count > 0;
}

void GenericVibration::stop() {
  pinState = LOW;
  digitalWrite(PIN_VIBRATION, pinState);
}

void GenericVibration::quiet(bool state) {
    _is_quiet = state;
}

bool GenericVibration::isQuiet() {
    return _is_quiet;
}

#endif // ifdef PIN_VIBRATION
