
#include "eeprom_drv.h"
#include "atmega328p.h"


static inline void eeprom_wait_ready(void)
{
    /* Wait for EEPE (EEPROM Program Enable) to clear */
    while (BIT_GET(REG_EECR, EECR_EEPE)) {
        /* Spin */
    }
}


uint8_t eeprom_read_byte(uint16_t addr)
{
    /* Wait for any previous write to complete */
    eeprom_wait_ready();
    
    /* Set address registers */
    REG_EEARH = HIGH_BYTE(addr);
    REG_EEARL = LOW_BYTE(addr);
    
    /* Start read by setting EERE (EEPROM Read Enable) */
    BIT_SET(REG_EECR, EECR_EERE);
    
    /* Return data from EEPROM data register */
    return REG_EEDR;
}

void eeprom_write_byte(uint16_t addr, uint8_t data)
{
    /* Wait for any previous write */
    eeprom_wait_ready();
    
    /* Set address */
    REG_EEARH = HIGH_BYTE(addr);
    REG_EEARL = LOW_BYTE(addr);
    
    /* Set data */
    REG_EEDR = data;
    
    /* Disable interrupts during timed write sequence */
    CRITICAL_SECTION_BEGIN;
    
// ...existing code...
    /*
     * Write sequence (per datasheet):
     * 1. Set EEMPE (Master Program Enable)
     * 2. Within 4 cycles, set EEPE (Program Enable)
     */
    BIT_SET(REG_EECR, EECR_EEMPE);
    BIT_SET(REG_EECR, EECR_EEPE);
    
    CRITICAL_SECTION_END;
}

/* ============================================================================
 * WORD OPERATIONS (16-bit)
 * ============================================================================ */

uint16_t eeprom_read_word(uint16_t addr)
{
    uint16_t value;
    
    /* Little-endian: LSB first */
    value = eeprom_read_byte(addr);
    value |= ((uint16_t)eeprom_read_byte(addr + 1) << 8);
    
    return value;
}

void eeprom_write_word(uint16_t addr, uint16_t data)
{
    /* Little-endian: LSB first */
    eeprom_write_byte(addr, LOW_BYTE(data));
    eeprom_write_byte(addr + 1, HIGH_BYTE(data));
}

/* ============================================================================
 * DWORD OPERATIONS (32-bit)
 * ============================================================================ */

uint32_t eeprom_read_dword(uint16_t addr)
{
    uint32_t value;
    
    value = eeprom_read_byte(addr);
    value |= ((uint32_t)eeprom_read_byte(addr + 1) << 8);
    value |= ((uint32_t)eeprom_read_byte(addr + 2) << 16);
    value |= ((uint32_t)eeprom_read_byte(addr + 3) << 24);
    
    return value;
}

void eeprom_write_dword(uint16_t addr, uint32_t data)
{
    eeprom_write_byte(addr,     (uint8_t)(data));
    eeprom_write_byte(addr + 1, (uint8_t)(data >> 8));
    eeprom_write_byte(addr + 2, (uint8_t)(data >> 16));
    eeprom_write_byte(addr + 3, (uint8_t)(data >> 24));
}

/* ============================================================================
 * BLOCK OPERATIONS
 * ============================================================================ */

void eeprom_read_block(uint16_t addr, void *dest, uint16_t len)
{
    uint8_t *p = (uint8_t *)dest;
    
    while (len--) {
        *p++ = eeprom_read_byte(addr++);
    }
}

void eeprom_write_block(uint16_t addr, const void *src, uint16_t len)
{
    const uint8_t *p = (const uint8_t *)src;
    
    while (len--) {
        eeprom_write_byte(addr++, *p++);
    }
}

/* ============================================================================
 * UPDATE OPERATIONS (Wear Leveling)
 * ============================================================================ */

void eeprom_update_byte(uint16_t addr, uint8_t data)
{
    /* Only write if value is different (extends EEPROM life) */
    if (eeprom_read_byte(addr) != data) {
        eeprom_write_byte(addr, data);
    }
}

void eeprom_update_word(uint16_t addr, uint16_t data)
{
    if (eeprom_read_word(addr) != data) {
        eeprom_write_word(addr, data);
    }
}

void eeprom_update_dword(uint16_t addr, uint32_t data)
{
    if (eeprom_read_dword(addr) != data) {
        eeprom_write_dword(addr, data);
    }
}
