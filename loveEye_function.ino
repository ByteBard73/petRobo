#include <U8g2lib.h>
#include <Arduino.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);

// Configuration for love eyes
struct LoveEyeConfig {
  int centerY = 32;
  int size = 20;            // Bigger heart size
  int leftX = 32;
  int rightX = 128 - 32;
  unsigned long trembleInterval = 150;
};

struct LoveEyeState {
  float tremble = 0;
  unsigned long lastTrembleTime = 0;
};

LoveEyeConfig loveConfig;
LoveEyeState loveState;

void setup() {
  u8g2.begin();
  randomSeed(analogRead(0));
}

// Draw thick filled heart at (x, y)
void drawHeartEye(int x, int y) {
  int s = loveConfig.size;

  // Top left and right bumps (thicker)
  for (int r = s / 4; r > 0; r--) {
    u8g2.drawDisc(x - s / 4, y, r, U8G2_DRAW_ALL);
    u8g2.drawDisc(x + s / 4, y, r, U8G2_DRAW_ALL);
  }

  // Thick bottom triangle
  for (int i = 0; i < 4; i++) {
    u8g2.drawTriangle(
      x - s / 1.9 + i, y + i,
      x + s / 1.6 - i, y + i,
      x, y + s
    );
  }
}

// Render both love eyes
void drawLoveEyes(float offsetY) {
  u8g2.clearBuffer();

  int y = loveConfig.centerY + offsetY;

  drawHeartEye(loveConfig.leftX, y);
  drawHeartEye(loveConfig.rightX, y);

  u8g2.sendBuffer();
}

// ✅ This is your final loveEyes() function
void loveEyes() {
  unsigned long now = millis();

  // Tremble effect
  if (now - loveState.lastTrembleTime > loveConfig.trembleInterval) {
    loveState.tremble = random(-1, 2); // Small vertical bounce
    loveState.lastTrembleTime = now;
  }

  drawLoveEyes(loveState.tremble);
}

void loop() {
  loveEyes();
  delay(30);
}
