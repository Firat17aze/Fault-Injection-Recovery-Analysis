/*
 * ============================================================================
 * FIRA - Fault Injection & Recovery Analysis
 * Configuration Header
 * ============================================================================
 */

#ifndef CONFIG_H
#define CONFIG_H

/* ============================================================================
 * MCU CONFIGURATION
 * ============================================================================ */

#ifndef F_CPU
#define F_CPU               16000000UL      /* 16 MHz crystal */
#endif

/* ============================================================================
 * ATTACK MODE SELECTION (uncomment ONE)
 * ============================================================================ */

#define ATTACK_MODE_A       /* Data Corruption (Bit Flip) */
/* #define ATTACK_MODE_B */    /* Sniper Attack (PC Reset) */
/* #define ATTACK_MODE_C */    /* Infinite Hang (Deadlock) */

/* ============================================================================
 * TIMING CONFIGURATION
 * ============================================================================ */

#define HEARTBEAT_INTERVAL_MS       100U
#define FAULT_INJECT_INTERVAL_SEC   3U
#define WDT_TIMEOUT_SEC             2U

/* ============================================================================
 * UART CONFIGURATION
 * ============================================================================ */

#define UART_BAUD_RATE      115200UL

/* ============================================================================
 * EEPROM MEMORY MAP
 * ============================================================================ */

#define EEPROM_ADDR_MAGIC           0x0000
#define EEPROM_ADDR_CRASH_COUNT     0x0002
#define EEPROM_ADDR_TOTAL_UPTIME    0x0004

#define EEPROM_MAGIC_VALUE          0xAA55

/* ============================================================================
 * BUILD CONFIGURATION
 * ============================================================================ */

#define ENABLE_RESEARCH_SUMMARY     1
#define RESEARCH_SUMMARY_INTERVAL   10000U  /* ms */

#endif /* CONFIG_H */
