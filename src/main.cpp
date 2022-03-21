#include <FastLED.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <uri/UriBraces.h>

#include <credentials.h>
#include <Animator.h>
#include <BicycleAnimator.h>
#include <PulseAnimator.h>

#define NUM_LEDS   360
#define DATA_PIN   22

CRGB leds[NUM_LEDS];

// ~60fps
#define FPS_DELAY        16

// Global animators
Animator *currentAnimator;
BicycleAnimator *bicycleAnimator;
PulseAnimator *pulseAnimator;

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

void endFrame() {
  FastLED.show();
  delay(FPS_DELAY);
  globalTimer++;
}

void setupNetwork() {
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  pulseAnimator->setColor(CRGB(0, 32, 64));
  pulseAnimator->setSpeed(120);
  while (WiFi.status() != WL_CONNECTED) {
    // flash while connecting to AP
    pulseAnimator->renderFrame(globalTimer);
    endFrame();
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
    pulseAnimator->setSpeed(0);
    pulseAnimator->setColor(CHSV(h, s, v));
    currentAnimator = pulseAnimator;
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
    pulseAnimator->setSpeed(0);
    pulseAnimator->setColor(CRGB(r, g, b));
    currentAnimator = pulseAnimator;
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
  pulseAnimator = new PulseAnimator(NUM_LEDS, leds);
  currentAnimator = bicycleAnimator;

  setupNetwork();

  globalTimer = 0;
}

void loop() {
  server.handleClient();

  currentAnimator->renderFrame(globalTimer);
  endFrame();
}
