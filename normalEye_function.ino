#include <U8g2lib.h>
#include <Arduino.h>

// OLED Display
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);

// Eye Properties
struct EyeConfig {
  int pupilSize = 26;
  int cornerRadius = 5;
  int eyeCenterY = 32;
  int leftEyeX = 26;
  int rightEyeX = 76;
  unsigned long moveInterval = 2300;
  unsigned long blinkIntervalMin = 3000;
  unsigned long blinkIntervalMax = 7000;
  int blinkDuration = 150;
};

// Eye animation state
struct EyeState {
  float currentX = 0;
  float currentY = 0;
  float targetX = 0;
  float targetY = 0;
  unsigned long lastMoveTime = 0;

  // Blinking
  bool isBlinking = false;
  unsigned long lastBlinkTime = 0;
  unsigned long nextBlinkTime = 0;
};

EyeConfig config;
EyeState state;

void setup() {
  u8g2.begin();
  randomSeed(analogRead(A0));
  state.lastBlinkTime = millis();
  state.nextBlinkTime = random(config.blinkIntervalMin, config.blinkIntervalMax);
}

void drawEyes(bool closed) {
  u8g2.clearBuffer();

  if (closed) {
    u8g2.drawBox(config.leftEyeX, config.eyeCenterY, config.pupilSize, 3);
    u8g2.drawBox(config.rightEyeX, config.eyeCenterY, config.pupilSize, 3);
  } else {
    int top = config.eyeCenterY - config.pupilSize / 2 + state.currentY;

    u8g2.drawRBox(config.leftEyeX + state.currentX, top,
                  config.pupilSize, config.pupilSize,
                  config.cornerRadius);
    u8g2.drawRBox(config.rightEyeX + state.currentX, top,
                  config.pupilSize, config.pupilSize,
                  config.cornerRadius);
  }

  u8g2.sendBuffer();
}

// ✅ Renamed animation function
void normalEye() {
  unsigned long now = millis();

  // Blinking
  if (!state.isBlinking && now - state.lastBlinkTime > state.nextBlinkTime) {
    state.isBlinking = true;
    state.lastBlinkTime = now;
    state.nextBlinkTime = random(config.blinkIntervalMin, config.blinkIntervalMax);
  }

  if (state.isBlinking) {
    if (now - state.lastBlinkTime < config.blinkDuration) {
      drawEyes(true); // Eyes closed
      return;
    } else {
      state.isBlinking = false;
    }
  }

  // Wandering movement
  if (now - state.lastMoveTime > config.moveInterval) {
    state.targetX = random(-7, 8);
    state.targetY = random(-5, 6);
    state.lastMoveTime = now;
  }

  state.currentX += (state.targetX - state.currentX) * 0.4;
  state.currentY += (state.targetY - state.currentY) * 0.4;

  drawEyes(false); // Eyes open
}

void loop() {
  normalEye();   // 👈 Just calls the renamed function
  delay(30);
}
