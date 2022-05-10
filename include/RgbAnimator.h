#ifndef __RGB_ANIMATOR_H__
#define __BICYCLE_ANIMATOR_H__

#include <Animator.h>
#include <FastLED.h>

#define RGB_ANIMATION_DURATION  120.0 // 120 frames to catch up on a target stats change

class RgbAnimator: public Animator {
  private:
    uint32_t targetAnimationTimer;
    CRGB targetColor;
    CRGB currentColor;
    double stepR;
    double stepG;
    double stepB;

  public:
    RgbAnimator(uint16_t numLeds, CRGB *leds);
    void renderFrame(uint32_t timer);
    void setTargetRgb(CRGB newColor, uint32_t timer);
};

#endif
