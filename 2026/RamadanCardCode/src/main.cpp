#include <Arduino.h>
#include <FastLED.h>

#define LED_PIN     0
#define COLOR_ORDER GRB
#define CHIPSET     WS2812B
#define NUM_LEDS    16
#define BRIGHTNESS  100
#define BUTTON_PIN  9
#define HOLD_MS     400
#define CLICK_GAP   350

CRGB leds[NUM_LEDS];

uint8_t mode = 0;
uint8_t clickCount = 0;
bool lastBtn = false;
bool holding = false;
unsigned long pressTime = 0;
unsigned long releaseTime = 0;
uint8_t seq0 = 0, seq1 = 0, seq2 = 0;

void twinkle() {
  fadeToBlackBy(leds, NUM_LEDS, 20);
  if (random8() < 80)
    leds[random8(NUM_LEDS)] = CRGB(255, 180, 50);
}

void breathe() {
  uint8_t t = millis() >> 3;
  uint8_t v = (t < 128) ? t * 2 : (255 - t) * 2;
  fill_solid(leds, NUM_LEDS, CRGB(v, v >> 1, 0));
}

void rainbow() {
  fill_rainbow(leds, NUM_LEDS, millis() / 20, 28);
}

// 424 easter egg: lights up 4, 2, 4 stars one by one
void show424() {
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  uint8_t p = 0;
  for (uint8_t d = 0; d < 3; d++) {
    for (uint8_t i = 0; i < ((d == 1) ? 2 : 4); i++) {
      leds[p++] = CRGB(255, 180, 50);
      FastLED.show(); delay(200);
    }
    delay(500);
  }
}

void setup() {
  delay(500);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
}

void loop() {
  bool btn = (digitalRead(BUTTON_PIN) == LOW);
  unsigned long now = millis();

  if (btn && !lastBtn) { pressTime = now; holding = false; }
  if (btn && !holding && (now - pressTime > HOLD_MS)) { holding = true; clickCount = 0; }
  if (!btn && lastBtn) {
    if (!holding) { clickCount++; releaseTime = now; }
    holding = false;
  }
  if (clickCount > 0 && !btn && (now - releaseTime > CLICK_GAP)) {
    seq0 = seq1; seq1 = seq2; seq2 = clickCount;
    if (seq0 == 4 && seq1 == 2 && seq2 == 4) {
      show424(); mode = 0; seq0 = seq1 = seq2 = 0;
    } else if (clickCount >= 3) mode = 3;
    else if (clickCount == 2) mode = 2;
    else mode = (mode == 0) ? 1 : 0;
    clickCount = 0;
  }

  if (btn && holding) rainbow();
  else if (mode == 1) twinkle();
  else if (mode == 2) breathe();
  else if (mode == 3) rainbow();
  else fill_solid(leds, NUM_LEDS, CRGB::Black);

  FastLED.show();
  delay(16);
  lastBtn = btn;
}
