#ifndef UART_H
#define UART_H

#include <stdint.h>

#define UART_BAUD 9600UL
#define UART_UBRR_VALUE ((F_CPU+UART_BAUD*8)/(UART_BAUD*16)-1)

void initUART(void);
void sendCharUART(char data);
void sendStringUART(char *data);
void sendIntUART(uint8_t data);

#endif /* UART_H */
