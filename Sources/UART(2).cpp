#include "UART.h"
#include "MK70F12.h"// involve mask for all registers
#include "FIFO.h" // to use FIFO_Get(), FIFO_Put

// declare the global object called RxFIFO, TxFIFO
extern TFIFO RxFIFO;
extern TFIFO TxFIFO;

// const number for converting baudrate into SBR
static constexpr uint32_t DIVISIOR = 16;

// This function is only used to obtain BRFA
static uint8_t get_fraction(const uint32_t &baudRate, const uint32_t &moduleClk)
{
  float sbr = (moduleClk / baudRate) / DIVISIOR ;
  float sbr_Fraction = sbr % (static_cast<int> sbr) ;

 return static_cast<uint8_t> (sbr_Fraction * 2 *  DIVISIOR);
}
                                             //CPU_BUS_CLK_HZ
bool UART_Init(const uint32_t &baudRate, const uint32_t &moduleClk)
{
  //local variable for storing SBR using union type
  uint16union_t sbr = 0;

 // Baud rate = UART module clock / (16* (SBR[12:0]+BRFD) )
 // UART_BDH_SBR_MASK , UART_BDH_SBR(x)
 //Enable the clock module for UART2
 SIM_SCGC4 |= SIM_SCGC4_UART2_MASK;
 // Enable the clock for PortE
 SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;
 //  PIN multiplexing
 PORTE_PCR16 |= PORT_PCR_MUX(3);
 PORTE_PCR17 |= PORT_PCR_MUX(3);
// Write the integer portion of the Baudrate to the BDH/L register.
  sbr.l = (moduleClk / baudRate) / DIVISIOR ;
  UART2_BDH |= sbr.s.Hi;
  UART2_BDL |= sbr.s.Lo;
// Write the fractional portion of the Baudrate to the UART2_C4 register.
 UART2_C4 |= UART_C4_BRFA( get_fraction( baudRate, moduleClk)) ;
 // Enable the receiver, transmitter
 UART2_C2 |= (UART_C2_RE_MASK | UART_C2_TE_MASK);

 // To reset the RDRF bit
 UART2_S1 &= ~UART_S1_RDRF_MASK;
  return true;
}

inline bool UART_InChar(uint8_t* const dataPtr)
{
 if( RxFIFO.FIFO_Get(dataPtr) )
   return true;
}

inline bool UART_OutChar(const uint8_t &data)
{
  if( TxFIFO.FIFO_Put(data) )
   return true;
}


// UART2_S1, UART2_D --8bit mode  9th_bit is 0
void UART_Poll(void)
{
  // receiving data condition
  // To check the state of RDRF bit
 if(UART2_S1 & UART_S1_RDRF_MASK)
{
  // let the receiver to send a byte of data
  RxFIFO.FIFO_Put(*UART2_D);
}
// To check the state of TDRE bit
else if(UART2_S1 & UART_S1_TDRE_MASK)
{
  //let transmitter to send data
  TxFIFO.FIFO_Get(&UART2_D);
}

}
