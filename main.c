#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>

#include "NEC.h"
#include "UART.h"
#include "PWM.h"

#define POWER_OFF 0X40
#define RED_UP 0x14
#define RED_DOWN 0x10

void commands(uint8_t command)
{
    uint8_t tmp;
    switch(command)
    {
        case RED_UP:
            tmp = getChannelPWM(PB5);
            if (tmp < 255)
                setChannelPWM(PB5, ++tmp);
            break;
        case RED_DOWN:
            tmp = getChannelPWM(PB5);
            if (tmp > 0)
                setChannelPWM(PB5, --tmp);
            break;
        case POWER_OFF:
            togglePWM();
            _delay_ms(1000);
            break;
        default:
            sendCharUART(command);
    }
}

int main (void)
{
    initUART();
    initNEC();
    initPWM();

    sei();
    while(1) {
        //if (nec.status & (1 << NEC_READY))
        //{
        //    tmp_command = nec.command;
        //    commands(tmp_command);
        //    nec.status &= ~(1 << NEC_READY);
        //}

        //if (nec.status & (1 << NEC_REPEAT))
        //{
        //    commands(tmp_command);

        //    nec.status &= ~(1 << NEC_REPEAT);
        //}

        if (uart.ready)
        {
           setChannelPWM(uart.buffer[0], uart.buffer[1]);
           uart.ready = 0;
        }
    }
}

