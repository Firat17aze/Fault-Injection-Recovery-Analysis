
#include "uart.h"
#include "atmega328p.h"
#include "config.h"
#include <avr/pgmspace.h>


/* Conversion buffer for number printing */
static char uart_conv_buf[12];


void uart_init(uint32_t baud_rate)
{
    uint16_t ubrr_value;
    
    /* Calculate UBRR value: UBRR = (F_CPU / (16 * BAUD)) - 1 */
    ubrr_value = (uint16_t)((F_CPU / (16UL * baud_rate)) - 1);
    
    /* Set baud rate registers */
    REG_UBRR0H = HIGH_BYTE(ubrr_value);
    REG_UBRR0L = LOW_BYTE(ubrr_value);
    
    /* Enable transmitter and receiver */
    REG_UCSR0B = BIT(UCSR0B_TXEN0) | BIT(UCSR0B_RXEN0);
    
    /* Frame format: 8 data bits, 1 stop bit, no parity */
    REG_UCSR0C = BIT(UCSR0C_UCSZ01) | BIT(UCSR0C_UCSZ00);
}

void uart_putc(char c)
{
    /* Wait for transmit buffer to be empty */
    while (!BIT_GET(REG_UCSR0A, UCSR0A_UDRE0)) {
        /* Spin */
    }
    
    /* Write data to transmit buffer */
    REG_UDR0 = c;
}

void uart_puts(const char *str)
{
    while (*str) {
        uart_putc(*str++);
    }
}

void uart_puts_P(const char *str)
// ...existing code...
{
    char c;
    while ((c = pgm_read_byte(str++)) != '\0') {
        uart_putc(c);
    }
}

void uart_put_u8(uint8_t num)
{
    uart_put_u32((uint32_t)num);
}

void uart_put_u16(uint16_t num)
{
    uart_put_u32((uint32_t)num);
}

void uart_put_u32(uint32_t num)
{
    char *p = uart_conv_buf + sizeof(uart_conv_buf) - 1;
    *p = '\0';
    
    if (num == 0) {
        uart_putc('0');
        return;
    }
    
    while (num > 0) {
        *--p = '0' + (num % 10);
        num /= 10;
    }
    
    uart_puts(p);
}

void uart_put_i32(int32_t num)
{
    if (num < 0) {
        uart_putc('-');
        num = -num;
    }
    uart_put_u32((uint32_t)num);
}

void uart_put_hex32(uint32_t num)
{
    static const char hex_chars[] = "0123456789ABCDEF";
    char buf[9];
    uint8_t i;
    
    for (i = 0; i < 8; i++) {
        buf[7 - i] = hex_chars[num & 0x0F];
        num >>= 4;
    }
    buf[8] = '\0';
    
    uart_puts("0x");
    uart_puts(buf);
}

void uart_newline(void)
{
    uart_putc('\r');
    uart_putc('\n');
}

uint8_t uart_available(void)
{
    return BIT_GET(REG_UCSR0A, UCSR0A_RXC0);
}

char uart_getc(void)
{
    /* Wait for data */
    while (!uart_available()) {
        /* Spin */
    }
    
    return REG_UDR0;
}
