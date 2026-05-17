#pragma once //para evitar que un archivo header sea incluido varias veces
#include <stdint.h>
uint8_t decimal2bcd(uint8_t decimal);
uint8_t bcd2decimal(uint8_t bcd);
void i2c_init(void);
void ds1307_write_hours(uint8_t seconds, uint8_t minutes, uint8_t hour, 
                        uint8_t day, uint8_t date, uint8_t month, uint8_t year);
void ds1307_read_time(void);

