
#ifndef STATS_H
#define STATS_H

#include <stdint.h>


typedef struct {
    uint16_t crash_count;       /* Total crash count (persisted in EEPROM) */
    uint32_t total_uptime_ms;   /* Cumulative uptime (persisted in EEPROM) */
    uint32_t session_start;     /* Current session start tick */
} system_stats_t;


void stats_init(void);

void stats_record_crash(void);

void stats_update_uptime(void);

uint16_t stats_get_crash_count(void);

uint32_t stats_get_total_uptime(void);

uint32_t stats_get_session_uptime(void);

uint8_t stats_get_availability(void);
void stats_reset(void);
void stats_session_start(void);
uint8_t stats_get_availability(void);
void stats_reset(void);
void stats_session_start(void);

#endif /* STATS_H */
