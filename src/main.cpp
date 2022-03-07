#include <FastLED.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <uri/UriBraces.h>

#include <credentials.h>

#define NUM_LEDS   300
#define DATA_PIN   22

CRGB leds[NUM_LEDS];

#define HUE_MIN          192.0
#define HUE_MAX          0.0
#define RESISTANCE_MAX   70.0
#define RESISTANCE_MIN   25.0
#define MAX_SPEED        12
// ~60fps
#define FPS_DELAY        16
const int16_t RESISTANCE_RANGE = RESISTANCE_MAX - RESISTANCE_MIN;
const uint8_t BRIGHTNESS[8] = { 32, 64, 128, 192, 255, 192, 128, 64 };

uint32_t globalTimer;
uint16_t hue;
CRGB color;
WebServer server(80);
uint16_t cadence = 0;
uint16_t resistance = 0;

void setupNetwork() {
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CHSV(HUE_MIN, 255, BRIGHTNESS[(globalTimer / 2) & 0x7]);
    }
    FastLED.show();
    delay(FPS_DELAY);
    globalTimer++;
  }

  Serial.print("Connected to WiFi: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32")) {
    Serial.println("MDSN responder starter");
  }

  server.on(UriBraces("/bicycle/{}/{}"), []() {
    Serial.println("Adjusting Peloton lighting");
    cadence = server.pathArg(0).toInt();
    resistance = server.pathArg(1).toInt();
    server.send(200, "text/html", "OK");
  });

  server.begin();
}

void setup() {
  // put your setup code here, to run once:
  FastLED.addLeds<WS2812, DATA_PIN>(leds, NUM_LEDS);
  Serial.begin(115200);

  setupNetwork();

  globalTimer = 0;
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

  return retVal;
}

void setLightsForSpeedAndResistance(uint8_t speed, uint8_t resistance) {
  uint8_t speedModifier = abs((speed / 10) - MAX_SPEED) + 1;
  uint8_t timerSpeedReduced = globalTimer / speedModifier;
  uint8_t resistanceHue = getHueForResistance(resistance);
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(resistanceHue, 255, BRIGHTNESS[(i + timerSpeedReduced) & 0x7]);
  }
  FastLED.show();
}

void loop() {
  globalTimer++;
  server.handleClient();

  if (cadence > 0 && resistance > 0) {
    setLightsForSpeedAndResistance(cadence, resistance);
  }

  delay(FPS_DELAY);
}
