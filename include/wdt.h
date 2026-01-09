
#ifndef WDT_H
#define WDT_H

#include <stdint.h>


typedef enum {
    WDT_16MS   = 0,
    WDT_32MS   = 1,
    WDT_64MS   = 2,
    WDT_125MS  = 3,
    WDT_250MS  = 4,
    WDT_500MS  = 5,
    WDT_1S     = 6,
    WDT_2S     = 7,
    WDT_4S     = 8,
    WDT_8S     = 9
} wdt_timeout_t;


typedef enum {
    RESET_POWERON   = 0x01,
    RESET_EXTERNAL  = 0x02,
    RESET_BROWNOUT  = 0x04,
    RESET_WATCHDOG  = 0x08
} reset_reason_t;


uint8_t wdt_get_reset_reason(void);

void wdt_clear_reset_reason(void);

void wdt_init(wdt_timeout_t timeout);
// ...existing code...

/**
 * @brief Disable watchdog timer
 * @note Must follow timed sequence per datasheet
 */
void wdt_disable(void);

/**
 * @brief Reset (kick) the watchdog timer
 * @note Must be called periodically to prevent system reset
 */
void wdt_kick(void);

/**
 * @brief Force immediate system reset via watchdog
 */
void wdt_force_reset(void);

/**
 * @brief Check if last reset was caused by watchdog
 * @return 1 if watchdog reset, 0 otherwise
 */
uint8_t wdt_was_reset(void);

#endif /* WDT_H */
