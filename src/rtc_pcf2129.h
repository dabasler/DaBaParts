#ifndef __RTC_H__
#define __RTC_H__
/* #####################################################################################*/
// RTC.H
// #include <avr/io.h>
#include "i2cmaster.h"
/* #####################################################################################*/

#define RTC  0xA2     // device address of PCF2129 RTC
#define ALARM_SEC_INTERRUPT alarm&0x40
#define ALARM_DELAYED_START alarm&0x80

struct time  { // 6 Byte
  uint8_t year; // two digits, 2000 is assumed
  uint8_t month ;
  uint8_t day;
  uint8_t hour;
  uint8_t minute ;
  uint8_t second ;
};

struct rtcal1{ //5 bytes
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  uint8_t alarm; // Defines which alarms are set 
};

void setRTC(struct time * stime);
void getRTC(struct time * ctime);
uint8_t	get_alarm(uint8_t interval,uint8_t unit,uint8_t delayed);
void set_RTC_Alarm (struct rtcal1 * salarm);
void setRTC_Interrupt(uint8_t on, uint8_t alarm);
void clearRTC_alarmFlags(uint8_t alarm);
uint8_t getRTC_alarmFlags();
void initializeRTC();
void stopRTC();
uint8_t BCD_encode(uint8_t timeval);      
uint8_t BCD_decode(uint8_t bcd);
#endif