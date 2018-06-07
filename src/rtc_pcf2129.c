/* #####################################################################################*/
// rtc_pcf2129.C
#include "rtc_pcf2129.h"
/* #####################################################################################*/

uint8_t BCD_encode(uint8_t timeval){      
  uint8_t tens=timeval/10;
  uint8_t ones=(timeval-tens*10);
  timeval= (tens<<4) + ones;
  return (timeval);
}

uint8_t BCD_decode(uint8_t bcd){
  uint8_t tens=bcd>>4;
  uint8_t ones=bcd&0x0F;
  bcd= tens*10 + ones;
  return (bcd);
}

void setRTC(struct time * stime){
  initializeRTC();
  i2c_start(RTC+I2C_WRITE);							// set device address and write mode
  i2c_write(0x03);									// write address
  i2c_write(BCD_encode(stime->second));				// write value
  i2c_write(BCD_encode(stime->minute));				// write value
  i2c_write(BCD_encode(stime->hour));				// write value
  i2c_write(BCD_encode(stime->day));				// write value
  i2c_write(0x00);									// WEEKDAY / NOTUSED write value
  i2c_write(BCD_encode(stime->month));				// write value
  i2c_write(BCD_encode(stime->year));				// write value
  i2c_stop();										// set stop condition = release bus
}

void getRTC(struct time * ctime){
  i2c_start(RTC+I2C_WRITE);							// set device address and write mode
  i2c_write(0x03);
  i2c_stop();		
  i2c_start(RTC+I2C_READ);							// write address
  //i2c_rep_start(RTC+I2C_READ);					// REP_START DOESNT WORK HERE? device address and read mode
  ctime->second=BCD_decode(i2c_readAck()&0x7F);								
  ctime->minute=BCD_decode(i2c_readAck());								
  ctime->hour=BCD_decode(i2c_readAck());									
  ctime->day=BCD_decode(i2c_readAck());
  i2c_readAck();									// drop	WEEKDAY
  ctime->month=BCD_decode(i2c_readAck());								
  ctime->year=BCD_decode(i2c_readNak());
  i2c_stop();										// set stop condition = release bus
}

uint8_t	get_alarm(uint8_t interval,uint8_t unit,uint8_t delayed){
  //  Here the logging intervals are defined: allow only seconds only if logging interval is less than 5 minutes ....
  uint8_t alarm;
  // alarm 			7 			6		    |      5             4           3 			 2 				1				0
  // 			(delayed start) (every second)  |     (NA)          (NA)     (day match) (hour match) (minute match) (second match) 
  // ATTENTION PCF2191 used 0 as alarm enabled and 1 as alarm diabled... 
  if (delayed) return (0x80);
  if (unit==0){										// interval is set in seconds
    if (interval % 60 == 0) alarm=0x3E;				// Every minute (when seconds match)
    else alarm=0x7F;								// Run Interrupt every SECOND, let program filter unimportant ones...
  }
  else if (unit==1){								// interval is set in minutes
    if (interval % 60 == 0) alarm=0x3C;				// Every Hour (when minutes and secons match)
    else alarm=0x3E;								// Run Interrupt every minute (when soconds match), let program filter unimportant ones... Alarm Mask 0 1 1 1 on MSB in register 0x07-0x0A
  }
  else {											// interval is set in hours
    if (interval % 24 == 0) alarm=0x38;				// Every Day  (when hours minutes and secons match)
    else alarm=0x3C;								// Run Interrupt every hour (when minures and soconds match), let program filter unimportant ones... Alarm Mask 0 1 1 1 on MSB in register 0x07-0x0A
  }
  return (alarm);
}

void set_RTC_Alarm(struct rtcal1 * salarm){
  if (!(salarm->ALARM_SEC_INTERRUPT)) { // SECOND INTERVAL is directly enabled from control register in setRTCInterrupt function...
    i2c_start (RTC+I2C_WRITE);					// set device address and write mode
    i2c_write(0x0A);							// write address
    i2c_write(BCD_encode(salarm->second) | ((salarm->alarm&0x01)<<7));		// write uint8_t
    i2c_write(BCD_encode(salarm->minute) | ((salarm->alarm&0x02)<<6));		// write uint8_t
    i2c_write(BCD_encode(salarm->hour)   | ((salarm->alarm&0x04)<<5));		// write uint8_t
    i2c_write(BCD_encode(salarm->day)    | ((salarm->alarm&0x08)<<4));		// write uint8_t
    i2c_stop();								// set stop condition = release bus
  }
}

void setRTC_Interrupt(uint8_t on, uint8_t alarm){ //OK
  i2c_start (RTC+I2C_WRITE);				// set device address and write mode
  if (ALARM_SEC_INTERRUPT){  						// We have INT set to every second
    i2c_write(0x00);						// write address
    i2c_write(on);							// write address
	i2c_write(0x00);
  } 
  else if (ALARM_DELAYED_START) {					// DELAYED START
    i2c_write(0x01);						// write address
	i2c_write(on<<1);						// ENABLE AIF : INT Pin Follows ALARM FUNCTIONS
  }
  else {
    i2c_write(0x00);						// write address
    i2c_write(on<<1);						// ENABLE MI : MINUTE PULSE for Alive Signal
	i2c_write(on<<1);						// ENABLE AIF : INT Pin Follows ALARM FUNCTIONS
  }	
  i2c_stop();								// set stop condition = release bus
}

void clearRTC_alarmFlags(uint8_t alarm){ 	//OK
  i2c_start (RTC+I2C_WRITE);				// set device address and write mode
  i2c_write(0x01);							// write address
  if (ALARM_SEC_INTERRUPT) i2c_write(0x00);  			// We have INT set to every second
  else i2c_write(0x02);						// ENABLE AIF : INT Pin Follows ALARM FUNCTIONS	
  i2c_stop();								// set stop condition = release bus
}

uint8_t getRTC_alarmFlags(){ //OK
  uint8_t af=0;
  i2c_start (RTC+I2C_WRITE);				// set device address and write mode
  i2c_write(0x01);							// write address
  i2c_stop();
  i2c_start (RTC+I2C_READ);					// set device address and write mode
  af=i2c_readNak();
  i2c_stop();
  return af;
}   

void initializeRTC(){  //OK
  i2c_start (RTC+I2C_WRITE);				// set device address and write mode
  i2c_write(0x00);							// write address CONTROL REGISTER 1
  i2c_write(0x00);							// write data: DEFAULT
  i2c_write(0x00);							// write data: DEFAULT
  i2c_write(0x00);							// write data: DEFAULT
  i2c_stop();								// set stop condition = release bus
  i2c_start (RTC+I2C_WRITE);				// set device address and write mode
  i2c_write(0x10);							// write address WATCHDOG REGISTER
  i2c_write(0x23);							// write address Watchdog timer: Set second/minute interrupt as pulse..... No need to clear flag
  i2c_stop();								// set stop condition = release bus
}

void stopRTC(){ //OK
  i2c_start (RTC+I2C_WRITE);				// set device address and write mode
  i2c_write(0x00);							// write address
  i2c_write(0x20);							// CLOCK OSCILLATOR STOPPED
  i2c_stop();								// set stop condition = release bus
}
