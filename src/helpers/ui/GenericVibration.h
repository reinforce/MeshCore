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

#ifndef VIBRATION_TIMEOUT
#define VIBRATION_TIMEOUT 5000 // 5 seconds default
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
  unsigned long duration;
  bool _is_quiet = false;
};

#endif // ifdef PIN_VIBRATION
