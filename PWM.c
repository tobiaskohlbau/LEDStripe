#include "PWM.h"

#include <avr/interrupt.h>
#include <avr/io.h>

volatile uint8_t pwm_channels[8] = {0, 0, 0, 0, 0, 255, 0, 0};

void initPWM(void)
{
    // PORTB as output
    DDRB = 0xFF; 

    TCCR1B = 1;
}

void startPWM(void)
{
    TIMSK1 |= (1 << OCIE1A);
}

void stopPWM(void)
{
    TIMSK1 &= ~(1 << OCIE1A);
}

void togglePWM(void)
{
    TIMSK1 ^= (1 << OCIE1A);
    if (!(TIMSK1 & (1 << OCIE1A)))
        PORTB = 0x00;
}

void setChannelPWM(uint8_t element, uint8_t value)
{
    cli();
    pwm_channels[element] = value;
    sei();
}

uint8_t getChannelPWM(uint8_t element)
{
    return pwm_channels[element];
}

ISR(TIMER1_COMPA_vect)
{
    static uint8_t pwm_counter = 0;
    uint8_t tmp = 0;

    OCR1A += (uint16_t) PWM;

    if (pwm_channels[0] > pwm_counter)
        tmp |= (1 << PB0);
    if (pwm_channels[1] > pwm_counter)
        tmp |= (1 << PB1);
    if (pwm_channels[2] > pwm_counter)
        tmp |= (1 << PB2);
    if (pwm_channels[3] > pwm_counter)
        tmp |= (1 << PB3);
    if (pwm_channels[4] > pwm_counter)
        tmp |= (1 << PB4);
    if (pwm_channels[5] > pwm_counter)
        tmp |= (1 << PB5);
    if (pwm_channels[6] > pwm_counter)
        tmp |= (1 << PB6);
    if (pwm_channels[7] > pwm_counter)
        tmp |= (1 << PB7);

    PORTB = tmp;

    if (pwm_counter == (uint8_t) (254))
        pwm_counter = 0;
    else
        pwm_counter++;
}
