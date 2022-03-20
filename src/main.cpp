#include <FastLED.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <uri/UriBraces.h>

#include <credentials.h>
#include <Animator.h>
#include <BicycleAnimator.h>

#define NUM_LEDS   300
#define DATA_PIN   22

CRGB leds[NUM_LEDS];

// ~60fps
#define FPS_DELAY        16

// Global animators
Animator *currentAnimator;
BicycleAnimator *bicycleAnimator;

uint32_t globalTimer;
WebServer server(80);
uint16_t cadence = 0;
uint16_t resistance = 0;

void setAllLights(CRGB color) {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = color;
  }
  FastLED.show();
}

void setupNetwork() {
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    // flash while connecting to AP
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CHSV(HUE_MIN, 255, (i + globalTimer) % 2 == 0 ? 64 : 0);
    }
    FastLED.show();
    delay(FPS_DELAY);
    globalTimer++;
  }

  Serial.print("Connected to WiFi: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("exercise-lights")) {
    Serial.println("MDNS responder starter");
  }

  server.on(UriBraces("/bicycle/{}/{}"), []() {
    Serial.println("Adjusting Peloton lighting");
    uint16_t cadence = server.pathArg(0).toInt();
    uint16_t resistance = server.pathArg(1).toInt();
    bicycleAnimator->setTargetStatus(cadence, resistance, globalTimer);
    currentAnimator = bicycleAnimator;
    server.send(200, "text/html", "OK");
  });

  server.on(UriBraces("/hsv/{}/{}/{}"), []() {
    uint8_t h = server.pathArg(0).toInt();
    uint8_t s = server.pathArg(1).toInt();
    uint8_t v = server.pathArg(2).toInt();
    Serial.print("Setting lights to HSV value: ");
    Serial.print(h, HEX);
    Serial.print(s, HEX);
    Serial.println(v, HEX);
    setAllLights(CHSV(h, s, v));
    server.send(200, "text/html", "OK");
  });

  server.on(UriBraces("/rgb/{}/{}/{}"), []() {
    uint8_t r = server.pathArg(0).toInt();
    uint8_t g = server.pathArg(1).toInt();
    uint8_t b = server.pathArg(2).toInt();
    Serial.print("Setting lights to RGB value: ");
    Serial.print(r, HEX);
    Serial.print(g, HEX);
    Serial.println(b, HEX);
    setAllLights(CRGB(r, g, b));
    server.send(200, "text/html", "OK");
  });

  server.begin();

  MDNS.addService("http", "tcp", 80);

  // reset to off
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(0, 0, 0);
  }
  FastLED.show();
}

void setup() {
  // put your setup code here, to run once:
  FastLED.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS);
  Serial.begin(115200);

  bicycleAnimator = new BicycleAnimator(NUM_LEDS, leds);
  currentAnimator = bicycleAnimator;

  setupNetwork();

  globalTimer = 0;
}

void loop() {
  globalTimer++;
  server.handleClient();

  currentAnimator->renderFrame(globalTimer);
  FastLED.show();

  delay(FPS_DELAY);
}
