#include <U8g2lib.h>
#include <Arduino.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);

// Configuration structure
struct AngryEyeConfig {
  int centerY = 32;
  int width = 28;
  int height = 22;
  int leftX = 26;
  int cornerRadius = 5;
  float maxOffset = 4.0;
  unsigned long trembleInterval = 150;
};

// Animation state
struct AngryEyeState {
  float currentOffset = 0;
  float targetOffset = 0;
  float tremble = 0;
  unsigned long lastTrembleTime = 0;
};

AngryEyeConfig angryConfig;
AngryEyeState angryState;

void setup() {
  u8g2.begin();
  randomSeed(analogRead(0));
  setAngerLevel(1.0);  // Start fully angry
}

// Draw angry eyes
void drawAngryEyes(float yOffset) {
  u8g2.clearBuffer();

  int rightX = 128 - angryConfig.leftX - angryConfig.width;
  int topY = angryConfig.centerY + yOffset;

  // Left Eye (sloped UP to right)
  u8g2.drawTriangle(
    angryConfig.leftX, topY,
    angryConfig.leftX + angryConfig.width, topY + 6,
    angryConfig.leftX + angryConfig.width, topY + angryConfig.height
  );
  u8g2.drawRBox(
    angryConfig.leftX, topY + 6,
    angryConfig.width, angryConfig.height - 6,
    angryConfig.cornerRadius
  );

  // Right Eye (sloped UP to left)
  u8g2.drawTriangle(
    rightX + angryConfig.width, topY,
    rightX, topY + 6,
    rightX, topY + angryConfig.height
  );
  u8g2.drawRBox(
    rightX, topY + 6,
    angryConfig.width, angryConfig.height - 6,
    angryConfig.cornerRadius
  );

  u8g2.sendBuffer();
}

// Angry face animation function
void angryEyesFunction() {
  unsigned long now = millis();

  // Smooth transition
  angryState.currentOffset += (angryState.targetOffset - angryState.currentOffset) * 0.1;

  // Tremble effect
  if (now - angryState.lastTrembleTime > angryConfig.trembleInterval) {
    angryState.tremble = random(-1, 2) * 0.5f;
    angryState.lastTrembleTime = now;
  }

  drawAngryEyes(angryState.currentOffset + angryState.tremble);
}

// Set anger intensity (0.0 to 1.0)
void setAngerLevel(float level) {
  angryState.targetOffset = -level * angryConfig.maxOffset;
}

void loop() {
  angryEyesFunction();
  delay(30);
}
