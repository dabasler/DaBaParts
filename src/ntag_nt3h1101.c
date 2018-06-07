/* #####################################################################################*/
// NTAG.C
 #include "ntag_nt3h1101.h"
/* #####################################################################################*/


/* NTAG COMMUNICATION:
 NTAG rests in RF to I2C direction, Pass-through mode OFF
 Upon RF Field detection, NTAG rests in Pass-through mode (RF to I2C direction) is activated.
 If RFMaster writes Something, uC is Informed by FD Pin that data is ready.
 uC reads data and decides next step:
 	# Data requests
 	if uint8_t 0 is 0x00: status readout
 	if uint8_t 0 is 0x01: program readout
 	if uint8_t 0 is 0x02: data readout
 	
 	to fulfill data requests, NTAG changes Pass-through mode to I2C to RF direction)
 		data is written into SRAM via I2C  (at the end of WRITE, SRAM_RF_READY gets 1,RF can read,when finished FD indicates readout complete)
 	
 	# Data Set	
 	if uint8_t 0 is 0x10: stop logger
 	if uint8_t 0 is 0x20: set time
 	if uint8_t 0 is 0x30: program logger
 	
 	Upon completion of the request, uint8_t 0 is set to FF: initial status, do nothing
 NTAG rests in Pass-through mode (RF to I2C direction)
 
 
 1. FD: Field is detected, FD goes Low, uC wakes up on PCINT, Tests for Field, Starts PASS-THROUGH, Sets FC to Show When Data IS READY
 2. NFC MASTER sends Direction (0 uint8_t to request readout//  or 0xFF and Program)
 3 Pass-Through is activated on NTAG, direction bit set
 4 Data is exchanged.
 
 FD Interrupt Signals 3 Things:
 	1. Field detection -> Init Session
 	2. New data from RF (if Session initialized and readout==false) -> read data with uC
 	3. data read by RF (if Session initialized and readout==true) -> send more data 
 
 
 
 The READ and WRITE operation handle always 16 bytes to be read or written (one block)
 
 */

void NTAG_SDRAM_PASS(uint8_t on)
{
  //WRITE SESSION REGISTER
  DEBUG_PRINTLN ("SDRAM PASSTHROUGH:");
  DEBUG_PRINTLN (on);
  i2c_start(NTAG+I2C_WRITE);						// set device address and write mode
  i2c_write(NTAG_SREG);							// write memory address (Session register)
  i2c_write(NTAG_NC_REG);							// write register address  (NC_REG)
  i2c_write(0b01111100);   	                    // MASK
  //if (on)  i2c_write(0b01111100);
  if (on)  i2c_write(0b01000000);
  else i2c_write(0b00000000);
  //i2c_write(0b00111100|(on<<6));							// Soft reset OFF, PassThrough ON, FD_OFF (High if NO_FIELD or reading RF data)2B,FD_ON (pin low if data is ready to read over I2C) 2B, SRAM Mirror OFF, FlowDir (RF --> I2C)
  i2c_stop();								// set stop condition = release bus
}

uint8_t get_NTAG_Status(){
  uint8_t status=0;
  DEBUG_PRINTLN ("READING STATUS");
  i2c_start(NTAG+I2C_WRITE);						// set device address and write mode
  i2c_write(NTAG_SREG);							// write memory address  (Session register block)
  i2c_write(NTAG_NS_REG);							// write register address  (NS_REG)
  i2c_rep_start(NTAG+I2C_READ);					// set device address and read mode
  status= i2c_readNak();							// read NS_REG
  i2c_stop();										// set stop condition = release bus
  return status;
}

void NTAG_I2CLock(uint8_t lock){
  DEBUG_PRINTLN ("Unlocking I2C");
  i2c_start(NTAG+I2C_WRITE);						// set device address and write mode
  i2c_write(NTAG_SREG);							// write memory address  (Session register block)
  i2c_write(NTAG_NS_REG);							// write register address  (NS_REG)
  i2c_write(0x40);								// Disable soft reset, NA,FD_OFF (pin High if no field)2B,FD_ON (pin low if valid Tag Selection) 2B, NA, FlowDir (0 i2c-rf,1 RF I2C)
  i2c_write(lock<<3);								// Disable soft reset, NA,FD_OFF (pin High if no field)2B,FD_ON (pin low if valid Tag Selection) 2B, NA, FlowDir (0 i2c-rf,1 RF I2C)
  i2c_stop();										// set stop condition = release bus
}

