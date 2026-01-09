
#include "fault_inject.h"
#include "timer.h"
#include "config.h"
#include "atmega328p.h"


/* Pointer to victim's critical data */
static volatile uint32_t *g_victim_ptr = (volatile uint32_t *)0;


volatile uint32_t* fault_get_victim_ptr(void)
{
    return g_victim_ptr;
}

void fault_set_victim_ptr(volatile uint32_t *ptr)
{
    g_victim_ptr = ptr;
}


static void attack_bitflip(void)
{
    if (g_victim_ptr == (volatile uint32_t *)0) {
        return;
    }
    
    uint16_t fault_num = fault_get_count();
    
    /* Select byte (0-3) and bit (0-7) based on fault count */
    uint8_t byte_offset = fault_num % 4;
    uint8_t bit_position = (fault_num * 7) % 8;
    
    /* Get pointer to specific byte within the 32-bit counter */
// ...existing code...
    volatile uint8_t *byte_ptr = (volatile uint8_t *)g_victim_ptr;
    
    /* Flip the bit using XOR */
    byte_ptr[byte_offset] ^= BIT(bit_position);
}

/**
 * @brief Attack Mode B: Sniper Attack (PC Reset)
 * 
 * Simulates program counter corruption caused by:
 *   - Stack corruption
 *   - Return address overwrite
 *   - Wild pointer dereference
 * 
 * Effect: Jumps to reset vector (0x0000), causing immediate reboot
 */
static void attack_pc_reset(void)
{
    /* Create function pointer to address 0x0000 (reset vector) */
    void (*reset_vector)(void) = (void (*)(void))0x0000;
    
    /* Jump to reset - this will reboot the system */
    reset_vector();
    
    /* Execution never reaches here */
}

/**
 * @brief Attack Mode C: Infinite Hang (Deadlock)
 * 
 * Simulates system lockup caused by:
 *   - Corrupted loop counter
 *   - Deadlocked mutex/semaphore
 *   - Hardware waiting on device that never responds
 *   - Stuck interrupt handler
 * 
 * Effect: System completely freezes. Only WDT can recover.
 * 
 * THIS IS THE PRIMARY WDT DEMONSTRATION!
 */
static void attack_hang(void)
{
    /*
     * THE DEADLY INFINITE LOOP
     * 
     * When this runs:
     *   1. Main loop stops executing
     *   2. Heartbeat stops printing
     *   3. wdt_kick() is never called
     *   4. After WDT timeout (2s), hardware forces reset
     *   5. System reboots and recovers!
     * 
     * Without WDT: System would be permanently frozen
     * With WDT: System automatically recovers
     */
    for (;;) {
        /* Trapped forever... unless WDT saves us */
        NOP();
    }
}

/* ============================================================================
 * FAULT EXECUTION (called from ISR)
 * ============================================================================ */

void fault_inject_execute(void)
{
#if defined(ATTACK_MODE_A)
    attack_bitflip();
    
#elif defined(ATTACK_MODE_B)
    attack_pc_reset();
    
#elif defined(ATTACK_MODE_C)
    attack_hang();
    
#else
    /* No attack - safe mode */
    (void)0;
#endif
}
