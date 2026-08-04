#pragma once

#include "DisplayDriver.h"
#include <U8g2lib.h>
#include <Wire.h>

#ifndef DISPLAY_ADDRESS
  #define DISPLAY_ADDRESS   0x3C
#endif

class SSD1306U8g2Display : public DisplayDriver {
  U8G2_SSD1306_128X64_NONAME_F_HW_I2C _u8g2;
  bool _isOn;
  uint8_t _drawColor;

  // Font metrics for current font (cached on setTextSize)
  uint8_t _fontAscent;
  uint8_t _fontHeight;

  void applyFont(int sz);
public:
  SSD1306U8g2Display() : DisplayDriver(128, 64),
      _u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE),
      _isOn(false), _drawColor(1), _fontAscent(5), _fontHeight(6) {}

  bool begin();
  bool isOn() override;
  void turnOn() override;
  void turnOff() override;
  void clear() override;
  void startFrame(Color bkg = DARK) override;
  void setTextSize(int sz) override;
  void setColor(Color c) override;
  void setCursor(int x, int y) override;
  void print(const char* str) override;
  void fillRect(int x, int y, int w, int h) override;
  void drawRect(int x, int y, int w, int h) override;
  void drawXbm(int x, int y, const uint8_t* bits, int w, int h) override;
  uint16_t getTextWidth(const char* str) override;
  void endFrame() override;
private:
  int _cursorX = 0;
  int _cursorY = 0;
};
