#ifndef __TEMPSI7051_H__
#define __TEMPSI7051_H__
/* #####################################################################################*/
// TEMP_SI7051.H
// #include <avr/io.h>
 #include "i2cmaster.h"
/* #####################################################################################*/
#define TEMPSI  0x40      // device address of TEMPERAUTRE SENSOR Si7051, see datasheet

#define CMD_TEMPSI_MEASURE_HOLD 0xE3
#define CMD_TEMPSI_MEASURE_NOHOLD 0xF3
#define CMD_TEMPSI_RST 0xFE

void TEMPSI_hold(uint16_t *T);
void TEMPSI_nohold_init();
void TEMPSI_nohold_read(uint16_t * T);


/* NO USED YET
#define CMD_TEMPSI_WRITE_USERREG 0xE6
#define CMD_TEMPSI_READ_USERREG 0xE7

#define TEMPSI_DEFAULT 0b00111010

// By default the device reads temperature in 14 bit mode.
// bit 7 and bit 0 indicate resolution

void TEMPSI_readconfig(uint8_t * cfg);
void TEMPSI_writeconfig(uint8_t cfg);
/*