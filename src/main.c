#include "config.h"
#include "atmega328p.h"
#include "uart.h"
#include "timer.h"
#include "wdt.h"
#include "eeprom_drv.h"
#include "fault_inject.h"
#include "stats.h"
#include <avr/pgmspace.h>

static volatile uint32_t g_critical_counter = 0;
static uint32_t g_last_valid_counter = 0;
static uint32_t g_heartbeat_tick = 0;
static uint32_t g_summary_tick = 0;

static const char str_banner1[] PROGMEM = "============================================================";
static const char str_banner2[] PROGMEM = "    FIRA - Fault Injection & Recovery Analysis";
static const char str_banner3[] PROGMEM = "    Running on ATmega328P (Pure C, No Libraries)";
static const char str_divider[] PROGMEM = "------------------------------------------------------------";

static const char str_boot[] PROGMEM = "Hey! I just woke up because of: ";
static const char str_wdt_reset[] PROGMEM = "The WATCHDOG saved me!";
static const char str_por[] PROGMEM = "Fresh power-on (first boot)";
static const char str_ext[] PROGMEM = "Someone pressed the reset button";
static const char str_bor[] PROGMEM = "Low voltage detected (brown-out)";
static const char str_unknown[] PROGMEM = "Not sure... something happened";

static const char str_reboot_art1[] PROGMEM = "  ____  _____ ____   ___   ___ _____ ";
static const char str_reboot_art2[] PROGMEM = " |  _ \\| ____| __ ) / _ \\ / _ \\_   _|";
static const char str_reboot_art3[] PROGMEM = " | |_) |  _| |  _ \\| | | | | | || |  ";
static const char str_reboot_art4[] PROGMEM = " |  _ <| |___| |_) | |_| | |_| || |  ";
static const char str_reboot_art5[] PROGMEM = " |_| \\_\\_____|____/ \\___/ \\___/ |_|  ";
static const char str_reboot_msg[] PROGMEM = "I crashed but the Watchdog brought me back to life!";

static const char str_crash_box1[] PROGMEM = "+------------------------------------------+";
static const char str_crash_msg[] PROGMEM = "| Oops! I crashed. Total crashes so far: ";

static const char str_eeprom_crash[] PROGMEM = "Times I've crashed: ";
static const char str_eeprom_uptime[] PROGMEM = "Total time running: ";

static const char str_config[] PROGMEM = "Attack mode: ";
static const char str_mode_a[] PROGMEM = "A - Flipping random bits (data corruption)";
static const char str_mode_b[] PROGMEM = "B - Resetting program counter (code jump)";
static const char str_mode_c[] PROGMEM = "C - Infinite loop (testing watchdog rescue)";
static const char str_mode_safe[] PROGMEM = "Safe mode (no attacks)";
static const char str_heartbeat_cfg[] PROGMEM = "Heartbeat every: ";
static const char str_fault_cfg[] PROGMEM = "Injecting faults every: ";
static const char str_wdt_cfg[] PROGMEM = "Watchdog timeout: 2s";

static const char str_init_systick[] PROGMEM = "Starting my internal clock...";
static const char str_init_fault[] PROGMEM = "Arming the fault injector (the saboteur)...";
static const char str_init_wdt[] PROGMEM = "Enabling the watchdog (my guardian angel)...";
static const char str_entering[] PROGMEM = "Alright, here we go! Starting the main loop...";

static const char str_running[] PROGMEM = "Counter: ";
static const char str_bitflip[] PROGMEM = " << CORRUPTION DETECTED! Jumped by ";
static const char str_uptime[] PROGMEM = " | Running: ";
static const char str_faults[] PROGMEM = "s | Attacks: ";
static const char str_close[] PROGMEM = " |";
static const char str_ms[] PROGMEM = "ms";
static const char str_sec[] PROGMEM = "s";

static const char str_research[] PROGMEM = "+-------- STATUS REPORT --------+";
static const char str_session[] PROGMEM = "| This session: ";
static const char str_counter[] PROGMEM = "| Counter value: ";
static const char str_fault_cnt[] PROGMEM = "| Faults injected: ";
static const char str_crash_cnt[] PROGMEM = "| Total crashes: ";
static const char str_avail[] PROGMEM = "| System uptime: ";
static const char str_percent[] PROGMEM = "%";
static const char str_box_end[] PROGMEM = "+-------------------------------+";

static void print_banner(void) {
    uart_newline();
    uart_puts_P(str_banner1); uart_newline();
    uart_puts_P(str_banner2); uart_newline();
    uart_puts_P(str_banner3); uart_newline();
    uart_puts_P(str_banner1); uart_newline();
    uart_newline();
}

static void print_reset_reason(void) {
    uint8_t reason = wdt_get_reset_reason();
    uart_puts_P(str_boot);
    
    if (reason & RESET_WATCHDOG) {
        uart_puts_P(str_wdt_reset);
        uart_newline();
        uart_newline();
        uart_puts_P(str_reboot_art1); uart_newline();
        uart_puts_P(str_reboot_art2); uart_newline();
        uart_puts_P(str_reboot_art3); uart_newline();
        uart_puts_P(str_reboot_art4); uart_newline();
        uart_puts_P(str_reboot_art5); uart_newline();
        uart_newline();
        uart_puts_P(str_reboot_msg);
        uart_newline();
        uart_newline();
    } else if (reason & RESET_BROWNOUT) {
        uart_puts_P(str_bor);
        uart_newline();
    } else if (reason & RESET_EXTERNAL) {
        uart_puts_P(str_ext);
        uart_newline();
    } else if (reason & RESET_POWERON) {
        uart_puts_P(str_por);
        uart_newline();
    } else {
        uart_puts_P(str_unknown);
        uart_newline();
    }
}

