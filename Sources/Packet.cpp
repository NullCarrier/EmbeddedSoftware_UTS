//to implement these functions in packet.h
#include "packet.h"

// define the static varibale
TPacket PacketVer2_t::s_Packet;

// define the global variable
const uint8_t PACKET_ACK_MASK = 0b10000000;

static bool PacketVer2_t::Packet_Get()
{	
  // local variable for holding temp value
  uint8_t rxData{0};
  static unsigned nbBytesPacket{0};

  while (nbBytesPacket <= 5)
  {
    //whenever the UART_Inchar has been called , incrementing  NbBytes_Packet
       if (UART_InChar(&rxData))
       {
         nbBytesPacket++;
            switch(nbBytes_Packet)
            {
			 case 0: Packet_Checksum = rxData;
			   break;
             case 1: Packet_Command = rxData; // assign data to Packet
        	   break;
             case 2: Packet_Parameter1 = rxData;
        	   break;
             case 3: Packet_Parameter2 = rxData;
		       break;
             case 4: Packet_Parameter3 = rxData;
        	   break;
             case 5:  
               if(CheckChecksum())
               {
                // checksum is good, then check it out
            	   nbBytesPacket++;
            	   return true;
            	   break;
               }
              else
              {
            // checksum is not good , then discarding first byte, going back to case 4
             this->Packet_SwitchPacket();
             nbBytesPacket--;
              }

          }

     }
       else
     {
       // must return false when the UART_InChar can't get a byte or
       // receiving section has done for packet module
       return false;
       break;
     }

 }
  if (nbBytesPacket > 5)
  {
	  nbBytesPacket = 0; // reset Nbbytes_packet
	  return false; // need to terminate receiving data
  }

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


static void PacketVer2_t::MakeChecksum()
{
  Packet_Checksum = Packet_Command^Packet_Parameter1^Packet_Parameter2^Packet_Parameter3;
}