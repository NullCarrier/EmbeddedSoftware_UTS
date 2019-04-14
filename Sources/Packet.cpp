//to implement these functions in packet.h
#include "packet.h"

// define the static varibale
TPacket PacketVer2_t::s_Packet;
// define the global variable
const uint8_t PACKET_ACK_MASK = 0b10000000;

static bool PacketVer2_t::Packet_Get()
{
  	
	
	
	
	
	
	
		
	
}

static bool PacketVer2_t::Packet_Put()
{
  Packet_Checksum = MakeChecksum();

 return UART_OutChar(Packet_Command)&& UART_OutChar(Packet_Parameter1)&& UART_OutChar(Packet_Parameter2)&& UART_OutChar(Packet_Parameter3)&& UART_OutChar(Packet_Checksum);
}

static void PacketVer2_t::SwitchPacket()
{
	
 while (!s_Packet.bytes.empty() )
 { 
    uint8_t newByte = 0;
  s_Packet.bytes.pop_front(); //delete first element of bytes
  
  s_Packet.bytes.push_back (newByte); // add a new byte 
 }
 
}


static void HandlePacketVer2::MakeChecksum()
{
  Packet_Checksum = Packet_Command^Packet_Parameter1^Packet_Parameter2^Packet_Parameter3;
}