#include <U8g2lib.h>
#include <Arduino.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);

int pulseOffset = 0;

void setup() {
  u8g2.begin();
}

void listening() {
  u8g2.clearBuffer();

  for (int x = 0; x < 128; x += 6) {
    int height = 10 + 6 * sin((x + pulseOffset) * 0.2);
    int yTop = 32 - height / 2;
    u8g2.drawBox(x, yTop, 4, height); // draw vertical pulse bar
  }

  u8g2.sendBuffer();
  pulseOffset += 3;
}

void loop() {
  listening();
  delay(30);
}
