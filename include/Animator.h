#ifndef __ANIMATOR_H__
#define __ANIMATOR_H__

#include <FastLED.h>

class Animator {
  protected:
    uint16_t numLeds;
    CRGB *leds;

  public:
    virtual void renderFrame(uint32_t timer);
};

#endif
