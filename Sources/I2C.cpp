/*! @file
 *
 *  @brief I/O routines for the K70 I2C interface.
 *
 *  This contains the functions for operating the I2C (inter-integrated circuit) module.
 *
 *  @author PMcL
 *  @date 2015-09-17
 */


 #include "I2C.h"


namespace I2C{

 bool Init() const
 {
  __DI();//Disable interrupt;

  //Enable clock gate control of I2C0
  SIM_SCGC4 |= SIM_SCGC4_IIC0_MASK;
  
  // Enable the clock for PortE
  SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;

  //  PIN multiplexing of SDA and SCL
  PORTE_PCR18 |= PORT_PCR_MUX(4);
  PORTE_PCR19 |= PORT_PCR_MUX(4);
  
  //Enable I2C0 module
  I2C0_C1 |= I2C_C1_IICEN_MASK;

  //Select master mode
  I2C0_C1 |= I2C_C1_MST_MASK;

  //Transmit Mode
  I2C0_C1 |= I2C_C1_TX_MASK;

  //Ack mode
  I2C0_C1 &= ~I2C_C1_TXAK_MASK;

  //Repeat START
  I2C0_C1 |= I2C_C1_RSTA_MASK;

  //Request baudRate


  //NVIC
  //Vector = 40, IRQ = 24
  NVICICPR0 |= (1 << (24 % 32) );
  NVICISER0 |= (1 << (24 % 32) );

  //Enable I2C interrupt
  //I2C0_C1 |= I2C_C1_IICIE_MASK;

  __EI();//Enable interrupt
  return true;
 }


 void I2C_t::SelectSlaveDevice(const uint8_t slaveAddress)
 {
  I2C0_RA |= (slaveAddress << 1); // Write into 7-bit Address
 }

 void I2C_t::Write(const uint8_t registerAddress, const uint8_t data)
 {
  I2C0_RA |= (registerAddress << 1); // Write into 7-bit Address

  //Transfer complete? and whether receive ack bit
  if ( (I2C0_S & I2C0_S_TCF_MASK) || (I2C0_S & I2C0_S_RXAK_MASK) ){
  //Generate STOP bit via selecting slave mode
  I2C0_C1 &= ~I2C_C1_MST_MASK;

  }
  else if (I2C0_S & I2C0_S_SRW_MASK){
  //switch to Rx mode
  //?
  }
  else 
  I2C0_D |= data; //initiate data transmitting
 }


 void I2C_t::PollRead(const uint8_t registerAddress, uint8_t &data, const uint8_t nbBytes)
 {
  I2C0_A2 |= (registerAddress << 1); //acquire data from specfic regAddress

  if (nbBtytes != 1){   

  I2C0_SMB |= I2C0_SMB_FACK_MASK;//Enable TXAK 

  I2C0_C1 |= I2C0_C1_TXAK_MASK; // No ack signal
  }
  else{

  //Generate STOP bit via selecting slave mode
  I2C0_C1 &= ~I2C_C1_MST_MASK;

  } 
    
  data = I2C0_D; // Read from data reg
 }

 


}






