// implement functions: Packet_Get, Packet_put
#include "packet.h"

 //implment FSM
bool Packet_t::Packet_Get(void)
{
 unsigned NbBytes_Packet = 0;


 while( ++NbBytes_Packet <= 5)
{
  //whenever the UART_Inchar has been called , incrementing  NbBytes_Packet
  if ( UART_InChar(&UART2_D) )
 {
        switch(NbBytes_Packet)
      {
         case 1: Packet_Command = *UART2_D;
            break;
         case 2: Packet_Parameter1 = *UART2_D;
             break;
         case 3: Packet_Parameter2 = *UART2_D;
             break;
         case 4: Packet_Parameter3 = *UART2_D;
              break;
         case 5: Packet_Checksum = *UART2_D;
  if(Packet_Command ^ Packet_Parameter1 ^ Packet_Parameter2 ^ Packet_Parameter3 != Packet_Checksum)
 {
    NbBytes_Packet--;
    break;
 }
 else
 NbBytes_Packet++;
       }

 }
}

return true;
}

// To send packet from tower to PC
bool Packet_t::Packed_Put()
{
 //After calling the handlePacket(), tower has got valid packet
 // 5th byte is calculated by checksum
 Packet_Checksum = Packet_Command ^ Packet_Parameter1 ^ Packet_Parameter2 ^ Packet_Parameter3;
// call UART_OutChar() 5times
 UART_OutChar(Packet_Command);
 UART_OutChar(Packet_Parameter1);
 UART_OutChar(Packet_Parameter2);
 UART_OutChar(Packet_Parameter3);
 UART_OutChar(Packet_Checksum);

return true;
}
