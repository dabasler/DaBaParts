#ifndef __NTAG_H__
#define __NTAG_H__
// #####################################################################################*/
// NTAG.H
// #include <avr/io.h>
// #include <avr/eeprom.h>
#include <util/delay.h>
#include "i2cmaster.h"
#include "debug.h"
/* #####################################################################################*/


#define NTAG  0xAA		  // device address of NFC Communication, see data sheet


#ifdef NTAGI2C2K
#define NTAG_CREG 0x7A
#else
#define NTAG_CREG 0x3A
#endif
#define NTAG_SREG 0xFE
#define NTAG_NC_REG 0x00
#define NTAG_NS_REG 0x06

/*Accordingly, the application can align on the Reader & Host side to transfer 16/32/48/64
 bytes of data in one Pass-through step by only using the last blocks/page of the SRAM
 buffer.*/
#define NTAG_SRAM_64 0xF8  // 16 PAGES of SRAM (64 Bytes)
#define NTAG_SRAM_48 0xF9
#define NTAG_SRAM_32 0xFA
#define NTAG_SRAM_16 0xFB


void NTAG_SDRAM_PASS(uint8_t on);
uint8_t get_NTAG_Status();
void free_NTAG_I2CLock();
void start_NTAG_IN();
void start_NTAG_OUT();
void read_NTAG_page(uint8_t page, void * data);
void write_NTAG_page(uint8_t page, void * data);
//uint8_t NTAGwait(uint8_t in_out); MOVED TO MAIN

#endif