#include <U8g2lib.h>
#include <Arduino.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);

// Configuration
struct FearEyeConfig {
  int centerY = 30;
  int eyeRadius = 15;
  int pupilRadius = 4;
  int leftX = 32;
  int rightX = 96;
  float maxTremble = 2.5;
  unsigned long trembleInterval = 40;
  float closingSpeed = 0.04; // Eye closes in ~1s
  unsigned long closedHoldDuration = 4000; // Hold closed for 4s
};

struct FearEyeState {
  float trembleOffset = 0;
  float eyeOpenRatio = 1.0;  // 1 = open, 0 = closed
  bool isClosing = true;
  bool isHoldingClosed = false;
  unsigned long lastTremble = 0;
  unsigned long closedStartTime = 0;
};

FearEyeConfig fearConfig;
FearEyeState fearState;

void setup() {
  u8g2.begin();
  randomSeed(analogRead(A0));
}

// Draw trembling, animating fearful eyes
void drawFearEyes(float yOffset, float openRatio) {
  u8g2.clearBuffer();

  int y = fearConfig.centerY + yOffset;
  int r = max(1, int(fearConfig.eyeRadius * openRatio));
  int p = max(1, int(fearConfig.pupilRadius * openRatio));

  // Left Eye
  u8g2.drawDisc(fearConfig.leftX, y, r);
  u8g2.setDrawColor(0);
  u8g2.drawDisc(fearConfig.leftX, y, p);
  u8g2.setDrawColor(1);

  // Right Eye
  u8g2.drawDisc(fearConfig.rightX, y, r);
  u8g2.setDrawColor(0);
  u8g2.drawDisc(fearConfig.rightX, y, p);
  u8g2.setDrawColor(1);

  u8g2.sendBuffer();
}

// MAIN FUNCTION: Call this in loop
void fearEyes() {
  unsigned long now = millis();

  // Tremble logic
  if (now - fearState.lastTremble > fearConfig.trembleInterval) {
    fearState.trembleOffset = random(-10, 11) * 0.3; // Wider tremble
    fearState.lastTremble = now;
  }

  // Eye closing animation
  if (fearState.isClosing) {
    fearState.eyeOpenRatio -= fearConfig.closingSpeed;
    if (fearState.eyeOpenRatio <= 0.0) {
      fearState.eyeOpenRatio = 0.0;
      fearState.isClosing = false;
      fearState.isHoldingClosed = true;
      fearState.closedStartTime = now;
    }
  } else if (fearState.isHoldingClosed) {
    if (now - fearState.closedStartTime > fearConfig.closedHoldDuration) {
      fearState.eyeOpenRatio = 1.0;
      fearState.isHoldingClosed = false;
      fearState.isClosing = true;
    }
  }

  drawFearEyes(fearState.trembleOffset, fearState.eyeOpenRatio);
}

void loop() {
  fearEyes();
  delay(15);
}
