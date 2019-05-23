/*! @file I2C.cpp
 *
 *  @brief I/O routines for the K70 I2C interface.
 *
 *  This contains the functions for operating the I2C (inter-integrated circuit) module.
 *
 *  @author Chao Li
 *  @date 23/05/2019
 *  Copyright (c) Chao Li. All rights reserved.
 */


 #include "I2C.h"


namespace I2C{

 using F = void (void*);

 F* I2C_t::readCompleteCallbackFunction;
 void* I2C_t::readCompleteCallbackArguments;

 bool I2C_t::Init() const
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
  //I2C0_C1 |= I2C_C1_MST_MASK;

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
  NVICICPR0 = (1 << (24 % 32) );
  NVICISER0 = (1 << (24 % 32) );

  //Enable I2C interrupt
  I2C0_C1 |= I2C_C1_IICIE_MASK;

  __EI();//Enable interrupt
  return true;
 }


 void I2C_t::SelectSlaveDevice(const uint8_t slaveAddress)
 {
  I2C0_RA = (slaveAddress << 1); // Write into 7-bit Address
  primarySlaveAddress = I2C0_RA;
 }

 void I2C_t::Write(const uint8_t registerAddress, const uint8_t data)
 {
  const uint8_t flag = ~0x01;

  // Generate start signal to initiate communication
  I2C0_C1 |= I2C_C1_MST_MASK;

  do{
  // send the address of slave with R/W bit = 0
  I2C0_D = primarySlaveAddress;

  if (!(I2C0_S & I2C_S_RXAK_MASK)) {
  //slave receives the address
  I2C0_D = registerAddress; // Send address of register to slave
  }

  }while (I2C0_S & I2C_S_RXAK_MASK);

  I2C0_D = data;//Send data

  if (!(I2C0_S & I2C_S_RXAK_MASK))
  I2C0_C1 &= ~I2C_C1_MST_MASK; //Generate STOP bit via selecting slave mode

 }

 I2C_t::I2C_t(const uint32_t clock, F* readCompleteCallbackFunc, void* readCompleteCallbackArgu):
 moduleClk(clock)
 {
  this->Init();
  //Assign callback function to static function pointers
  readCompleteCallbackFunction = readCompleteCallbackFunc;

  readCompleteCallbackArguments = readCompleteCallbackArgu;
 }


 void I2C_t::PollRead(const uint8_t registerAddress, uint8_t* const data, const uint8_t nbBytes)
 {
  const uint8_t wFlag = ~0x01;
  const uint8_t rFlag = 0x01;
  uint8_t regAddress = registerAddress;
  uint8* dataPtr = data;

  I2C0_C1 &= ~I2C_C1_TXAK_MASK; // Send ACK signal to accel after receiving byte

  I2C0_C1 |= I2C_C1_MST_MASK; // Generate start signal to initiate communication

  //primarySlaveAddress &= wFlag;

  I2C0_D = primarySlaveAddress; // send the address of slave with R/W bit = 0

  if (!(I2C0_S & I2C_S_RXAK_MASK)) {
  //slave receives the address
  I2C0_D = registerAddress; // Send address of register to slave
  }

  if (!(I2C0_S & I2C_S_RXAK_MASK)) {
  //Generate signal of repeat START
  I2C0_C1 |= I2C_C1_RSTA_MASK;

  // send the address of slave with R/W bit = 1
  primarySlaveAddress |= rFlag;
  I2C0_D = primarySlaveAddress;

  }

  for (uint8_t count = nbBytes; count != 0;count--){

  if (!(I2C0_S & I2C_S_RXAK_MASK)) {
  //slave transimits the data
  *dataPtr = I2C0_D;  // Reading data from slave
  dataPtr++;
  }
  else{
  // wait until receive ack bit
  count++;
  continue;
  }
/*
  regAddress += 2; // read data from next register

  if (count != 1)
  // send the address of slave with R/W bit = 1
  I2C0_D = regAddress;
*/
  }

  I2C0_C1 |= I2C_C1_TXAK_MASK; // No ack signal

  //Generate STOP bit via selecting slave mode
  I2C0_C1 &= ~I2C_C1_MST_MASK;
 }




}






