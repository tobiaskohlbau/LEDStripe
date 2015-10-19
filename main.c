#include <avr/io.h>
#include <util/delay.h>

#include <avr/interrupt.h>

#define UART_BAUD 9600UL

#define UART_UBRR_VALUE ((F_CPU+UART_BAUD*8)/(UART_BAUD*16)-1)

volatile uint8_t tics = 0;
volatile uint16_t ms = 0;

volatile uint8_t tics_begin = 0;
volatile uint8_t tics_end = 0;
volatile uint16_t ms_begin = 0;
volatile uint16_t ms_end = 0;

void uartSend(uint8_t data)
{
    while((UCSR0A&(1<<UDRE0)) == 0);
    UDR0 = data;
}

int main (void)
{
    // enable INT0 on every state change
    EICRA |= (1 << ISC00);
    EICRA &= ~(1 << ISC01);
    EIMSK |= (1 << INT0);

    // ctc and prescaler 8
    TCCR0A |= (1 << WGM01);
    TCCR0B |= (1 << CS00);
    // every 10us
    OCR0A = 159;

    // enable compare interupt
    TIMSK0 |= (1 << OCIE0A);

    //
    UBRR0H = (UART_UBRR_VALUE >> 8);
    UBRR0L = UART_UBRR_VALUE;
    UCSR0B = (1 << RXEN0 | 1 << TXEN0);
    UCSR0C = (1 << UCSZ01 | 1 << UCSZ00);

    // set DDD2 as input
    DDRD &= ~(1 << DDD2);
    /* set pin 5 of PORTB for output*/
    DDRB |= _BV(DDB5);

    sei();
    while(1) {
        if (ms_end != 0)
        {
            uartSend((tics_end - tics_begin));
            uartSend(0xFF & (ms_end - ms_begin));
            uartSend('\n');
            tics_begin = 0;
            tics_end = 0;
            ms_begin = 0;
            ms_end = 0;
        }
    }
}

ISR(INT0_vect)
{
    uint8_t state = (PIND & (1 << PD2));
    uint8_t time = TCNT0;
}

ISR(TIMER0_COMPA_vect)
{
}
