#ifndef UART_H
#define UART_H

#include <stdint.h>

void uart_init(uint32_t baud);
void uart_putchar(char c);
void uart_print(const char *str);
void uart_println(const char *str);
void uart_print_double(double val, uint8_t decimals);
void uart_println_double(double val, uint8_t decimals);
uint8_t uart_available(void);
char uart_getchar(void);

/* Reads a line terminated by '\n' or '\r' into buf (max len-1 chars + NUL).
 * Blocking: waits for at least one character, then drains until the
 * terminator or the buffer fills. Returns number of characters stored. */
uint16_t uart_read_line(char *buf, uint16_t len);

#endif
