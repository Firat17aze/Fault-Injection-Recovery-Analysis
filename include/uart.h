
#ifndef UART_H
#define UART_H

#include <stdint.h>


void uart_init(uint32_t baud_rate);

void uart_putc(char c);

void uart_puts(const char *str);

void uart_puts_P(const char *str);

void uart_put_u8(uint8_t num);

void uart_put_u16(uint16_t num);

void uart_put_u32(uint32_t num);

void uart_put_i32(int32_t num);
void uart_put_hex32(uint32_t num);
void uart_newline(void);
uint8_t uart_available(void);
char uart_getc(void);
#define UART_PRINT(s)       uart_puts_P(PSTR(s))
#define UART_PRINTLN(s)     do { uart_puts_P(PSTR(s)); uart_newline(); } while(0)
void uart_put_i32(int32_t num);
void uart_put_hex32(uint32_t num);
void uart_newline(void);
uint8_t uart_available(void);
char uart_getc(void);
#define UART_PRINT(s)       uart_puts_P(PSTR(s))
#define UART_PRINTLN(s)     do { uart_puts_P(PSTR(s)); uart_newline(); } while(0)

#endif /* UART_H */
