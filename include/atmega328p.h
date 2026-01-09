#define REG_EEARH       MMIO8(0x41)
#define REG_EEARL       MMIO8(0x40)
#define REG_EEDR        MMIO8(0x3C)
#define EECR_EEPE       1
#define EECR_EERE       0
#define EECR_EEMPE      2

#ifndef ATMEGA328P_H
#define ATMEGA328P_H

#include <stdint.h>


#define MMIO8(addr)     (*(volatile uint8_t *)(addr))
#define MMIO16(addr)    (*(volatile uint16_t *)(addr))


#define REG_SREG        MMIO8(0x5F)
#define SREG_I          7       



#define REG_MCUSR       MMIO8(0x54)
#define MCUSR_PORF      0       
#define MCUSR_EXTRF     1       
#define MCUSR_BORF      2       
#define MCUSR_WDRF      3       


#define REG_WDTCSR      MMIO8(0x60)
#define WDTCSR_WDP0     0       
#define WDTCSR_WDP1     1       
#define WDTCSR_WDP2     2       
#define WDTCSR_WDE      3       
#define WDTCSR_WDCE     4      
#define WDTCSR_WDP3     5       
#define WDTCSR_WDIE     6       
#define WDTCSR_WDIF     7       

#define WDT_TIMEOUT_16MS    0x00
#define WDT_TIMEOUT_32MS    0x01
#define WDT_TIMEOUT_64MS    0x02
#define WDT_TIMEOUT_125MS   0x03
#define WDT_TIMEOUT_250MS   0x04
#define WDT_TIMEOUT_500MS   0x05
#define WDT_TIMEOUT_1S      0x06
#define WDT_TIMEOUT_2S      0x07
#define WDT_TIMEOUT_4S      0x08
#define WDT_TIMEOUT_8S      0x09



#define REG_UDR0        MMIO8(0xC6)     /* USART Data Register */
#define REG_UCSR0A      MMIO8(0xC0)     /* USART Control and Status A */
#define REG_UCSR0B      MMIO8(0xC1)     /* USART Control and Status B */
#define REG_UCSR0C      MMIO8(0xC2)     /* USART Control and Status C */
#define REG_UBRR0L      MMIO8(0xC4)     /* USART Baud Rate Low */
#define REG_UBRR0H      MMIO8(0xC5)     /* USART Baud Rate High */


#define UCSR0A_RXC0     7       
#define UCSR0A_TXC0     6       
#define UCSR0A_UDRE0    5       
#define UCSR0A_FE0      4       
#define UCSR0A_DOR0     3       
#define UCSR0A_UPE0     2      
#define UCSR0A_U2X0     1       

#define UCSR0B_RXCIE0   7       /* RX Complete Interrupt Enable */
#define UCSR0B_TXCIE0   6       /* TX Complete Interrupt Enable */
#define UCSR0B_UDRIE0   5       /* Data Register Empty Interrupt Enable */
#define UCSR0B_RXEN0    4       /* Receiver Enable */
#define UCSR0B_TXEN0    3       /* Transmitter Enable */

#define UCSR0C_UCSZ01   2       /* Character Size bit 1 */
#define UCSR0C_UCSZ00   1       /* Character Size bit 0 */


#define REG_TCCR0A      MMIO8(0x44)     /* Timer/Counter Control A */
#define REG_TCCR0B      MMIO8(0x45)     /* Timer/Counter Control B */
#define REG_TCNT0       MMIO8(0x46)     /* Timer/Counter Value */
#define REG_OCR0A       MMIO8(0x47)     /* Output Compare A */
#define REG_OCR0B       MMIO8(0x48)     /* Output Compare B */
// ...existing code...
#define REG_TIMSK0      MMIO8(0x6E)     /* Timer Interrupt Mask */
#define REG_TIFR0       MMIO8(0x35)     /* Timer Interrupt Flag */

/* TCCR0A bits */
#define TCCR0A_WGM00    0       /* Waveform Generation Mode bit 0 */
#define TCCR0A_WGM01    1       /* Waveform Generation Mode bit 1 */

/* TCCR0B bits */
#define TCCR0B_CS00     0       /* Clock Select bit 0 */
#define TCCR0B_CS01     1       /* Clock Select bit 1 */
#define TCCR0B_CS02     2       /* Clock Select bit 2 */
#define TCCR0B_WGM02    3       /* Waveform Generation Mode bit 2 */

/* TIMSK0 bits */
#define TIMSK0_TOIE0    0       /* Overflow Interrupt Enable */
#define TIMSK0_OCIE0A   1       /* Compare Match A Interrupt Enable */
#define TIMSK0_OCIE0B   2       /* Compare Match B Interrupt Enable */



