#include "UART.h"
#include "MK70F12.h"// involve mask for RDRF,TDRE, RXRTSE
#include "Cpu.h"
#include "FIFO.c"

// declare the global var called RxFIFO, TxFIFO 
extern TFIFO RxFIFO;
extern TFIFO TxFIFO;
                                             //CPU_BUS_CLK_HZ
bool UART_Init(const uint32_t baudRate, const uint32_t moduleClk)
{
  uint16union_t sbr;
  
 // Baud rate = UART module clock / (16* (SBR[12:0]+BRFD) )
 // UART_BDH_SBR_MASK , UART_BDH_SBR(x)
 //Enable the clock module for UART2
 SIM_SCGC4 |= SIM_SCGC4_UART2_MASK;
 // Enable the clock for PortE
 SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;
 // Mux signal (ALT3)
   
// Write the integer portion of the Baudrate to the BDH/L register.
  sbr.l = (moduleClk / baudRate) / (uint32_t)16 ;
  UART2_BDL |= sbr.s.Lo;
// Write the frantional portion of the Baudrate to the BDH/L register.
   UART2_C4 = UART_C4_BRFA(4) ; // get remainder
 // Enable the receiver, transmitter
 UART2_C2 |= (UART_C2_RE_MASK | UART_C2_TE_MASK);

 // To reset the RDRF bit
 UART2_S1 &= ~UART_S1_RDRF_MASK;
  return true;
}

bool UART_InChar(uint8_t* const dataPtr)
{
 if( FIFO_Get(&RxFIFO, dataPtr) )
   return true;
}

bool UART_OutChar(const uint8_t data)
{
  if( FIFO_Put(&TxFIFO, data) )
   return true;
}

// need to do the signal muxing?
// UART2_S1, UART2_D --8bit mode  9th_bit is 0

void UART_Poll(void)
{ 
  // receiving data condition
  // To check the state of RDRF
 if(UART2_S1 & UART_S1_RDRF_MASK)
{
  // enable the receiver to send a byte of data, ?: UART2_S2
 FIFO_Put(&RxFIFO, *UART2_D);
 // UART_InChar(&UART2_D);
}
// send data condition
else if(UART2_S1 & UART_S1_TDRE_MASK)
{
  //enable transmitter to send data
  // UART_OutChar( *UART2_D_REG(base) );
  
  FIFO_Get(&TxFIFO, &UART2_D);
 // UART2_MODEM |= UART_MODEM_TXRTSE_MASK;
}

}


