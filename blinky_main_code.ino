#include <U8g2lib.h>
#include <Arduino.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);

#define IR A0
#define SOUND A1
#define BUZZER 3

#define SOUND_THRESHOLD 30
#define FEAR_THRESHOLD 700
#define SLEEP_TIMEOUT 15000
#define WAKE_COOLDOWN 3000
#define SHIVER_DURATION 1500
#define RECOVER_DURATION 500
#define MIN_RESPOND_DURATION 2000

unsigned long t_now, t_lastActive, t_stateStart, t_phaseStart;
bool isHappy, isSleeping, wakeCooldown, isFear, isShivering, isRecovering;
bool isSpeaking, isResponding;

unsigned long speakingStart, respondDuration, lastBeepTime, beepInterval;
unsigned long t_lastMove, t_blink, t_nextBlink;
int cx, cy, tx, ty;
bool blink;

void playBeepOnce() {
  digitalWrite(BUZZER, HIGH); delay(100);
  digitalWrite(BUZZER, LOW); delay(100);
}

int soundLevel() {
  int minV = 1023, maxV = 0;
  for (uint8_t i = 0; i < 20; i++) {
    int v = analogRead(SOUND);
    if (v < minV) minV = v;
    if (v > maxV) maxV = v;
    delay(1);
  }
  return maxV - minV;
}

void drawEyes(bool closed, int jx = 0, int jy = 0) {
  u8g2.clearBuffer();
  if (closed) {
    u8g2.drawBox(26, 32, 26, 3);
    u8g2.drawBox(76, 32, 26, 3);
  } else {
    int y = 19 + cy + jy;
    u8g2.drawRBox(26 + cx + jx, y, 26, 26, 5);
    u8g2.drawRBox(76 + cx + jx, y, 26, 26, 5);
  }
  u8g2.sendBuffer();
}

void drawHappy() {
  u8g2.clearBuffer();
  for (uint8_t i = 0; i <= 38; i++) {
    int y = 48 - sin(i * PI / 38.0) * 10;
    u8g2.drawLine(16 + i, y, 16 + i, 48);
    u8g2.drawLine(74 + i, y, 74 + i, 48);
  }
  u8g2.sendBuffer();
}

void drawSleep() {
  float offset = sin(millis() * 0.0015f) * 2;
  int y = 32 + offset;
  u8g2.clearBuffer();
  u8g2.drawLine(28, y, 56, y);
  u8g2.drawLine(72, y, 100, y);
  u8g2.setFont(u8g2_font_6x12_tr);
  u8g2.drawStr(5, 12, "Z");
  u8g2.drawStr(11, 8, "z");
  u8g2.sendBuffer();
}

void drawFear() {
  static float ratio = 1.0;
  static bool closing = true;
  static float offset = 0;
  if (millis() % 30 == 0) offset = random(-3, 4);
  if (closing) {
    ratio -= 0.04;
    if (ratio <= 0) {
      ratio = 0;
      closing = false;
      t_phaseStart = millis();
    }
  } else if (millis() - t_phaseStart > 4000) {
    ratio = 1.0;
    closing = true;
  }
  playBeepOnce();
  u8g2.clearBuffer();
  int y = 30 + offset;
  int r = max(1, (int)(15 * ratio));
  int p = max(1, (int)(4 * ratio));
  u8g2.drawDisc(32, y, r); u8g2.setDrawColor(0); u8g2.drawDisc(32, y, p);
  u8g2.setDrawColor(1);    u8g2.drawDisc(96, y, r); u8g2.setDrawColor(0); u8g2.drawDisc(96, y, p);
  u8g2.setDrawColor(1);    u8g2.sendBuffer();
}

void drawRespond() {
  float pulse = sin(millis() * 0.015f) * 2 + 5;
  u8g2.clearBuffer();
  u8g2.drawRBox(26 + cx, 20 + cy, 26, 26, pulse);
  u8g2.drawRBox(76 + cx, 20 + cy, 26, 26, pulse);
  u8g2.sendBuffer();
}

void setup() {
  u8g2.begin();
  pinMode(BUZZER, OUTPUT);
  randomSeed(analogRead(IR));
  t_lastActive = millis();
  t_nextBlink = random(3000, 6000);
}

void loop() {
  t_now = millis();
  int ir = analogRead(IR);
  int sound = soundLevel();

  if (sound > FEAR_THRESHOLD) {
    isFear = true; isShivering = isRecovering = false;
  } else if (isFear && sound < FEAR_THRESHOLD - 20) {
    isFear = false; isShivering = true; t_phaseStart = t_now;
  }

  if (isSleeping && (ir < 500 || sound > SOUND_THRESHOLD)) {
    isSleeping = false; wakeCooldown = true;
    t_stateStart = t_lastActive = t_now;
  }

  if (!isSleeping && t_now - t_lastActive > SLEEP_TIMEOUT) {
    isSleeping = true;
    isHappy = wakeCooldown = isSpeaking = isResponding = false;
  }

  if (isSleeping) {
    drawSleep(); delay(15); return;
  }

  if (wakeCooldown && t_now - t_stateStart < WAKE_COOLDOWN) {
    drawEyes(false); delay(30); return;
  } else wakeCooldown = false;

  if (ir < 500) {
    isHappy = true;
    t_stateStart = t_now;
  }

  if (ir < 500 || sound > SOUND_THRESHOLD || isHappy || isSpeaking || isResponding)
    t_lastActive = t_now;

  if (isFear) {
    drawFear(); delay(15); return;
  }

  if (isShivering) {
    drawEyes(false, random(-3, 4), random(-3, 4));
    if (t_now - t_phaseStart > SHIVER_DURATION) {
      isShivering = false; isRecovering = true; t_phaseStart = t_now;
    }
    delay(20); return;
  }

  if (isRecovering) {
    drawEyes(false);
    if (t_now - t_phaseStart > RECOVER_DURATION) isRecovering = false;
    delay(20); return;
  }

  if (sound > SOUND_THRESHOLD && !isSpeaking && !isResponding) {
    isSpeaking = true;
    speakingStart = t_now;
  }

  if (isSpeaking) {
    drawEyes(false);
    if (sound < SOUND_THRESHOLD && t_now - speakingStart > 500) {
      isSpeaking = false;
      isResponding = true;
      t_stateStart = t_now;
      respondDuration = max((t_now - speakingStart) / 2, (unsigned long)MIN_RESPOND_DURATION);
      beepInterval = random(250, 450);
      lastBeepTime = t_now;
    }
    delay(15); return;
  }

  if (isResponding) {
    drawRespond();
    if (t_now - lastBeepTime > beepInterval) {
      playBeepOnce();
      lastBeepTime = t_now;
      beepInterval = random(250, 450);
    }
    if (t_now - t_stateStart > respondDuration) isResponding = false;
    delay(15); return;
  }

  if (isHappy) {
    drawHappy();
    if (t_now - t_stateStart > 3000) isHappy = false;
    delay(15); return;
  }

  // Idle with blinking and movement
  if (!blink && t_now - t_blink > t_nextBlink) {
    blink = true;
    t_blink = t_now;
    t_nextBlink = random(3000, 6000);
  }

  if (blink && t_now - t_blink < 150) {
    drawEyes(true);
  } else {
    blink = false;
    if (t_now - t_lastMove > 2300) {
      tx = random(-7, 8); ty = random(-5, 6);
      t_lastMove = t_now;
    }
    cx += (tx - cx) / 2;
    cy += (ty - cy) / 2;
    drawEyes(false);
  }

  delay(30);
}
