#include "UART.h"

#include <avr/io.h>
#include <avr/interrupt.h>

volatile uart_t uart;

void initUART(void)
{
    // set baud to UART_BAUD and use 8N1 tx and rx
    UBRR0H = (UART_UBRR_VALUE >> 8);
    UBRR0L = UART_UBRR_VALUE;
    UCSR0B = (1 << RXEN0 | 1 << TXEN0 | 1 << RXCIE0);
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

ISR(USART_RX_vect)
{
    unsigned char character;

    character = UDR0;
    if (!uart.ready)
    {
        if (character != '\r' && character != '\n' && uart.count < UART_MAX_LENGTH)
        {
            uart.buffer[uart.count++] = character;
        }
        else
        {
            uart.buffer[uart.count] = '\0';
            uart.count = 0;
            uart.ready = 1;
        }
    }
}
