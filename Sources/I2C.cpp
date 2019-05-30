/*! @file I2C.cpp
 *
 *  @brief I/O routines for the K70 I2C interface.
 *
 *  This contains the functions for operating the I2C (inter-integrated circuit) module.
 *
 *  @author Chao Li
 *  @date 27/05/2019
 *  Copyright (c) Chao Li. All rights reserved.
 */


 #include "I2C.h"


namespace I2C
{

  using F = void (void*);

  F* I2C_t::readCompleteCallbackFunction;
  void* I2C_t::readCompleteCallbackArguments;

  bool I2C_t::Init() const
  {
    //Enable clock gate control of I2C0
    SIM_SCGC4 |= SIM_SCGC4_IIC0_MASK;

    // Enable the clock for PortE
    SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;

    // PIN multiplexing of SDA and SCL
    PORTE_PCR18 |= PORT_PCR_MUX(4);
    PORTE_PCR19 |= PORT_PCR_MUX(4);

    //Enable I2C0 module
    I2C0_C1 |= I2C_C1_IICEN_MASK;

    //Transmit Mode
    I2C0_C1 |= I2C_C1_TX_MASK;

    //Ack mode
    I2C0_C1 &= ~I2C_C1_TXAK_MASK;

    //Request baudRate: 100Kbps
    //Select ICR = 12, SCL = 64, MUL = 4
    I2C0_F |= 1 << 7;
    I2C0_F |= I2C_F_ICR(12);

    //NVIC
    //Vector = 40, IRQ = 24
   // NVICICPR0 = (1 << (24 % 32) );
   // NVICISER0 = (1 << (24 % 32) );

    //Enable I2C interrupt
    I2C0_C1 |= I2C_C1_IICIE_MASK;

    return true;
  }


  void I2C_t::SelectSlaveDevice(const uint8_t slaveAddress)
  {
     primarySlaveAddress = (slaveAddress << 1); // Write into 7-bit Address
  }

 /*! @brief Checking the receiving ack bit from slave
 *
 *   @return true ack bit has been received
 */
  inline bool InterruptFlag()
  {
    //wait for transimission complete
    if (I2C0_S & I2C_S_IICIF_MASK)
    {
	  I2C0_S |= I2C_S_IICIF_MASK; //Clear interrupt flag bit
	  return true;
    }

  }

 /*! @brief Send the address to slave under I2C protocol
  *
  *  @param slaveAddress The slave device address.
  *  @param registerAddress The register address.
  */
  void AddressCycle(uint8_t slaveAddress, const uint8_t registerAddress)
  {
    const uint8_t flag = ~0x01;

    //Transmit Mode
    I2C0_C1 |= I2C_C1_TX_MASK;

    // Send ACK signal to accel after receiving byte
    I2C0_C1 &= ~I2C_C1_TXAK_MASK;

    // Generate start signal to initiate communication
    I2C0_C1 |= I2C_C1_MST_MASK;

    do
    {
     // send the address of slave with R/W bit = 0
     slaveAddress &= flag;
     I2C0_D = slaveAddress;

     if (InterruptFlag() )
     {
       //slave receives the address
       I2C0_D = registerAddress; // Send address of register to slave
     }

    }while (!InterruptFlag());

  }


  void I2C_t::Write(const uint8_t registerAddress, const uint8_t data)
  {
     //Send address to accelerometer
     AddressCycle(primarySlaveAddress, registerAddress);

     I2C0_D = data;//Send data

     if (InterruptFlag() )
     I2C0_C1 &= ~I2C_C1_MST_MASK; //Generate STOP bit via selecting slave mode
  }


  I2C_t::I2C_t(const uint32_t clock, F* readCompleteCallbackFunc, void* readCompleteCallbackArgu):
  moduleClk(clock)
  {
    __DI(); //Disable interrupt;

    this->Init();

    //Assign callback function to static function pointers
    readCompleteCallbackFunction = readCompleteCallbackFunc;
    readCompleteCallbackArguments = readCompleteCallbackArgu;

    __EI(); //Enable interrupt
  }


  void I2C_t::PollRead(const uint8_t registerAddress, uint8_t* const data, const uint8_t nbBytes)
  {
    const uint8_t rFlag = 0x01;
    uint8* dataPtr = data;

    //Send address to accelerometer
    AddressCycle(primarySlaveAddress, registerAddress);

      //Generate signal of repeat START
    I2C0_C1 |= I2C_C1_RSTA_MASK;

      // send the address of slave with R/W bit = 1
    primarySlaveAddress |= rFlag;
    I2C0_D = primarySlaveAddress;


    if (InterruptFlag() )
    {
      //Master Rx mode
      I2C0_C1 &= ~I2C_C1_TX_MASK;

      //Reveiving data from slave
      for (uint8_t count = nbBytes; count != 0;count--)
	  {
         *dataPtr = I2C0_D; // Reading data from slav
         dataPtr++;
		
         if (count == 1)
         //Send NACK when last byte received
         I2C0_C1 |= I2C_C1_TXAK_MASK;
         else
         //Send Ack bit
         I2C0_C1 &= ~I2C_C1_TXAK_MASK;
      } 

    }

    //Generate STOP bit via selecting slave mode
    I2C0_C1 &= ~I2C_C1_MST_MASK;
    InterruptFlag();
  }


  void I2C_t::IntRead(const uint8_t registerAddress, uint8_t* const data, const uint8_t nbBytes)
  {
    const uint8_t rFlag = 0x01;
    uint8* dataPtr = data;

    //Send address to accelerometer
    //AddressCycle(primarySlaveAddress, registerAddress);

    if (InterruptFlag() )
    {
      //Generate signal of repeat START
      I2C0_C1 |= I2C_C1_RSTA_MASK;

      // send the address of slave with R/W bit = 1
      primarySlaveAddress |= rFlag;
      I2C0_D = primarySlaveAddress;
    }

    if (InterruptFlag() )
	{
     //Master Rx mode
     I2C0_C1 &= ~I2C_C1_TX_MASK;

  //Reveiving data from slave
  for (uint8_t count = nbBytes; count != 0;count--){

  if (count == 1) // last bytes to be read
  //Generate STOP bit via selecting slave mode
  I2C0_C1 &= ~I2C_C1_MST_MASK;
  else if (count == nbBytes - 1) // 2nd to last bytes to be read
  I2C0_C1 |= I2C_C1_TXAK_MASK; //Set TXACK

  *dataPtr++ = I2C0_D; // Reading data from slav

  }

  }

 }


/*

 void I2C::ISR()
 {
  //Clear IICIF
  I2C0_S |= I2C_S_IICIF_MASK;
  
  


 }

*/

}






