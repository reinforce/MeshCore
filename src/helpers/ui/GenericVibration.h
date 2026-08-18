#pragma once

#ifdef PIN_VIBRATION

#include <Arduino.h>

/*
 * Vibration motor control class
 *
 * Provides vibration feedback for events like new messages and new contacts
 * Features:
 * - 1-second vibration pulse
 * - 5-second nag timeout (cooldown between vibrations)
 * - Non-blocking operation
 */

#ifndef VIBRATION_INTERVAL
#define VIBRATION_INTERVAL 250 // 250 milliseconds default
#endif

class GenericVibration {
public:
  void begin();       // set up vibration pin
  void trigger();     // trigger vibration if cooldown has passed
  void loop();        // non-blocking timer handling
  bool isVibrating(); // returns true if currently vibrating
  void stop();        // stop vibration immediately
  void quiet(bool state);  // enables or disables the vibration
  bool isQuiet();     // get vibration state on/off

private:
  bool _is_quiet = false;
  unsigned long currentMillis;
  unsigned long previousMillis;
  int count = 0;
  bool pinState = LOW;
};

#endif // ifdef PIN_VIBRATION
