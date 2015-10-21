#ifndef PWM_H
#define PWM_H

#include <stdint.h>

// 255 steps with 100Hz
#define PWM 627

extern volatile uint8_t pwm_channels[8];

void initPWM(void);
void setChannelPWM(uint8_t element, uint8_t value);
uint8_t getChannelPWM(uint8_t element);
void startPWM(void);
void stopPWM(void);
void togglePWM(void);

#endif /* PWM_H */
