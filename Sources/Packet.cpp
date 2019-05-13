/*! @file packet.cpp
 *
 *  @brief Routines to implement packet encoding and decoding for the serial port.
 *
 *  This contains the functions for implementing the "Tower to PC Protocol" 5-byte packets.
 *
 *  @author : Chao Li
 *  @date 07/05/2019
 *  Copyright (c) Chao Li. All rights reserved.
 */

//to implement these functions in packet.h
#include "packet.h"

// define the static varibale
TPacket Packet_t::s_Packet;

// define the global variable
const uint8_t PACKET_ACK_MASK = 0b10000000;

 bool Packet_t::PacketGet()
{
  // local variable for holding temp value
 uint8_t rxData{0};
 static unsigned nbBytesPacket{1};

    //whenever the UART_Inchar has been called , incrementing  NbBytes_Packet
 if (UART_InChar(&rxData))
 {
  switch (nbBytesPacket)
  {
   case 1: Packet_Command = rxData; // assign data to Packet
            nbBytesPacket++;
            break;
   case 2: Packet_Parameter1 = rxData;
            nbBytesPacket++;
            break;
   case 3: Packet_Parameter2 = rxData;
            nbBytesPacket++;
            break;
   case 4: Packet_Parameter3 = rxData;
            nbBytesPacket++;
            break;
   case 5: Packet_Checksum = rxData;
    if (Packet_Checksum == MakeChecksum())
    {
      // checksum is good, then check it out
      nbBytesPacket = 1;
      return true;
    }
    else
    {
     // checksum is not good , then discarding first byte, going back to case 4
      SwitchPacket();
      nbBytesPacket--;
    }

   }

  }

  return false;
}


 bool Packet_t::PacketPut()
{
 Packet_Checksum = MakeChecksum();

 return UART_OutChar(Packet_Command)&& UART_OutChar(Packet_Parameter1)&&
 UART_OutChar(Packet_Parameter2)&& UART_OutChar(Packet_Parameter3)&& UART_OutChar(Packet_Checksum);

}


 void Packet_t::SwitchPacket()
 {
   EnterCritical(); //Start critical section

   Packet_Command = Packet_Parameter1;
   Packet_Parameter1 = Packet_Parameter2;
   Packet_Parameter2 = Packet_Parameter3;
   Packet_Parameter3 = Packet_Checksum;

   ExitCritical(); //End critical section
 }


 uint8_t Packet_t::MakeChecksum()
{
  return Packet_Command^Packet_Parameter1^Packet_Parameter2^Packet_Parameter3;
}


