#ifndef UART_H
#define UART_H

#include <stdint.h>

#define UART_BAUD 9600UL
#define UART_UBRR_VALUE ((F_CPU+UART_BAUD*8)/(UART_BAUD*16)-1)
#define UART_MAX_LENGTH 10

void initUART(void);
void sendCharUART(char data);
void sendStringUART(char *data);
void sendIntUART(uint8_t data);

typedef struct
{
    uint8_t ready;
    uint8_t count;
    unsigned char buffer[UART_MAX_LENGTH + 1];
} uart_t;

extern volatile uart_t uart;

#endif /* UART_H */
