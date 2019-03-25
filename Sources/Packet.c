#include "packet.h"
#include "UART.c"


bool Packet_Init(const uint32_t baudRate, const uint32_t moduleClk)
{
  UART_Init(baudRate, moduleClk);
  
  return true;
}

 /* need to call UART_intchar 5times */
 //implment FSM
bool Packet_Get(void)
{
 unsigned nbBytes_Packet = 0;

 
 while( nbBytes_Packet <= 5)
{
  //whenever the UART_Inchar has been called , incrementing  NbBytes_Packet
  if ( UART_InChar( &UART2_D ) )
 {     
      NbBytes_Packet++;
        switch(nbBytes_Packet) 
      {
         case 0:  break;
         case 1:  break;
         case 2:  break;
         case 3:  break;
         case 4:  break;
       /*  case 5:  if(checkSum is good)
            // reset state
             nbBytes_Packet = 0; */
       }

 }

}

return true;
}

// call UART_OutChar() 5times
// 5th byte is calculated by checksum
bool Packed_Put(const uint8_t command, const uint8_t parameter1, const uint8_t parameter2, const uint8_t parameter3)
{
   


return true;
}





