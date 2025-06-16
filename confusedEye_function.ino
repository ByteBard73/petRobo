#include <U8g2lib.h>
#include <Arduino.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);

// Configuration for confused expression
struct ConfusedEyeConfig {
  int centerY = 32;
  int eyeSize = 22;
  int cornerRadius = 4;
  int leftX = 24;
  int rightX = 128 - 24 - 22;
  unsigned long tiltInterval = 300;
  float maxTilt = 4.0;
};

struct ConfusedEyeState {
  float tiltOffset = 0;
  bool tiltingLeft = true;
  unsigned long lastTiltTime = 0;
};

ConfusedEyeConfig confusedConfig;
ConfusedEyeState confusedState;

void setup() {
  u8g2.begin();
  randomSeed(analogRead(A0));
}

// Draw square eyes with curved corners and asymmetric pupils
void drawConfusedEyes(float tiltOffset) {
  u8g2.clearBuffer();

  int y = confusedConfig.centerY + tiltOffset;

  // Left eye (larger pupil)
  u8g2.drawRBox(confusedConfig.leftX, y, confusedConfig.eyeSize, confusedConfig.eyeSize, confusedConfig.cornerRadius);
  u8g2.setDrawColor(0);
  u8g2.drawDisc(confusedConfig.leftX + confusedConfig.eyeSize / 2, y + confusedConfig.eyeSize / 2, 5);
  u8g2.setDrawColor(1);

  // Right eye (smaller pupil)
  u8g2.drawRBox(confusedConfig.rightX, y, confusedConfig.eyeSize, confusedConfig.eyeSize, confusedConfig.cornerRadius);
  u8g2.setDrawColor(0);
  u8g2.drawDisc(confusedConfig.rightX + confusedConfig.eyeSize / 2, y + confusedConfig.eyeSize / 2, 2);
  u8g2.setDrawColor(1);

  // Large question mark
  u8g2.setFont(u8g2_font_logisoso16_tf);
  u8g2.drawStr(4, 18, "?");

  u8g2.sendBuffer();
}

// 🌀 Public function to run confused eye animation
void confusedEyesFunction() {
  unsigned long now = millis();

  // Alternate tilt direction
  if (now - confusedState.lastTiltTime > confusedConfig.tiltInterval) {
    confusedState.tiltingLeft = !confusedState.tiltingLeft;
    confusedState.lastTiltTime = now;
  }

  // Smooth head tilt
  float target = confusedState.tiltingLeft ? -confusedConfig.maxTilt : confusedConfig.maxTilt;
  confusedState.tiltOffset += (target - confusedState.tiltOffset) * 0.1;

  drawConfusedEyes(confusedState.tiltOffset);
}

void loop() {
  confusedEyesFunction();
  delay(15);
}
