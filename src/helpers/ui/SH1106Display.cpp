#include "SH1106Display.h"
#include <Adafruit_GrayOLED.h>
#include "Adafruit_SH110X.h"

bool SH1106Display::i2c_probe(TwoWire &wire, uint8_t addr)
{
  wire.beginTransmission(addr);
  uint8_t error = wire.endTransmission();
  return (error == 0);
}

// Color scheme
ColorVal UIColor::window_bkg = SH110X_BLACK;
ColorVal UIColor::title_bkg = SH110X_BLACK;
ColorVal UIColor::title_txt = SH110X_WHITE;
ColorVal UIColor::primary_txt = SH110X_WHITE;
ColorVal UIColor::secondary_txt = SH110X_WHITE;
ColorVal UIColor::warning_txt = SH110X_WHITE;
ColorVal UIColor::popup_bkg = SH110X_BLACK;
ColorVal UIColor::popup_txt = SH110X_WHITE;
ColorVal UIColor::corp_blue = SH110X_WHITE;

bool SH1106Display::begin()
{
#ifdef DISPLAY_UTF8_FONTS
  u8f.begin(display);
#endif
  // Wire must already be initialised by board.begin() before this is called.
  // Boards with non-standard SH1106 addresses should define DISPLAY_ADDRESS
  // in their variant/platformio configuration. The SA0 strap selects 0x3C or
  // 0x3D and differs between revisions of the same board (e.g. T-Beam
  // Supreme), so fall back to the other address of the pair.
  uint8_t addr = 0;
  if (i2c_probe(Wire, DISPLAY_ADDRESS)) {
    addr = DISPLAY_ADDRESS;
  } else if (i2c_probe(Wire, DISPLAY_ADDRESS ^ 1)) {
    addr = DISPLAY_ADDRESS ^ 1;
  }
  // Run the Adafruit init even when no panel answered: it is what allocates
  // the frame buffer and the I2C device. Skipping it leaves i2c_dev and
  // spi_dev NULL, and UITask::begin() calls turnOn() regardless of our
  // return value, which then dereferences the null spi_dev.
  bool ok = display.begin(addr ? addr : DISPLAY_ADDRESS, true);
  return addr != 0 && ok;
}

void SH1106Display::turnOn()
{
  display.oled_command(SH110X_DISPLAYON);
  _isOn = true;
}

void SH1106Display::turnOff()
{
  display.oled_command(SH110X_DISPLAYOFF);
  _isOn = false;
}

void SH1106Display::clear()
{
  display.clearDisplay();
  display.display();
}

void SH1106Display::startFrame(ColorVal bkg)
{
  display.clearDisplay(); // TODO: apply 'bkg'
  _color = SH110X_WHITE;
  display.setTextColor(_color);
  setTextSize(1);
  display.cp437(true); // Use full 256 char 'Code Page 437' font
}

void SH1106Display::setTextSize(int sz)
{
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

void SH1106Display::setColor(ColorVal c)
{
  _color = c;
  display.setTextColor(_color);
}

void SH1106Display::setCursor(int x, int y)
{
  display.setCursor(x, y);
}

void SH1106Display::print(const char *str)
{
#ifdef DISPLAY_UTF8_FONTS
  if (hasNonASCII(str)) {
    printUTF8(str);
    return;
  }
#endif
  display.print(str);
}

#ifdef DISPLAY_UTF8_FONTS
void SH1106Display::printUTF8(const char *str)
{
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

void SH1106Display::fillRect(int x, int y, int w, int h)
{
  display.fillRect(x, y, w, h, _color);
}

void SH1106Display::drawRect(int x, int y, int w, int h)
{
  display.drawRect(x, y, w, h, _color);
}

void SH1106Display::drawXbm(int x, int y, const uint8_t *bits, int w, int h)
{
  display.drawBitmap(x, y, bits, w, h, _color);
}

uint16_t SH1106Display::getTextWidth(const char *str)
{
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

void SH1106Display::endFrame()
{
  display.display();
}
