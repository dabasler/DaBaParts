#ifndef __ALS25911_H__
#define __ALS25911_H__
/* #####################################################################################*/
// ALS25911.H
#include <avr/io.h>
#include <util/delay.h>
#include "i2cmaster.h"
#include "debug.h"
/* #####################################################################################*/

#define ALS  0x52		  // device address of TLS 25911 ALS-IR Sensor, see datasheet 

#define ALS_ENABLE_REG  0x00
#define ALS_CONTROL_REG 0x01
#define ALS_STATUS_REG  0x13
#define ALS_CH0 0x14
#define ALS_CH1 0x16

#define ALS_MAX_GAIN 0x03
#define ALS_MIN_GAIN 0x00
#define ALS_MAX_INT  0x05
#define ALS_MIN_INT  0x00
#define ALS_THR_LOW  45000  //ca 70 percent
#define ALS_THR_HIGH 60000  //ca 90 percent
void ALS_powerup(uint8_t on);
void ALS_write(uint8_t address,uint8_t data);
void ALS_read(uint8_t address, uint8_t * val);
void ALS_measure(uint8_t gainitime, uint16_t *VIS, uint16_t *IR);
void ALS_autoGain(uint8_t * autogain, uint16_t *visval,uint16_t *irval);

/* #####################################################################################*/

#endif