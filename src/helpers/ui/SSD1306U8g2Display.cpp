#include "SSD1306U8g2Display.h"

void SSD1306U8g2Display::applyFont(int sz) {
  if (sz >= 2) {
    _u8g2.setFont(u8g2_font_6x10_mr); // slightly larger font for better readability. TODO: more font sizes?
  } else {
    _u8g2.setFont(u8g2_font_5x7_mr);
  }
  _fontAscent = _u8g2.getAscent();
  _fontHeight = _u8g2.getAscent() - _u8g2.getDescent();
}

bool SSD1306U8g2Display::begin() {
  // Wire must already be initialised by board.begin() before this is called
  #ifdef DISPLAY_ROTATION
    switch(DISPLAY_ROTATION) {
    case 0:
      _u8g2.setDisplayRotation(U8G2_R0);
      break;
    case 1:
      _u8g2.setDisplayRotation(U8G2_R1);
      break;
    case 2:
      _u8g2.setDisplayRotation(U8G2_R2);
      break;
    case 3:
      _u8g2.setDisplayRotation(U8G2_R3);
      break;
    default:
      break;
  #endif

  bool ok = _u8g2.begin();
  if (ok) {
    _u8g2.enableUTF8Print(); // enable UTF8 support for the Arduino print() function
    _u8g2.setI2CAddress(DISPLAY_ADDRESS * 2);  // U8g2 uses 8-bit address
    _u8g2.setFontPosTop();     // y coordinate = top of text, not baseline
    _u8g2.setFontMode(1);      // transparent background
    applyFont(1);              // default to compact font
    _isOn = true;
  }
  return ok;
}

bool SSD1306U8g2Display::isOn() {
  return _isOn;
}

void SSD1306U8g2Display::turnOn() {
  _u8g2.setPowerSave(0);
  _isOn = true;
}

void SSD1306U8g2Display::turnOff() {
  _u8g2.setPowerSave(1);
  _isOn = false;
}

void SSD1306U8g2Display::clear() {
  _u8g2.clearBuffer();
  _u8g2.sendBuffer();
}

void SSD1306U8g2Display::startFrame(Color bkg) {
  _u8g2.clearBuffer();
  _drawColor = 1;
  _u8g2.setDrawColor(_drawColor);
}

void SSD1306U8g2Display::setTextSize(int sz) {
  applyFont(sz);
}

void SSD1306U8g2Display::setColor(Color c) {
  _drawColor = (c != DARK) ? 1 : 0;
  _u8g2.setDrawColor(_drawColor);
}

void SSD1306U8g2Display::setCursor(int x, int y) {
  _u8g2.setDrawColor(_drawColor);
  _u8g2.setCursor(x, y);
}

void SSD1306U8g2Display::print(const char* str) {
  _u8g2.setDrawColor(_drawColor);
  _u8g2.print(str);
}

void SSD1306U8g2Display::fillRect(int x, int y, int w, int h) {
  _u8g2.setDrawColor(_drawColor);
  _u8g2.drawBox(x, y, w, h);
}

void SSD1306U8g2Display::drawRect(int x, int y, int w, int h) {
  _u8g2.setDrawColor(_drawColor);
  _u8g2.drawFrame(x, y, w, h);
}

void SSD1306U8g2Display::drawXbm(int x, int y, const uint8_t* bits, int w, int h) {
  _u8g2.setDrawColor(1);
  _u8g2.drawXBM(x, y, w, h, bits);
}

uint16_t SSD1306U8g2Display::getTextWidth(const char* str) {
  return _u8g2.getStrWidth(str);
}

void SSD1306U8g2Display::endFrame() {
  _u8g2.sendBuffer();
}
