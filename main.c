#include <avr/io.h>
#include <avr/interrupt.h>

#include "NEC.h"
#include "UART.h"

int main (void)
{
    // set PB5 as output
    DDRB |= (1 << DDB5);

    initUART();
    initNEC();

    sei();
    while(1) {
    }
}