void start_NTAG_IN(){
  //WRITE SESSION REGISTER
  DEBUG_PRINTLN ("SET SRAM IN");
  i2c_start(NTAG+I2C_WRITE);						// set device address and write mode
  i2c_write(NTAG_SREG);							// write memory address (Session register)
  i2c_write(NTAG_NC_REG);							// write register address  (NC_REG)
  i2c_write(0b00000001);   	                    // MASK
  i2c_write(0b00000001);							// Soft reset OFF, PassThrough ON, FD_OFF (High if NO_FIELD or reading RF data)2B,FD_ON (pin low if data is ready to read over I2C) 2B, SRAM Mirror OFF, FlowDir (RF --> I2C)
  //i2c_write(0b00111101);   	                    // MASK
  //i2c_write(0b00111101);							// Soft reset OFF, PassThrough ON, FD_OFF (High if NO_FIELD or reading RF data)2B,FD_ON (pin low if data is ready to read over I2C) 2B, SRAM Mirror OFF, FlowDir (RF --> I2C)
  i2c_stop();										// set stop condition = release bus
}

void start_NTAG_OUT(){
  // SESSION REGISTER
  DEBUG_PRINTLN ("SET SRAM OUT");
  i2c_start(NTAG+I2C_WRITE);						// set device address and write mode
  i2c_write(NTAG_SREG);							// write memory address (Session register)
  i2c_write(NTAG_NC_REG);							// write register address  (NC_REG)
  i2c_write(0b00000001);                          // MASK
  i2c_write(0b00000000);							// Soft reset OFF, PassThrough ON, FD_OFF (High if NO_FIELD or reading I2C data)2B,FD_ON (pin low if RF data reading finished) 2B, SRAM Mirror OFF, FlowDir (I2C --> RF)
  //i2c_write(0b00111101);                          // MASK
  //i2c_write(0b00111100);							// Soft reset OFF, PassThrough ON, FD_OFF (High if NO_FIELD or reading I2C data)2B,FD_ON (pin low if RF data reading finished) 2B, SRAM Mirror OFF, FlowDir (I2C --> RF)
  i2c_stop();										// set stop condition = release bus
}

void write_NTAG_page(uint8_t page, void * data)
{
  DEBUG_PRINTLN ("WRITE PAGE");
  uint8_t i,*ptr=(uint8_t *)data; 						//NTAG i2C handles 16 bytes per command
  i2c_start_wait(NTAG | I2C_WRITE);			// set device address and write mode
  i2c_write(page);
  DEBUG_PRINT(page);
  DEBUG_PRINT(": ");								// write address
  for (i=0;i<16;i++){
    i2c_write(*(ptr+i));
    DEBUG_PRINT( *(ptr+i),HEX);
    DEBUG_PRINT( " ");
  }		// write data to address
  i2c_stop();										// set stop condition = release bus
  DEBUG_PRINT("\n");
}

void read_NTAG_page(uint8_t page, void * data)
{
  DEBUG_PRINTLN ("READ PAGE");
  uint8_t i,*ptr=(uint8_t *)data; 						//NTAG i2C handles 16 bytes per command
  i2c_start_wait(NTAG | I2C_WRITE);					// set device address and write mode
  i2c_write(page);								// write address
  DEBUG_PRINT(page,HEX);
  DEBUG_PRINT(": ");		        			// write address
  //i2c_stop();										// set stop condition = release bus
  //_delay_ms(60);
  //i2c_start(NTAG | I2C_READ);					// set device address and write mod
  i2c_rep_start(NTAG | I2C_READ);					// set device address and read mode
  for (i=0;i<15;i++){
    ptr[i]=i2c_readAck();
    DEBUG_PRINT(ptr[i],HEX);
    DEBUG_PRINT( " ");
  }		// read uint8_t
  ptr[15]=i2c_readNak();
  DEBUG_PRINTLN(ptr[15],HEX);
  i2c_stop();										// set stop condition = release bus
}
/*
THE FOLLOWING FUNCTION WAS MOVED TO THE MAIN FILE; AS LOGGING DURING AITING SHOULD BE POSSIBLE....
uint8_t NTAGwait(uint8_t i2c) // in_out defibnes the direction to wait, 1 Wait until data is ready to read via i2c, 0 Wait until data has been read by RF
{
  uint8_t NTAG_status;
  while (1){ 	//Wait until data is ready to read via i2c
	NTAG_status=get_NTAG_Status();
	//NTAG_pin_status=NTAG_PIN_STATUS;
	if (!NTAG_status) return 1; //Field Lost
    if (i2c==1 && (NTAG_status&0x10) ) {
	NTAG_SDRAM_PASS(0); // Disable PASS THROUGH TO READ OUT SDRAM VIA I2C
	break;}   //I2C Data Ready becomes 1
	else if (i2c==0 && !(NTAG_status&0x08)) break;  //Data has been read by RF becomes 0
    PORTA ^= 1 << 7;
	_delay_ms(10);
	 if (i2c==1) _delay_ms(10); // SLOW BLINK IF WAITING FOR COMMAND
  }
  _delay_ms(2);
  //PORTA |= _BV(PA7);
  PORTA = 0x04;
  return 0;
}
*/