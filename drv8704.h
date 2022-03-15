#pragma once


extern void drv8704_init(void);
extern void drv8704_write(uint8_t addr, uint16_t value);
extern bool drv8704_read(uint8_t addr, uint16_t* value);