static void print_crash_notification(void) {
    if (wdt_was_reset()) {
        uart_puts_P(str_crash_box1); uart_newline();
        uart_puts_P(str_crash_msg);
        uart_put_u16(stats_get_crash_count());
        uart_newline();
        uart_puts_P(str_crash_box1); uart_newline();
        uart_newline();
    }
}

static void print_eeprom_stats(void) {
    uart_puts_P(str_eeprom_crash);
    uart_put_u16(stats_get_crash_count());
    uart_newline();
    
    uart_puts_P(str_eeprom_uptime);
    uart_put_u32(stats_get_total_uptime() / 1000);
    uart_puts_P(str_sec);
    uart_newline();
}

static void print_config(void) {
    uart_puts_P(str_config);
#if defined(ATTACK_MODE_A)
    uart_puts_P(str_mode_a);
#elif defined(ATTACK_MODE_B)
    uart_puts_P(str_mode_b);
#elif defined(ATTACK_MODE_C)
    uart_puts_P(str_mode_c);
#else
    uart_puts_P(str_mode_safe);
#endif
    uart_newline();
    
    uart_puts_P(str_heartbeat_cfg);
    uart_put_u16(HEARTBEAT_INTERVAL_MS);
    uart_puts_P(str_ms);
    uart_newline();
    
    uart_puts_P(str_fault_cfg);
    uart_put_u16(FAULT_INJECT_INTERVAL_SEC);
    uart_puts_P(str_sec);
    uart_newline();
    
    uart_puts_P(str_wdt_cfg);
    uart_newline();
    uart_newline();
}

static void heartbeat(void) {
    if (!systick_elapsed(&g_heartbeat_tick, HEARTBEAT_INTERVAL_MS)) {
        return;
    }
    
    g_critical_counter++;
    
    uart_puts_P(str_running);
    uart_put_u32(g_critical_counter);
    
    if (fault_check_flag()) {
        int32_t delta = (int32_t)g_critical_counter - (int32_t)g_last_valid_counter - 1;
        if (delta > 1 || delta < -1) {
            uart_puts_P(str_bitflip);
            uart_put_i32(delta);
            uart_puts_P(PSTR(" ***"));
        }
    }
    
    uart_puts_P(str_uptime);
    uart_put_u32(stats_get_session_uptime() / 1000);
    uart_puts_P(str_faults);
    uart_put_u16(fault_get_count());
    uart_puts_P(str_close);
    uart_newline();
    
    g_last_valid_counter = g_critical_counter;
}

#if ENABLE_RESEARCH_SUMMARY
static void research_summary(void) {
    if (!systick_elapsed(&g_summary_tick, RESEARCH_SUMMARY_INTERVAL)) {
        return;
    }
    
    uart_newline();
    uart_puts_P(str_research); uart_newline();
    
    uart_puts_P(str_session);
    uart_put_u32(stats_get_session_uptime() / 1000);
    uart_puts_P(str_sec);
    uart_newline();
    
    uart_puts_P(str_counter);
    uart_put_u32(g_critical_counter);
    uart_newline();
    
    uart_puts_P(str_fault_cnt);
    uart_put_u16(fault_get_count());
    uart_newline();
    
    uart_puts_P(str_crash_cnt);
    uart_put_u16(stats_get_crash_count());
    uart_newline();
    
    uart_puts_P(str_avail);
    uart_put_u8(stats_get_availability());
    uart_puts_P(str_percent);
    uart_newline();
    
    uart_puts_P(str_box_end);
    uart_newline();
    uart_newline();
}
#endif

static void system_init(void) {
    uart_init(UART_BAUD_RATE);
    
    print_banner();
    print_reset_reason();
    
    stats_init();
    
    if (wdt_was_reset()) {
        stats_record_crash();
    }
    
    print_crash_notification();
    print_eeprom_stats();
    print_config();
    
    uart_puts_P(str_init_systick);
    uart_newline();
    systick_init();
    
    uart_puts_P(str_init_fault);
    uart_newline();
    fault_timer_init(FAULT_INJECT_INTERVAL_SEC);
    
    fault_set_victim_ptr(&g_critical_counter);
    
    uart_puts_P(str_init_wdt);
    uart_newline();
    wdt_init(WDT_2S);
    
    INTERRUPTS_ENABLE();
    
    stats_session_start();
    g_heartbeat_tick = systick_get_ms();
    g_summary_tick = systick_get_ms();
    
    uart_newline();
    uart_puts_P(str_entering);
    uart_newline();
    uart_puts_P(str_banner1);
    uart_newline();
    uart_newline();
}

int main(void) {
    system_init();
    
    for (;;) {
        heartbeat();
        
#if ENABLE_RESEARCH_SUMMARY
        research_summary();
#endif
        
        wdt_kick();
    }
    
    return 0;
}
