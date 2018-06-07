
/* #####################################################################################*/
// TEMP_SI7051.C
#include "temp_si7051.h"
/* #####################################################################################*/

void TEMPSI_hold(uint16_t *T) { 		// Reads out the temperautre in hold mode (uses clock stretching)
  uint8_t lsb,msb;
  i2c_start(TEMPSI+I2C_WRITE); 			// set device address and write mode
  i2c_write(CMD_TEMPSI_MEASURE_HOLD); 	// write address
  i2c_rep_start(TEMPSI+I2C_READ); 		// set device address and read mode
  msb=i2c_readAck();
  lsb=i2c_readNak();
  *T= (msb<<8) + lsb;
  i2c_stop();						// set stop condition = release bus
}

void TEMPSI_nohold_init(){
  i2c_start(TEMPSI+I2C_WRITE); 			// set device address and write mode
  i2c_write(CMD_TEMPSI_MEASURE_NOHOLD); // write address
  }

void TEMPSI_nohold_read(uint16_t * T){
	i2c_start_wait(TEMPSI+I2C_READ); 	// set device address and read mode
	*T=i2c_readAck();
	*T= (i2c_readNak()<<8) | *T;
	i2c_stop();
	}


void TEMPSI_reset() { 		// Reads out the temperautre in hold mode (uses clock stretching)
  i2c_start(TEMPSI+I2C_WRITE); 			// set device address and write mode
  i2c_write(CMD_TEMPSI_RST);
  i2c_stop();							// set stop condition = release bus
}

void TEMPSI_firmware(uint8_t *T) { 		// Reads out the temperautre in hold mode (uses clock stretching)
  i2c_start(TEMPSI+I2C_WRITE); 			// set device address and write mode
  i2c_write(CMD_TEMPSI_FW1_1);
  i2c_write(CMD_TEMPSI_FW1_2);
  //i2c_stop();
  i2c_rep_start(TEMPSI+I2C_READ); 		// set device address and read mode
  T[0]=i2c_readAck();
  i2c_readAck(); //CRC
  T[1]=i2c_readAck();
  i2c_readAck(); //CRC
  T[2]=i2c_readAck();
  i2c_readAck(); //CRC
  T[3]=i2c_readAck();
  i2c_readNak(); //CRC
  i2c_stop();							// set stop condition = release bus
}

//Temperature (degC)=(175.72*Temp_Code)/65536 – 46.85
/*
void TEMPSI_readconfig(uint8_t * cfg){
  i2c_start(TEMPSI+I2C_WRITE); 			// set device address and write mode
  i2c_write(CMD_TEMPSI_READ_USERREG); 	// write address
  i2c_rep_start(TEMPSI+I2C_READ); 		// set device address and read mode
  *cfg = i2c_readNak();
  i2c_stop();
  }
 
 
//Temperature (degC)=(175.72*Temp_Code)/65536 – 46.85
void TEMPSI_writeconfig(uint8_t cfg){
  i2c_start(TEMPSI+I2C_WRITE); 			// set device address and write mode
  i2c_write(CMD_TEMPSI_WRITE_USERREG); 	// write address
  i2c_write(cfg); 	// write address 
  i2c_stop();
  } 
*/