
/* #####################################################################################*/
// ALS25911.C
#include "als25911.h"
/* #####################################################################################*/

void ALS_powerup(uint8_t on){
  //POWERUP
  i2c_start(ALS+I2C_WRITE);							// set device address and write mode
  i2c_write(ALS_ENABLE_REG | 0xA0);					// write memory address (wnable register)
  i2c_write(on);	
  i2c_stop();										// set stop condition = release bus
  _delay_ms(2);										// delay of 2ms according to data sheet 
  if (on) {
    i2c_start(ALS+I2C_WRITE);						// set device address and write mode
    i2c_write(ALS_ENABLE_REG | 0xA0);				// write memory address (wnable register)
    i2c_write(0x03);	
    i2c_stop();										// set stop condition = release bus  £
	_delay_ms(2);									// delay of 2ms according to data sheet 
  }
}

void ALS_write(uint8_t address,uint8_t data){
  i2c_start(ALS+I2C_WRITE);							// set device address and write mode
  i2c_write(address | 0xA0 );						// write memory address
  i2c_write(data);
  i2c_stop();										// set stop condition = release bus
}

void ALS_read(uint8_t address, uint8_t * val)
{
  i2c_start(ALS+I2C_WRITE);							// set device address and write mode
  i2c_write(address | 0xA0);						// write address
  i2c_rep_start(ALS+I2C_READ);						// set device address and read mode
  *val=i2c_readNak();
  i2c_stop();										// set stop condition = release bus
}

void ALS_wait(uint8_t inttime){
  uint8_t i=0;
  _delay_ms(101);
  for (i=0;i<inttime+1;i++) {
    _delay_ms(101);
  } // WAIT UNTIL MEASUREMTS ARE DONE
}

void ALS_measure(uint8_t gainitime, uint16_t *VIS, uint16_t *IR){
	uint8_t i;
	ALS_write (ALS_CONTROL_REG,gainitime);
	for(i=0;i<2;i++){
		ALS_wait(gainitime&0x0F);
		i2c_start(ALS | I2C_WRITE);				// set device address and write mode
		i2c_write(ALS_CH0 | 0xA0);				// write address
		i2c_rep_start(ALS | I2C_READ);			// set device address and read mode
		*VIS=i2c_readAck();
		*VIS= (i2c_readAck()<<8) | *VIS;
		*IR=i2c_readAck();
		*IR= (i2c_readNak()<<8) | *IR;
		i2c_stop();	
	} // Drop first Measuerent
}

void ALS_autoGain(uint8_t * autogain, uint16_t *visval,uint16_t *irval){
  uint8_t adjust_direction=2;
  uint16_t VIS,IR;
  uint8_t gain=0x00; //// DEFAULT LOW GAIN 
  uint8_t inttime=0x02; // DEFAULT 300 ms
  ALS_measure(gain<<4|inttime,&VIS,&IR);
  while (VIS>ALS_THR_HIGH || VIS<ALS_THR_LOW){
    DEBUG_PRINT(gain);
    DEBUG_PRINT("/");
    DEBUG_PRINT(inttime);
    if (VIS<ALS_THR_LOW) { // LOW VALUE -> adjust to more light Adust direction 1
      if (adjust_direction==0) {
        while (VIS==0x0000){
          if (gain<ALS_MAX_GAIN) gain++;
          else if (inttime<ALS_MAX_INT) inttime++;
          ALS_measure(gain<<4|inttime,&VIS,&IR);
          if (gain==ALS_MAX_GAIN && inttime==ALS_MAX_INT) break;
        }
        break;
      }
      adjust_direction=1;
      if (gain<ALS_MAX_GAIN) gain++;
      else if (gain==ALS_MAX_GAIN && inttime<ALS_MAX_INT) inttime++;
      else break;

      DEBUG_PRINT(" -->");
    }
    else if (VIS>ALS_THR_HIGH) { // HIGH VALUE

      if (adjust_direction==1) { // Autoadjust overshooted the target value
        while (VIS==0xFFFF){
          if  (gain>ALS_MIN_GAIN) gain--;
          else if (inttime>ALS_MIN_INT) inttime--; 
          ALS_measure(gain<<4|inttime,&VIS,&IR);
          if (gain==ALS_MIN_GAIN && inttime==ALS_MIN_INT) break;
        }
        break;
      } 
      adjust_direction=0;
      if (gain>ALS_MIN_GAIN) gain--;
      else if (gain==ALS_MIN_GAIN && inttime>ALS_MIN_INT) inttime--;
      else  break;
      DEBUG_PRINT("<-- ");
    }
    DEBUG_PRINT("\n");
    ALS_measure(gain<<4|inttime,&VIS,&IR);
  }
  *visval=VIS;
  *irval=IR;	
  *autogain=(gain<<4|inttime);
}
