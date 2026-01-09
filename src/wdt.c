
#include "wdt.h"
#include "atmega328p.h"


/* Store reset reason early before MCUSR is cleared */
static uint8_t g_reset_reason = 0;


void wdt_early_init(void) __attribute__((naked, used, section(".init3")));
void wdt_early_init(void)
{
    /* Save reset reason */
    g_reset_reason = REG_MCUSR;
    
    /* Clear all reset flags */
    REG_MCUSR = 0;
    
    /*
     * Disable Watchdog Timer
     * 
     * Per ATmega328P datasheet, to disable WDT:
     * 1. Write 1 to WDCE and WDE simultaneously
     * 2. Within 4 clock cycles, write 0 to WDE
     */
    REG_WDTCSR = BIT(WDTCSR_WDCE) | BIT(WDTCSR_WDE);
    REG_WDTCSR = 0x00;
}


uint8_t wdt_get_reset_reason(void)
{
    return g_reset_reason;
}

void wdt_clear_reset_reason(void)
{
    REG_MCUSR = 0;
}
// ...existing code...

void wdt_init(wdt_timeout_t timeout)
{
    uint8_t wdt_value;
    
    /*
     * Build WDT configuration byte
     * 
     * WDP3:0 bits are split:
     *   WDP3 is bit 5 in WDTCSR
     *   WDP2:0 are bits 2:0 in WDTCSR
     *   WDE is bit 3 (enable)
     */
    
    /* Handle timeout value > 7 (needs WDP3) */
    if (timeout > 7) {
        wdt_value = BIT(WDTCSR_WDP3) | BIT(WDTCSR_WDE) | (timeout & 0x07);
    } else {
        wdt_value = BIT(WDTCSR_WDE) | (timeout & 0x07);
    }
    
    /* Disable interrupts during timed sequence */
    INTERRUPTS_DISABLE();
    
    /* Reset watchdog timer */
    WDT_RESET();
    
    /*
     * Enable WDT with timed sequence:
     * 1. Set WDCE and WDE
     * 2. Within 4 clock cycles, write new value with WDE set
     */
    REG_WDTCSR = BIT(WDTCSR_WDCE) | BIT(WDTCSR_WDE);
    REG_WDTCSR = wdt_value;
    
    INTERRUPTS_ENABLE();
}

void wdt_disable(void)
{
    INTERRUPTS_DISABLE();
    
    /* Reset watchdog first */
    WDT_RESET();
    
    /* Clear WDRF in MCUSR */
    BIT_CLR(REG_MCUSR, MCUSR_WDRF);
    
    /*
     * Timed sequence to disable:
     * 1. Set WDCE and WDE
     * 2. Within 4 cycles, clear WDE
     */
    REG_WDTCSR = BIT(WDTCSR_WDCE) | BIT(WDTCSR_WDE);
    REG_WDTCSR = 0x00;
    
    INTERRUPTS_ENABLE();
}

void wdt_kick(void)
{
    WDT_RESET();
}

void wdt_force_reset(void)
{
    /* Enable WDT with shortest timeout */
    wdt_init(WDT_16MS);
    
    /* Enter infinite loop - WDT will reset us */
    while (1) {
        /* Wait for reset */
    }
}

uint8_t wdt_was_reset(void)
{
    return (g_reset_reason & BIT(MCUSR_WDRF)) ? 1 : 0;
}
