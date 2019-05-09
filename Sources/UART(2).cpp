/*! @file UART(2).cpp
 *
 *  @brief I/O routines for UART communications on the TWR-K70F120M.
 *
 *  This contains the functions for operating the UART (serial port).
 *
 *  @author Chao Li
 *  @date 07/05/2019
 *  Copyright (c) Chao Li. All rights reserved.
 */

#include "UART(2).h"

// include fmod()
#include <cmath>


// const number for converting baudrate into SBR
const float DIVISIOR = 16.0;

uint32_t UART_t::baudRate;
uint32_t UART_t::moduleClk;

// This function is only used to obtain BRFA
// Baud rate = UART module clock / (16* (SBR[12:0]+BRFD) )
static uint8_t GetFraction(const uint32_t &baudRate, const uint32_t &moduleClk)
{
  float sbr = (moduleClk / baudRate) / DIVISIOR; // using this formula to obtain SBR
                                                        // and typecast it into float type
  float sbr_Fraction = fmod(sbr, static_cast<int>(sbr)); // fmod will return reminder
                                                // using reminder to obtain fractional part of SBR

 return static_cast<uint8_t> (sbr_Fraction * 2 * DIVISIOR); //multiplying sbr_fracion by 32,
                                                      // and typecast it into uint8_t
}


bool UART_t::Init()
{
  __DI();//Disable interrupt

 //local variable for storing SBR using union type
  uint16union_t sbr;

 //Enable the clock module for UART2
  SIM_SCGC4 |= SIM_SCGC4_UART2_MASK;

 // Enable the clock for PortE
  SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;

 //  PIN multiplexing
  PORTE_PCR16 |= PORT_PCR_MUX(3);
  PORTE_PCR17 |= PORT_PCR_MUX(3);

// Write the integer portion of the Baudrate to the BDH/L register.
  sbr.l = (uint16_t) (moduleClk / baudRate) / DIVISIOR;
  UART2_BDH = sbr.s.Hi;
  UART2_BDL = sbr.s.Lo;

// Write the fractional portion of the Baudrate to the UART2_C4 register.
  UART2_C4 |= UART_C4_BRFA(GetFraction(baudRate, moduleClk));

 // Enable the receiver, transmitter
  UART2_C2 |= (UART_C2_RE_MASK | UART_C2_TE_MASK);

 //The interrupt source is enabled in the NVIC
 // vector num = 65, IRQ = 49 means should use NVICICPR1 IRQ(32-63)
 // Clear any pending interrupts on UART2
  NVICICPR1 = NVIC_ICPR_CLRPEND(1 << 17);

 // Enable interrupts from UART2 module
  NVICISER1 = NVIC_ISER_SETENA(1 << 17);

 //Set priority?

  __EI();// Enable the interrupt

  UART2_C2 |= UART_C2_RIE_MASK; // Arm the receive interrupt

  return true;
}


bool UART_t::InChar(uint8_t* const dataPtr)
{
 return this->TFIFO::Get(dataPtr); // retrieve data from FIFO and send it to Packet module
}


void UART_t::Module(const uint32_t rate, const uint32_t clock)
{
  baudRate = rate;
  moduleClk = clock;
}


 bool UART_t::OutChar(const uint8_t data)
{
 if (this->TFIFO::Put(data)) // Packet module requires to send data to FIFO
 UART2_C2 |= UART_C2_TIE_MASK;// Arm output device
}


#if 0
void UART_Poll(void)
{
  // receiving data condition
  // To check the state of RDRF bit
 if (UART2_S1 & UART_S1_RDRF_MASK)
{
  // let the receiver to send a byte of data to RxFIFO
  RxFIFO.FIFO_Put(UART2_D);
}
// To check the state of TDRE bit
 else if (UART2_S1 & UART_S1_TDRE_MASK)
 {
  //send data from TxFIFO to data register
  TxFIFO.FIFO_Get((uint8_t *) &UART2_D);
 }

}
#endif

void __attribute__ ((interrupt)) UART_t::ISR(void)
{
  // receiving data condition
 if (UART2_C2 & UART_C2_RIE_MASK)
 {
  if (UART2_S1 & UART_S1_RDRF_MASK) // To check the state of RDRF bit
  {
   this->TFIFO::Put(UART2_D); // let the receiver to send a byte of data to RxFIFO
  }
 }

// Transmit a byte of data
 if (UART2_C2 & UART_C2_TIE_MASK)
 {
  if (UART2_S1 & UART_S1_TDRE_MASK)
  {

   if (!this->TFIFO::Get( (uint8_t*) &UART2_D ) )
   {
	UART2_C2 &= ~UART_C2_TIE_MASK; // Disarm the UART output
   }

  }

 }

}


