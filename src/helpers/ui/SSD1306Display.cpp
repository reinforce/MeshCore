#include "SSD1306Display.h"

bool SSD1306Display::i2c_probe(TwoWire& wire, uint8_t addr) {
  wire.beginTransmission(addr);
  uint8_t error = wire.endTransmission();
  return (error == 0);
}

// Color scheme
ColorVal UIColor::window_bkg = SSD1306_BLACK;
ColorVal UIColor::title_bkg = SSD1306_BLACK;
ColorVal UIColor::title_txt = SSD1306_WHITE;
ColorVal UIColor::primary_txt = SSD1306_WHITE;
ColorVal UIColor::secondary_txt = SSD1306_WHITE;
ColorVal UIColor::warning_txt = SSD1306_WHITE;
ColorVal UIColor::popup_bkg = SSD1306_BLACK;
ColorVal UIColor::popup_txt = SSD1306_WHITE;
ColorVal UIColor::corp_blue = SSD1306_WHITE;

bool SSD1306Display::begin() {
  if (!_isOn) {
    if (_peripher_power) _peripher_power->claim();
    _isOn = true;
  }
  #ifdef DISPLAY_ROTATION
  display.setRotation(DISPLAY_ROTATION);
  #endif
#ifdef DISPLAY_UTF8_FONTS
  u8f.begin(display);
#endif
  return display.begin(SSD1306_SWITCHCAPVCC, DISPLAY_ADDRESS, true, false) && i2c_probe(Wire, DISPLAY_ADDRESS);
}

void SSD1306Display::turnOn() {
  if (!_isOn) {
    if (_peripher_power) _peripher_power->claim();
    _isOn = true;  // set before begin() to prevent double claim
    if (_peripher_power) begin();  // re-init display after power was cut
  }
  display.ssd1306_command(SSD1306_DISPLAYON);
}

void SSD1306Display::turnOff() {
  display.ssd1306_command(SSD1306_DISPLAYOFF);
  if (_isOn) {
    if (_peripher_power) {
#if PIN_OLED_RESET >= 0
      digitalWrite(PIN_OLED_RESET, LOW);
#endif
      _peripher_power->release();
    }
    _isOn = false;
  }
}

void SSD1306Display::clear() {
  display.clearDisplay();
  display.display();
}

void SSD1306Display::startFrame(ColorVal bkg) {
  display.clearDisplay();  // TODO: apply 'bkg'
  _color = SSD1306_WHITE;
  display.setTextColor(_color);
  setTextSize(1);
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
}

void SSD1306Display::setTextSize(int sz) {
  display.setTextSize(sz);
#ifdef DISPLAY_UTF8_FONTS
  // match the u8g2 Cyrillic font to the scaled 5x7 GFX font: size 1 is a
  // 6px advance / 8px line, size 2+ is 12x16 -- 10x20 is the largest available
  if (sz <= 1) {
    u8f.setFont(u8g2_font_6x12_t_cyrillic);
  } else {
    u8f.setFont(u8g2_font_10x20_t_cyrillic);
  }
  u8f.setFontMode(1);   // must follow setFont(): setFont() resets to solid mode
#endif
}

void SSD1306Display::setColor(ColorVal c) {
  _color = c;
  display.setTextColor(_color);
}

void SSD1306Display::setCursor(int x, int y) {
  display.setCursor(x, y);
}

void SSD1306Display::print(const char* str) {
#ifdef DISPLAY_UTF8_FONTS
  if (hasNonASCII(str)) {
    printUTF8(str);
    return;
  }
#endif
  display.print(str);
}

#ifdef DISPLAY_UTF8_FONTS
void SSD1306Display::printUTF8(const char* str) {
  u8f.setForegroundColor(_color);
  // the GFX built-in font draws from a top-left cursor, u8g2 from the baseline
  int16_t ascent = u8f.getFontAscent();
  u8f.setCursor(display.getCursorX(), display.getCursorY() + ascent);
  int16_t line_height = ascent - u8f.getFontDescent() + 1;
  char glyph[5];
  for (const char* p = str; *p; ) {
    if (*p == '\n') {
      u8f.setCursor(0, u8f.getCursorY() + line_height);
      p++;
      continue;
    }
    int n = 1;
    while (n < 4 && (p[n] & 0xC0) == 0x80) n++;  // include UTF-8 continuation bytes
    memcpy(glyph, p, n);
    glyph[n] = 0;
    // wrap to next line at right edge, same as Adafruit GFX does for ASCII
    if (u8f.getCursorX() + u8f.getUTF8Width(glyph) > display.width()) {
      u8f.setCursor(0, u8f.getCursorY() + line_height);
    }
    u8f.print(glyph);
    p += n;
  }
  display.setCursor(u8f.getCursorX(), u8f.getCursorY() - ascent);
}
#endif

void SSD1306Display::fillRect(int x, int y, int w, int h) {
  display.fillRect(x, y, w, h, _color);
}

void SSD1306Display::drawRect(int x, int y, int w, int h) {
  display.drawRect(x, y, w, h, _color);
}

void SSD1306Display::drawXbm(int x, int y, const uint8_t* bits, int w, int h) {
  display.drawBitmap(x, y, bits, w, h, _color);
}

uint16_t SSD1306Display::getTextWidth(const char* str) {
#ifdef DISPLAY_UTF8_FONTS
  if (hasNonASCII(str)) {
    return u8f.getUTF8Width(str);
  }
#endif
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(str, 0, 0, &x1, &y1, &w, &h);
  return w;
}

void SSD1306Display::endFrame() {
  display.display();
}
