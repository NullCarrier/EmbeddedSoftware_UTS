/*! @file
 *
 *  @brief I/O routines for UART communications on the TWR-K70F120M.
 *
 *  This contains the functions for operating the UART (serial port).
 *
 *  @author Chao Li
 *   student ID: 12199434
 *  @date 02/04/2019
 *  Copyright (c) UTS. All rights reserved.
 */

#include "UART(2).h"
#include "MK70F12.h"// involve mask for all registers
#include <cmath>// include fmod()
#include "type_cpp.h" // uint16union_t

// const number for converting baudrate into SBR
static constexpr uint32_t DIVISIOR = 16;

// declare the object called RxFIFO, TxFIFO
static TFIFO RxFIFO;
static TFIFO TxFIFO;

// This function is only used to obtain BRFA
static uint8_t GetFraction(const uint32_t &baudRate, const uint32_t &moduleClk)
{
  float sbr = (float)(moduleClk / baudRate) / DIVISIOR; // using this formula to obtain SBR
                                                        // and typecast it into float type
  float sbr_Fraction = fmod(sbr, static_cast<int>(sbr)); // fmod will return reminder
                                                // using reminder to obtain fractional part of SBR

 return static_cast<uint8_t>(sbr_Fraction * 2 * DIVISIOR); //multiplying sbr_fracion by 32,
                                                      // and typecast it into uint8_t
}


bool UART_Init(const uint32_t &baudRate, const uint32_t &moduleClk)
{
  //local variable for storing SBR using union type
  uint16union_t sbr;

 // Baud rate = UART module clock / (16* (SBR[12:0]+BRFD) )

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

  return true;
}


 bool UART_InChar(uint8_t* const dataPtr)
{
 if(RxFIFO.FIFO_Get(dataPtr)) // retrieve data from FIFO and send it to Packet module
   return true;
}

 bool UART_OutChar(const uint8_t &data)
{
  if(TxFIFO.FIFO_Put(data)) // Packet module requires to send data to FIFO
   return true;
}


// UART2_S1, UART2_D --8bit mode  9th_bit is 0
void UART_Poll(void)
{
  // receiving data condition
  // To check the state of RDRF bit
 if(UART2_S1 & UART_S1_RDRF_MASK)
{
  // let the receiver to send a byte of data to RxFIFO
  RxFIFO.FIFO_Put(UART2_D);
}
// To check the state of TDRE bit
   else if(UART2_S1 & UART_S1_TDRE_MASK)
  {
  //send data from TxFIFO to data register
  TxFIFO.FIFO_Get((uint8_t *) &UART2_D);
  }

}
