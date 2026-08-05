#ifdef PIN_VIBRATION
#include "GenericVibration.h"

void GenericVibration::begin() {
  pinMode(PIN_VIBRATION, OUTPUT);
  digitalWrite(PIN_VIBRATION, LOW);
  duration = 0;
  quiet(false);
}

void GenericVibration::trigger() {
  if (_is_quiet) return;
  duration = millis();
  digitalWrite(PIN_VIBRATION, HIGH);
}

void GenericVibration::loop() {
  if (isVibrating()) {
    if ((millis() / 500) % 2 == 0) {
      digitalWrite(PIN_VIBRATION, LOW);
    } else {
      digitalWrite(PIN_VIBRATION, HIGH);
    }

    if (millis() - duration > VIBRATION_TIMEOUT) {
      stop();
    }
  }
}

bool GenericVibration::isVibrating() {
  return duration > 0;
}

void GenericVibration::stop() {
  duration = 0;
  digitalWrite(PIN_VIBRATION, LOW);
}

void GenericVibration::quiet(bool vibra_state) {
    _is_quiet = vibra_state;
}

bool GenericVibration::isQuiet() {
    return _is_quiet;
}

#endif // ifdef PIN_VIBRATION
