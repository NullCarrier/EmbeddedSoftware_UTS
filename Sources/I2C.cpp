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
  I2C0_C1 |= I2C_C1_IICIE_MASK;

  __EI();//Enable interrupt
  return true;
}







	 
	 
}
 





