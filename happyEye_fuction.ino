#include <U8g2lib.h>
#include <Arduino.h>

// OLED Display Setup
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);

// Eye Configuration (Increased size)
const int eyeCenterY = 32;
const int eyeWidth = 38;     // Wider eyes
const int eyeHeight = 16;    // Taller eyes
const int leftEyeX = 16;
const int rightEyeX = 128 - leftEyeX - eyeWidth;

// Animation variables
float currentY = 0;
float targetY = -4;          // Wider motion
float tremble = 0;
unsigned long lastUpdate = 0;

void setup() {
  u8g2.begin();
  randomSeed(analogRead(A0));
}

// Draw "⌒"-shaped curved filled eye (larger version)
void drawSmileEye(int x, int y) {
  int steps = eyeWidth;
  int bottomY = y + eyeHeight;

  for (int i = 0; i <= steps; i++) {
    float t = (float)i / steps;
    int px = x + t * eyeWidth;

    float curveOffset = sin(t * PI) * 10;  // Curve height
    int curveTop = bottomY - curveOffset;

    u8g2.drawLine(px, curveTop, px, bottomY);
  }
}

// Draw both eyes with vertical offset
void drawHappyEyes(float yOffset) {
  u8g2.clearBuffer();
  int topY = eyeCenterY + yOffset - eyeHeight / 2;

  drawSmileEye(leftEyeX, topY);
  drawSmileEye(rightEyeX, topY);

  u8g2.sendBuffer();
}

// Encapsulated Happy Eye Function
void happyEyeFunction() {
  unsigned long now = millis();

  // Smooth easing
  currentY += (targetY - currentY) * 0.05;

  // Light trembling every 150ms
  if (now - lastUpdate > 150) {
    tremble = random(-1, 2);
    lastUpdate = now;
  }

  drawHappyEyes(currentY + tremble);
}

// Main loop
void loop() {
  happyEyeFunction();
  delay(30);
}
