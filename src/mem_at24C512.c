/* #####################################################################################*/
// MEMIC.C
#include "mem_at24C512.h"
/* #####################################################################################*/

void writeEEPROM_I2C(uint16_t address,void * data, uint8_t nbytes)
{
  /* write nbytes to eeprom address (Page Write),
   	wait until the device is no longer busy from the previous write operation
   	The byte is internally incremeted, this function also handels page borders....
   	*/
  uint16_t page= address >>7;
  uint8_t  bytenum= address & 0x007F;  
  uint8_t i=0,*ptr=(uint8_t *)data;
  uint8_t addr_byte;
  i2c_start_wait(EXTMEM+I2C_WRITE);			// set device address and write mode
  addr_byte=address>>8;
  i2c_write(addr_byte);				//send MSByte address first
  addr_byte=address & 0x00FF;
  i2c_write(addr_byte);				//send LSByte address
  while (i<nbytes){
    i2c_write(*(ptr++)); // write data to address
    bytenum++;
    if (bytenum==0x80){ //Byte 128
      i2c_stop();							// set stop conditon = release bus
      page++;
      bytenum=0x00;
      address=page<<7;
      i2c_start_wait(EXTMEM+I2C_WRITE);			// set device address and write mode
      addr_byte=address>>8;
      i2c_write(addr_byte);				//send MSByte address first
      addr_byte=address & 0x00FF;
      i2c_write(addr_byte);				//send LSByte address	
    }
    i++;
  }	
  i2c_stop();								// set stop conditon = release bus
}


void readEEPROM_I2C(uint16_t  address, void * data, uint8_t nbytes)
{
  // max nbytes = 128
  /* write nbytes to eeprom address (Page Write), wait until the device is no longer busy from the previous write operation */
  uint8_t i,*ptr=(uint8_t *)data;
  uint8_t addr_byte;
  i2c_start_wait(EXTMEM+I2C_WRITE);			// set device address and write mode
  addr_byte=address>>8;
  i2c_write(addr_byte);				//send MSByte address first
  addr_byte=address & 0x00FF;
  i2c_write(addr_byte);				//send LSByte address
  i2c_rep_start(EXTMEM+I2C_READ);				// set device address and read mode
  for (i=0;i<(nbytes-1);i++){
    *(ptr++) = i2c_readAck();
  }	// read one uint8_t form address
  *(ptr++) = i2c_readNak();
  i2c_stop();									// set stop conditon = release bus
}


void eraseallEEPROM_I2C()
{
  /* erases the whole content of the i2c memory....*/
  uint16_t address=0x0000,i;
  uint8_t addr_byte,j;
  for (i=0;i<512;i++){
	i2c_start_wait(EXTMEM+I2C_WRITE);			// set device address and write mode
	addr_byte=address>>8;
	i2c_write(addr_byte);				//send MSByte address first
	addr_byte=address & 0x00FF;
	i2c_write(addr_byte);				//send LSByte address
	for (j=0;j<128;j++) i2c_write(0xFF); // write data to address
	address+=128;
  }
  i2c_stop();								// set stop conditon = release bus
}
