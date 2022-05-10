#include <RgbAnimator.h>

RgbAnimator::RgbAnimator(uint16_t numLeds, CRGB *leds) {
  this->numLeds = numLeds;
  this->leds = leds;
  targetAnimationTimer = 0;
  currentColor = CRGB::Black;
  targetColor = CRGB::Black;
}

void RgbAnimator::renderFrame(uint32_t timer) {
  CRGB frameColor;
  if (targetAnimationTimer > timer) {
    double startTime = targetAnimationTimer - RGB_ANIMATION_DURATION;
    double timerDelta = (timer - startTime) / RGB_ANIMATION_DURATION;
    frameColor = CRGB(
      currentColor.r + (uint8_t) (stepR * timerDelta),
      currentColor.g + (uint8_t) (stepG * timerDelta),
      currentColor.b + (uint8_t) (stepB * timerDelta)
    );
  } else {
    frameColor = currentColor = targetColor;
  }

  for (uint16_t i = 0; i < numLeds; i++) {
    leds[i] = frameColor;
  }
}

void RgbAnimator::setTargetRgb(CRGB newColor, uint32_t timer) {
  targetColor = newColor;
  stepR = (double)(newColor.r - currentColor.r);
  stepG = (double)(newColor.g - currentColor.g);
  stepB = (double)(newColor.b - currentColor.b);
  targetAnimationTimer = timer + RGB_ANIMATION_DURATION;
}
