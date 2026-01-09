
#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>


void systick_init(void);

uint32_t systick_get_ms(void);

uint8_t systick_elapsed(uint32_t *last_tick, uint32_t interval_ms);

void delay_ms(uint16_t ms);


void fault_timer_init(uint8_t interval_sec);

void fault_timer_enable(void);

void fault_timer_disable(void);
// ...existing code...

/**
 * @brief Get fault injection count
 * @return Number of fault injections since boot
 */
uint16_t fault_get_count(void);

/**
 * @brief Check if fault was recently injected (clears flag)
 * @return 1 if fault occurred, 0 otherwise
 */
uint8_t fault_check_flag(void);

#endif /* TIMER_H */
