#include <U8g2lib.h>
#include <Arduino.h>
#include <math.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);

// Configuration for dizzy expression
struct DizzyEyeConfig {
  int centerY = 32;
  int maxRadius = 13;
  int leftX = 36;
  int rightX = 128 - 36;
  float rotationSpeed = 0.35;
  int spiralPoints = 60;
};

struct DizzyEyeState {
  float angle = 0;
};

DizzyEyeConfig dizzyConfig;
DizzyEyeState dizzyState;

void setup() {
  u8g2.begin();
}

// Spiral eye with thick bold lines
void drawSpiralEye(int cx, int cy, float baseAngle) {
  float radiusStep = (float)dizzyConfig.maxRadius / dizzyConfig.spiralPoints;
  float angleStep = 0.3;

  for (int i = 0; i < dizzyConfig.spiralPoints - 1; i++) {
    float r1 = i * radiusStep;
    float r2 = (i + 1) * radiusStep;

    float a1 = baseAngle + i * angleStep;
    float a2 = baseAngle + (i + 1) * angleStep;

    int x1 = cx + cos(a1) * r1;
    int y1 = cy + sin(a1) * r1;
    int x2 = cx + cos(a2) * r2;
    int y2 = cy + sin(a2) * r2;

    // Thick line effect
    u8g2.drawLine(x1, y1, x2, y2);
    u8g2.drawLine(x1 + 1, y1, x2 + 1, y2);
    u8g2.drawLine(x1, y1 + 1, x2, y2 + 1);
  }
}

// Main function for dizzy expression
void dizzyEyesFunction() {
  dizzyState.angle += dizzyConfig.rotationSpeed;

  u8g2.clearBuffer();
  drawSpiralEye(dizzyConfig.leftX, dizzyConfig.centerY, dizzyState.angle);
  drawSpiralEye(dizzyConfig.rightX, dizzyConfig.centerY, dizzyState.angle);
  u8g2.sendBuffer();
}

void loop() {
  dizzyEyesFunction();
  delay(20);
}
