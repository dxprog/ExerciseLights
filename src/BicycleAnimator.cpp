#include <BicycleAnimator.h>

const int16_t RESISTANCE_RANGE = RESISTANCE_MAX - RESISTANCE_MIN;
const uint8_t BRIGHTNESS[8] = { 32, 64, 128, 192, 255, 192, 128, 64 };

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

BicycleAnimator::BicycleAnimator(uint16_t numLeds, CRGB *leds) {
  this->numLeds = numLeds;
  this->leds = leds;
  this->currentCadence = 0;
  this->currentResistance = 0;
  this->targetAnimationTimer = 0;
  this->targetCadence = 0;
  this->targetResistance = 0;
}

void BicycleAnimator::renderFrame(uint32_t timer) {
  uint8_t cadence = this->currentCadence;
  uint8_t resistance = this->currentResistance;

  if (this->targetAnimationTimer > timer) {
    double startTime = this->targetAnimationTimer - TARGET_DURATION;
    double timerDelta = (timer - startTime) / TARGET_DURATION;
    int8_t cadenceDelta = this->targetCadence - this->currentCadence;
    int8_t resistanceDelta = this->targetResistance - this->currentResistance;
    cadence = this->currentCadence + (int8_t) (timerDelta * cadenceDelta);
    resistance = this->currentResistance + (int8_t) (timerDelta * resistanceDelta);
  } else {
    this->currentCadence = cadence = this->targetCadence;
    this->currentResistance = resistance = this->targetResistance;
  }

  uint8_t speedModifier = abs((cadence / 10) - MAX_SPEED) + 1;
  uint8_t timerSpeedReduced = timer / speedModifier;
  uint8_t resistanceHue = getHueForResistance(resistance);
  for (int i = 0; i < this->numLeds; i++) {
    this->leds[i] = CHSV(resistanceHue, 255, BRIGHTNESS[(i + timerSpeedReduced) & 0x7]);
  }
}

void BicycleAnimator::setTargetStatus(uint8_t cadence, uint8_t resistance, uint32_t timer) {
  this->targetCadence = cadence;
  this->targetResistance = resistance;
  this->targetAnimationTimer = timer + TARGET_DURATION;
}
