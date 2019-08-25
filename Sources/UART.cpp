/*! @file UART.cpp
 *
 *  @brief I/O routines for UART communications on the TWR-K70F120M.
 *
 *  This contains the functions for operating the UART (serial port).
 *
 *  @author Chao Li
 *  @date 07/05/2019
 *  Copyright (c) Chao Li. All rights reserved.
 */

#include "UART.h"

// include fmod()
#include <cmath>

// const number for converting baudrate into SBR
const float DIVISIOR = 16.0;

static TFIFO TxFIFO;
static TFIFO RxFIFO;

static OS_ECB* TxfifoSemaphore;
static OS_ECB* RxfifoSemaphore;

static uint8_t RxData;


/*! @brief Calculate the fractional part of number
 *
 *         Baud rate = UART module clock / (16* (SBR[12:0]+BRFD) )
 *  @param baudRate The desired baud rate in bits/sec.
 *  @param moduleClk The module clock rate in Hz.
 *  @return uint8_t - return a fractional part for UART
 */
static uint8_t GetFraction(const uint32_t &baudRate, const uint32_t &moduleClk)
{
  float sbr = (moduleClk / baudRate) / DIVISIOR; // using this formula to obtain SBR
                                                        // and typecast it into float type
  float sbr_Fraction = fmod(sbr, static_cast<int>(sbr)); // fmod will return reminder
                                                // using reminder to obtain fractional part of SBR

 return (uint8_t) (sbr_Fraction * 2 * DIVISIOR); //multiplying sbr_fracion by 32,
                                                      // and typecast it into uint8_t
}


bool UART_t::Init() const
{
  critical section; //Enter critical section

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

  //To signal occurrence of one or more events
  TxfifoSemaphore = OS_SemaphoreCreate(0);
  RxfifoSemaphore = OS_SemaphoreCreate(0);

  //Create binary semaphore
  //ShareFIFOSemaphore = OS_SemaphoreCreate(0);

 //Set priority?
  UART2_C2 |= UART_C2_RIE_MASK; // Arm the receive interrupt

  return true;
}


bool UART_t::InChar(uint8_t &rxData)
{
  //OS_SemaphoreWait(ShareFIFOSemaphore, 0); // Wait for semaphore signalled by RxThread

  // retrieve data from FIFO and send it to Packet module
  return RxFIFO.Get(rxData);
}


 bool UART_t::OutChar(const uint8_t txData)
{
  return TxFIFO.Put(txData); // Packet module requires to send data to FIFO
}


 /*! @brief Receiving thread
      *
      *  @param pData might not use but is required by the OS to create a thread.
      *
      */
 void RxThread(void* pData)
{
  for (;;)
  {
    OS_SemaphoreWait(RxfifoSemaphore, 0); //suspend the thread until next time it has been siginified
    RxFIFO.Put(RxData); // let the receiver to send a byte of data to RxFIFO

   // OS_SemaphoreSignal(ShareFIFOSemaphore); //Release a remaphore
  }
}


/*! @brief Transimission thread
     *
     *  @param pData might not use but is required by the OS to create a thread.
     *
     */
 void TxThread(void* pData)
{
  for (;;)
  {
	OS_SemaphoreWait(TxfifoSemaphore, 0); //suspend the thread until next time it has been siginified

	uint8_t data = 0;

    TxFIFO.Get(data);

    OS_DisableInterrupts(); //Start critical section

	//UART2_C2 &= ~UART_C2_TIE_MASK; // Disarm the UART output

	UART2_D = data;

	UART2_C2 |= UART_C2_TIE_MASK;// Arm output device

	OS_EnableInterrupts();

  }

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




void __attribute__ ((interrupt)) UART_ISR(void)
{
  // inform RTOS that ISR is being processed
  OS ISR;

  // receiving data condition
   if (UART2_C2 & UART_C2_RIE_MASK)
   {
     if (UART2_S1 & UART_S1_RDRF_MASK) // To check the state of RDRF bit
     {
       RxData = UART2_D; //Clear RDRF bit
       OS_SemaphoreSignal(RxfifoSemaphore); // To signal an event
     }
   }

// Transmit a byte of data
   if (UART2_C2 & UART_C2_TIE_MASK)
   {
     if (UART2_S1 & UART_S1_TDRE_MASK)
       OS_SemaphoreSignal(TxfifoSemaphore); // To check any threads waiting on semaphore and make them ready to run
     UART2_C2 &= ~UART_C2_TIE_MASK; // Disarm the UART output
   }

}


