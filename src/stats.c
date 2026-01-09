
#include "stats.h"
#include "eeprom_drv.h"
#include "timer.h"
#include "config.h"


static system_stats_t g_stats;


void stats_init(void)
{
    uint16_t magic;
    
    /* Check for magic number (first boot detection) */
    magic = eeprom_read_word(EEPROM_ADDR_MAGIC);
    
    if (magic != EEPROM_MAGIC_VALUE) {
        /* First boot - initialize EEPROM */
        g_stats.crash_count = 0;
        g_stats.total_uptime_ms = 0;
        
        eeprom_write_word(EEPROM_ADDR_CRASH_COUNT, 0);
        eeprom_write_dword(EEPROM_ADDR_TOTAL_UPTIME, 0);
        eeprom_write_word(EEPROM_ADDR_MAGIC, EEPROM_MAGIC_VALUE);
    } else {
        /* Load existing statistics */
        g_stats.crash_count = eeprom_read_word(EEPROM_ADDR_CRASH_COUNT);
        g_stats.total_uptime_ms = eeprom_read_dword(EEPROM_ADDR_TOTAL_UPTIME);
    }
    
    g_stats.session_start = 0;
}

void stats_record_crash(void)
{
    g_stats.crash_count++;
    eeprom_update_word(EEPROM_ADDR_CRASH_COUNT, g_stats.crash_count);
}

void stats_update_uptime(void)
{
    uint32_t session_uptime = systick_get_ms() - g_stats.session_start;
    g_stats.total_uptime_ms += session_uptime;
    eeprom_update_dword(EEPROM_ADDR_TOTAL_UPTIME, g_stats.total_uptime_ms);
}

uint16_t stats_get_crash_count(void)
// ...existing code...
{
    return g_stats.crash_count;
}

uint32_t stats_get_total_uptime(void)
{
    return g_stats.total_uptime_ms;
}

uint32_t stats_get_session_uptime(void)
{
    return systick_get_ms() - g_stats.session_start;
}

uint8_t stats_get_availability(void)
{
    uint32_t total_uptime;
    uint32_t total_downtime;
    uint32_t total_time;
    
    total_uptime = g_stats.total_uptime_ms + stats_get_session_uptime();
    
    if (total_uptime == 0 || g_stats.crash_count == 0) {
        return 100;
    }
    
    /* Assume 2 second recovery per crash (WDT timeout) */
    total_downtime = (uint32_t)g_stats.crash_count * 2000UL;
    total_time = total_uptime + total_downtime;
    
    /* Calculate percentage */
    return (uint8_t)((total_uptime * 100UL) / total_time);
}

void stats_reset(void)
{
    g_stats.crash_count = 0;
    g_stats.total_uptime_ms = 0;
    
    eeprom_write_word(EEPROM_ADDR_CRASH_COUNT, 0);
    eeprom_write_dword(EEPROM_ADDR_TOTAL_UPTIME, 0);
}

void stats_session_start(void)
{
    g_stats.session_start = systick_get_ms();
}
