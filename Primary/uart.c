#include "uart.h"
#include <avr/io.h>
#include <stdlib.h>

void uart_init(uint32_t baud)
{
    uint16_t ubrr = (uint16_t)((F_CPU / 16UL / baud) - 1);
    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)ubrr;
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); /* 8N1 */
}

void uart_putchar(char c)
{
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = (uint8_t)c;
}

void uart_print(const char *str)
{
    while (*str) uart_putchar(*str++);
}

void uart_println(const char *str)
{
    uart_print(str);
    uart_print("\r\n");
}

void uart_print_double(double val, uint8_t decimals)
{
    char buf[32];
    dtostrf(val, 0, decimals, buf);
    uart_print(buf);
}

void uart_println_double(double val, uint8_t decimals)
{
    uart_print_double(val, decimals);
    uart_print("\r\n");
}

uint8_t uart_available(void)
{
    return (UCSR0A & (1 << RXC0)) ? 1 : 0;
}

char uart_getchar(void)
{
    while (!(UCSR0A & (1 << RXC0)));
    return (char)UDR0;
}

uint16_t uart_read_line(char *buf, uint16_t len)
{
    uint16_t i = 0;
    while (i < len - 1)
    {
        char c = uart_getchar();
        if (c == '\n' || c == '\r') break;
        buf[i++] = c;
    }
    buf[i] = '\0';
    return i;
}
