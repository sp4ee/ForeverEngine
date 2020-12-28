#include "display_oled.h"
#include <Arduino.h>
#include <Tiny4kOLED.h>

void Display::setup()
{
    Wire.begin((int)400000);
    oled.begin(128, 32, sizeof(tiny4koled_init_128x32), tiny4koled_init_128x32);
    oled.on();
    oled.switchRenderFrame();
    oled.setContrast(0x10);
    oled.clear();
    oled.setFont(FONT6X8CAPS);
    oled.setCursor(0, 0);
    oled.print(F("FOREVER ENGINE"));
    oled.setCursor(0, 2);
    oled.print(F("HELLO."));
    oled.switchFrame();
}

void Display::update(int16_t rpm, int16_t duty)
{
    oled.clear();
    snprintf(buf, DISPLAY_BUFSIZE, "RPM: %d  D: %d", rpm, duty);
    oled.setCursor(0, 0);
    oled.print(buf);
    oled.switchFrame();
}
