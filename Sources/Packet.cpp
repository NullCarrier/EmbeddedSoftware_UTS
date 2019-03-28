// implement functions: Packet_Get, Packet_put
#include "packet.h"

 //implment FSM
bool Packet_t::Packet_Get(void)
{

  for(auto &frame : RxPacket)
{
 // need error checking
     UART_Inchar(&frame);  	
}

  while( !Check_Checksum() )
{
  uint8_t new_coming_byte = 0;

  RxPacket.pop_front();
  RxPacket.push_back( new_coming_byte );

  UART_Inchar(&new_coming_byte);
}

return true;
}

// To send packet from tower to PC
bool Packet_t::Packed_Put()
{
 //After calling the handlePacket(), tower has got valid packet
 // 5th byte is calculated by checksum
 RxPacket[4] = Packet_Command ^ Packet_Parameter1 ^ Packet_Parameter2 ^ Packet_Parameter3;
 // call UART_OutChar() 5times
 for(auto send_packet : RxPacket)
{
 // need to check error
  UART_Outchar(send_packet);
}

return true;
}
