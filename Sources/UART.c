#include "UART.h"
#include "MK70F12.h"// involve mask for RDRF,TDRE, RXRTSE
#include "Cpu.h"
#include "FIFO.c"

// declare the global var called RxFIFO, TxFIFO
extern TFIFO RxFIFO;
extern TFIFO TxFIFO;

bool UART_Init(const uint32_t baudRate, const uint32_t moduleClk)
{

 // Baud rate = UART module clock / (16* (SBR[12:0]+BRFD) )
 // UART_BDH_SBR_MASK , UART_BDH_SBR(x)
 //Enable the clock module for UART2
 SIM_SCGC4 |= SIM_SCGC4_UART2_MASK;
 // Enable the clock for PortF[14:11]
 SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;
// Write the integer/fractional portion of the Baudrate to the BDH/L register.
 (moduleClk / baudRate) / (uint32_t)16 ;
 UART2_BDH |= ;

/* UART2_BDL |=*/

 // Enable the receiver, transmitter
 UART2_C2 |= (UART_C2_RE_MASK | UART_C2_TE_MASK);

 // To reset the RDRF bit
 UART2_S1 &= ~ UART_S1_RDRF_MASK;
  return true;
}

bool UART_InChar(uint8_t* const dataPtr)
{
 if( FIFO_Get(&RxFIFO, *dataPtr) )
   return true;
}

bool UART_OutChar(const uint8_t data)
{
  if( FIFO_Put(&TxFIFO, data) )
   return true;
}


void UART_Poll(void)
{ // receiving data condition
  // UART2_S1, UART2_D --8bit mode  9th_bit is 0
  // To check the state of RDRF
 if(UART2_S1 & UART_S1_RDRF_MASK)
{
  //set RXRTSE after determining RDRF is set
  // enable the receiver to send a byte of data, ?: UART2_S2
 UART2_MODEM |= UART_MODEM_RXRTSE_MASK;
 FIFO_Put(&RxFIFO, *UART2_D_REG(base) );
}
// send data situation
else if(UART2_S1 & UART_S1_TDRE_MASK)
{
  //enable transmitter to send data
  UART2_MODEM |= UART_MODEM_TXRTSE_MASK;
  FIFO_Get(&TxFIFO, UART2_D_REG(base) );
}

//? UART2_MODEM |= UART_MODEM_TXCTSE;

}


