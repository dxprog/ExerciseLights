#include <FastLED.h>
// #include <WiFi.h>
// #include <WiFiClient.h>
// #include <WebServer.h>
// #include <ESPmDNS.h>

#define NUM_LEDS   120
#define DATA_PIN   22

CRGB leds[NUM_LEDS];

#define HUE_MIN          192.0
#define HUE_MAX          0.0
#define RESISTANCE_MAX   70.0
#define RESISTANCE_MIN   25.0
#define MAX_SPEED        12
const int16_t RESISTANCE_RANGE = RESISTANCE_MAX - RESISTANCE_MIN;
const uint8_t BRIGHTNESS[8] = { 32, 64, 128, 192, 255, 192, 128, 64 };

uint32_t timer;
uint16_t hue;
CRGB color;

void setup() {
  // put your setup code here, to run once:
  FastLED.addLeds<WS2812, DATA_PIN>(leds, NUM_LEDS);
  Serial.begin(115200);

  timer = RESISTANCE_MIN;
}

uint8_t getHueForResistance(uint8_t resistance) {
  uint8_t retVal;

  if (resistance >= RESISTANCE_MAX) {
    retVal = HUE_MAX;
  } else if (resistance <= RESISTANCE_MIN) {
    retVal = HUE_MIN;
  } else {
    double resistancePercent = ((int16_t) resistance - RESISTANCE_MIN) / RESISTANCE_RANGE;
    double resistanceInv = (resistancePercent - 1) * -1;
    double hue = HUE_MIN * resistanceInv;
    retVal = hue;
  }

  Serial.println(retVal);

  return retVal;
}

void setLightsForSpeedAndResistance(uint8_t speed, uint8_t resistance) {
  uint8_t speedModifier = abs((speed / 10) - MAX_SPEED) + 1;
  uint8_t timerSpeedReduced = timer / speedModifier;
  uint8_t resistanceHue = getHueForResistance(resistance);
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(resistanceHue, 255, BRIGHTNESS[(i + timerSpeedReduced) & 0x7]);
  }
  FastLED.show();
}

void loop() {
  timer++;
  setLightsForSpeedAndResistance(120, 35);
  delay(16);
}
