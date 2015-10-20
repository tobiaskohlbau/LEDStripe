#include "UART.h"

#include <avr/io.h>

void initUART(void)
{
    // set baud to UART_BAUD and use 8N1 tx and rx
    UBRR0H = (UART_UBRR_VALUE >> 8);
    UBRR0L = UART_UBRR_VALUE;
    UCSR0B = (1 << RXEN0 | 1 << TXEN0);
    UCSR0C = (1 << UCSZ01 | 1 << UCSZ00);
}

void sendCharUART(char data)
{
    while(!(UCSR0A & (1 << UDRE0)));
    UDR0 = data;
}

void sendStringUART(char *data)
{
    while (*data)
    {
        sendCharUART(*data);
        data++;
    }
}

void sendIntUART(uint8_t data)
{
    sendCharUART((data / 100) + '0');
    sendCharUART(((data / 10) % 10) + '0');
    sendCharUART((data % 10) + '0');
}