#define REG_TCCR1A      MMIO8(0x80)     /* Timer/Counter1 Control A */
#define REG_TCCR1B      MMIO8(0x81)     /* Timer/Counter1 Control B */
#define REG_TCCR1C      MMIO8(0x82)     /* Timer/Counter1 Control C */
#define REG_TCNT1       MMIO16(0x84)    /* Timer/Counter1 Value */
#define REG_TCNT1L      MMIO8(0x84)     /* Timer/Counter1 Value Low */
#define REG_TCNT1H      MMIO8(0x85)     /* Timer/Counter1 Value High */
#define REG_ICR1        MMIO16(0x86)    /* Input Capture Register */
#define REG_OCR1A       MMIO16(0x88)    /* Output Compare A */
#define REG_OCR1AL      MMIO8(0x88)
#define REG_OCR1AH      MMIO8(0x89)
#define REG_OCR1B       MMIO16(0x8A)    /* Output Compare B */
#define REG_TIMSK1      MMIO8(0x6F)     /* Timer1 Interrupt Mask */
#define REG_TIFR1       MMIO8(0x36)     /* Timer1 Interrupt Flag */

/* TCCR1B bits */
#define TCCR1B_CS10     0       /* Clock Select bit 0 */
#define TCCR1B_CS11     1       /* Clock Select bit 1 */
#define TCCR1B_CS12     2       /* Clock Select bit 2 */
#define TCCR1B_WGM12    3       /* Waveform Generation Mode bit 2 (CTC) */
#define TCCR1B_WGM13    4       /* Waveform Generation Mode bit 3 */

/* TIMSK1 bits */
#define TIMSK1_TOIE1    0       /* Overflow Interrupt Enable */
#define TIMSK1_OCIE1A   1       /* Compare Match A Interrupt Enable */
#define TIMSK1_OCIE1B   2       /* Compare Match B Interrupt Enable */



#define REG_EECR        MMIO8(0x3F)     /* EEPROM Control Register */
#define REG_UDR0        MMIO8(0xC6)
#define REG_UCSR0A      MMIO8(0xC0)
#define REG_UCSR0B      MMIO8(0xC1)
#define REG_UCSR0C      MMIO8(0xC2)
#define REG_UBRR0L      MMIO8(0xC4)
#define REG_UBRR0H      MMIO8(0xC5)
#define UCSR0B_RXCIE0   7
#define UCSR0B_TXCIE0   6
#define UCSR0B_UDRIE0   5
#define UCSR0B_RXEN0    4
#define UCSR0B_TXEN0    3
#define UCSR0C_UCSZ01   2
#define UCSR0C_UCSZ00   1
#define REG_TCCR0A      MMIO8(0x44)
#define REG_TCCR0B      MMIO8(0x45)
#define REG_TCNT0       MMIO8(0x46)
#define REG_OCR0A       MMIO8(0x47)
#define REG_OCR0B       MMIO8(0x48)
#define REG_TIMSK0      MMIO8(0x6E)
#define REG_TIFR0       MMIO8(0x35)
#define TCCR0A_WGM00    0
#define TCCR0A_WGM01    1
#define TCCR0B_CS00     0
#define TCCR0B_CS01     1
#define TCCR0B_CS02     2
#define TCCR0B_WGM02    3
#define TIMSK0_TOIE0    0
#define TIMSK0_OCIE0A   1
#define TIMSK0_OCIE0B   2
#define REG_TCCR1A      MMIO8(0x80)
#define REG_TCCR1B      MMIO8(0x81)



#define BIT(n)                  (1U << (n))
#define BIT_SET(reg, bit)       ((reg) |= BIT(bit))
#define BIT_CLR(reg, bit)       ((reg) &= ~BIT(bit))
#define BIT_TGL(reg, bit)       ((reg) ^= BIT(bit))
#define BIT_GET(reg, bit)       (((reg) >> (bit)) & 1U)

#define LOW_BYTE(x)             ((uint8_t)((x) & 0xFF))
#define HIGH_BYTE(x)            ((uint8_t)(((x) >> 8) & 0xFF))



#define INTERRUPTS_ENABLE()     __asm__ __volatile__ ("sei" ::: "memory")
#define INTERRUPTS_DISABLE()    __asm__ __volatile__ ("cli" ::: "memory")

#define CRITICAL_SECTION_BEGIN  uint8_t _sreg_save = REG_SREG; INTERRUPTS_DISABLE()
#define CRITICAL_SECTION_END    REG_SREG = _sreg_save


#define NOP()                   __asm__ __volatile__ ("nop")


#define WDT_RESET()             __asm__ __volatile__ ("wdr")

#endif 
