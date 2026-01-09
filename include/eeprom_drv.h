
#ifndef EEPROM_DRV_H
#define EEPROM_DRV_H

#include <stdint.h>


uint8_t eeprom_read_byte(uint16_t addr);

void eeprom_write_byte(uint16_t addr, uint8_t data);

uint16_t eeprom_read_word(uint16_t addr);

void eeprom_write_word(uint16_t addr, uint16_t data);

uint32_t eeprom_read_dword(uint16_t addr);

void eeprom_write_dword(uint16_t addr, uint32_t data);

void eeprom_read_block(uint16_t addr, void *dest, uint16_t len);
void eeprom_write_block(uint16_t addr, const void *src, uint16_t len);
void eeprom_update_byte(uint16_t addr, uint8_t data);
void eeprom_update_word(uint16_t addr, uint16_t data);
void eeprom_update_dword(uint16_t addr, uint32_t data);
void eeprom_read_block(uint16_t addr, void *dest, uint16_t len);
void eeprom_write_block(uint16_t addr, const void *src, uint16_t len);
void eeprom_update_byte(uint16_t addr, uint8_t data);
void eeprom_update_word(uint16_t addr, uint16_t data);
void eeprom_update_dword(uint16_t addr, uint32_t data);

#endif /* EEPROM_DRV_H */
