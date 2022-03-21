#ifndef __BICYCLE_ANIMATOR_H__
#define __BICYCLE_ANIMATOR_H__

#include <Animator.h>

#define HUE_MIN          192.0
#define HUE_MAX          0.0
#define RESISTANCE_MAX   70.0
#define RESISTANCE_MIN   25.0
#define MAX_SPEED        12
#define TARGET_DURATION  60.0 // 60 frames to catch up on a target stats change

class BicycleAnimator: public Animator {
  private:
    uint32_t targetAnimationTimer;
    uint8_t targetCadence;
    uint8_t targetResistance;
    uint8_t currentCadence;
    uint8_t currentResistance;

  public:
    BicycleAnimator(uint16_t numLeds, CRGB *leds);
    void renderFrame(uint32_t timer);
    void setTargetStatus(uint8_t cadence, uint8_t resistance, uint32_t timer);
};

#endif
