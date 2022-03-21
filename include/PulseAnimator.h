#ifndef __PULSE_ANIMATOR_H__
#define __PULSE_ANIMATOR_H__

#include <Animator.h>

#define DEFAULT_PULSE_SPEED 120
#define RADS_IN_CIRCLE      6.28318530718


class PulseAnimator: public Animator {
  private:
    uint8_t speed;
    CRGB color;

  public:
    PulseAnimator(uint16_t numLeds, CRGB *leds);
    void renderFrame(uint32_t timer);
    void setSpeed(uint8_t speed);
    void setColor(CHSV hsv);
    void setColor(CRGB rgb);
};

#endif
