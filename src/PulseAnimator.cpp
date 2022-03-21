#include <PulseAnimator.h>
#include <math.h>

PulseAnimator::PulseAnimator(uint16_t numLeds, CRGB *leds) {
  this->leds = leds;
  this->numLeds = numLeds;
  this->speed = DEFAULT_PULSE_SPEED;
  this->color = CRGB(0, 0, 0);
}

void PulseAnimator::renderFrame(uint32_t timer) {
  CRGB color = this->color;
  if (this->speed > 0) {
    // this is highly inefficient because I am bad at math
    double pulsePositionDeg = (timer % this->speed) / (double) this->speed * RADS_IN_CIRCLE;
    uint8_t adjustedBrightness = (uint8_t) (abs(sin(pulsePositionDeg / 2)) * 255.0);
    color.fadeToBlackBy(adjustedBrightness);
  }

  for (uint16_t i = 0; i < this->numLeds; i++) {
    leds[i] = color;
  }
}

void PulseAnimator::setSpeed(uint8_t speed) {
  this->speed = speed;
}

void PulseAnimator::setColor(CHSV hsv) {
  this->color = CHSV(hsv);
}

void PulseAnimator::setColor(CRGB rgb) {
  this->color = rgb;
}
