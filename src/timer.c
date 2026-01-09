
#include "timer.h"
#include "atmega328p.h"
#include "config.h"
#include <avr/interrupt.h>


/* System tick counter (1ms resolution) */
static volatile uint32_t g_systick_ms = 0;

/* Fault injection statistics */
static volatile uint16_t g_fault_count = 0;
static volatile uint8_t g_fault_flag = 0;


ISR(TIMER0_COMPA_vect)
{
    g_systick_ms++;
}


void systick_init(void)
{
    /* Reset Timer0 */
    REG_TCCR0A = 0;
    REG_TCCR0B = 0;
    REG_TCNT0 = 0;
    
    /*
     * CTC Mode (Clear Timer on Compare Match)
     * WGM01 = 1, WGM00 = 0, WGM02 = 0
     */
    BIT_SET(REG_TCCR0A, TCCR0A_WGM01);
    
    /*
     * Prescaler = 64
     * CS01 = 1, CS00 = 1
     * Timer frequency = 16MHz / 64 = 250kHz
     */
    BIT_SET(REG_TCCR0B, TCCR0B_CS01);
    BIT_SET(REG_TCCR0B, TCCR0B_CS00);
    
    /*
     * Compare value for 1ms interrupt
     * OCR0A = (F_CPU / Prescaler / 1000Hz) - 1
// ...existing code...
     * OCR0A = (16000000 / 64 / 1000) - 1 = 249
     */
    REG_OCR0A = 249;
    
    /* Enable Compare Match A interrupt */
    BIT_SET(REG_TIMSK0, TIMSK0_OCIE0A);
}

uint32_t systick_get_ms(void)
{
    uint32_t ticks;
    
    /* Atomic read of 32-bit value */
    CRITICAL_SECTION_BEGIN;
    ticks = g_systick_ms;
    CRITICAL_SECTION_END;
    
    return ticks;
}

uint8_t systick_elapsed(uint32_t *last_tick, uint32_t interval_ms)
{
    uint32_t current = systick_get_ms();
    
    if ((current - *last_tick) >= interval_ms) {
        *last_tick = current;
        return 1;
    }
    
    return 0;
}

void delay_ms(uint16_t ms)
{
    uint32_t start = systick_get_ms();
    while ((systick_get_ms() - start) < ms) {
        /* Spin */
    }
}

/* ============================================================================
 * TIMER1 - FAULT INJECTION FUNCTIONS
 * ============================================================================ */

void fault_timer_init(uint8_t interval_sec)
{
    /* Reset Timer1 */
    REG_TCCR1A = 0;
    REG_TCCR1B = 0;
    REG_TCNT1 = 0;
    
    /*
     * CTC Mode
     * WGM12 = 1 (CTC mode, TOP = OCR1A)
     */
    BIT_SET(REG_TCCR1B, TCCR1B_WGM12);
    
    /*
     * Prescaler = 1024
     * CS12 = 1, CS10 = 1
     * Timer frequency = 16MHz / 1024 = 15625 Hz
     */
    BIT_SET(REG_TCCR1B, TCCR1B_CS12);
    BIT_SET(REG_TCCR1B, TCCR1B_CS10);
    
    /*
     * Compare value for N second interval
     * OCR1A = (15625 * interval_sec) - 1
     */
    uint16_t compare_val = (uint16_t)(15625UL * interval_sec) - 1;
    REG_OCR1AH = HIGH_BYTE(compare_val);
    REG_OCR1AL = LOW_BYTE(compare_val);
    
    /* Enable Compare Match A interrupt */
    BIT_SET(REG_TIMSK1, TIMSK1_OCIE1A);
}

void fault_timer_enable(void)
{
    /* Re-enable clock by setting prescaler */
    BIT_SET(REG_TCCR1B, TCCR1B_CS12);
    BIT_SET(REG_TCCR1B, TCCR1B_CS10);
}

void fault_timer_disable(void)
{
    /* Stop clock by clearing prescaler bits */
    BIT_CLR(REG_TCCR1B, TCCR1B_CS12);
    BIT_CLR(REG_TCCR1B, TCCR1B_CS11);
    BIT_CLR(REG_TCCR1B, TCCR1B_CS10);
}

uint16_t fault_get_count(void)
{
    uint16_t count;
    
    CRITICAL_SECTION_BEGIN;
    count = g_fault_count;
    CRITICAL_SECTION_END;
    
    return count;
}

uint8_t fault_check_flag(void)
{
    uint8_t flag;
    
    CRITICAL_SECTION_BEGIN;
    flag = g_fault_flag;
    g_fault_flag = 0;
    CRITICAL_SECTION_END;
    
    return flag;
}

/* ============================================================================
 * TIMER1 - FAULT INJECTION ISR
 * ============================================================================ */

/* Forward declaration - implemented in fault_inject.c */
extern void fault_inject_execute(void);

ISR(TIMER1_COMPA_vect)
{
    g_fault_count++;
    g_fault_flag = 1;
    
    /* Execute fault injection attack */
    fault_inject_execute();
}
