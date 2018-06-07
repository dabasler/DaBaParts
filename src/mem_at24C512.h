#ifndef __MEMAT24C512_H__
#define __MEMAT24C512_H__
/* #####################################################################################*/
// MEMIC.H
// #include <avr/io.h>
 #include "i2cmaster.h"
/* #####################################################################################*/
#define EXTMEM  0xA0      // device address of EEPROM Atmel AT24C512C, see datasheet
#define EXTMEM_SIZE  0xFFFF 
void writeEEPROM_I2C ( uint16_t address, void * data, uint8_t nbytes);
void readEEPROM_I2C ( uint16_t  address, void * data, uint8_t nbytes);
void eraseallEEPROM_I2C();
#endif