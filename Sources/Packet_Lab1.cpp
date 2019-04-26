/*! @file
 *
 *  @brief Routines to implement packet encoding and decoding for the serial port.
 *
 *  This contains the functions for implementing the "Tower to PC Protocol" 5-byte packets.
 *
 *  @author : Chao Li
 *  @date 25/04/2019
 *  Copyright (c) Chao Li. All rights reserved.
 */

#include "Packet(2).h"


Packet_t::Packet_t(const uint32_t baudRate, const uint32_t moduleClk):
 m_baudRate{baudRate}, m_moduleClk{moduleClk}
 {
  assert(UART_Init(m_baudRate, m_moduleClk));
 }

#if 0
// in the condition of receiving bad checksum
// need to discard first byte and add a new byte at the end of packet
void Packet_t::SwitchPacket()
{
    Packet_Command = Packet_Parameter1;
    Packet_Parameter1 = Packet_Parameter2;
    Packet_Parameter2 = Packet_Parameter3;
    Packet_Parameter3 = Packet_Checksum;
}

// this function is to obtain 5 bytes of data via UART_InChar()
bool Packet_t::Packet_Get(void)
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
            switch(nbBytesPacket)
            {
             case 1: Packet_Command = rxData; // assign data to Packet
        	   break;
             case 2: Packet_Parameter1 = rxData;
        	   break;
             case 3: Packet_Parameter2 = rxData;
		       break;
             case 4: Packet_Parameter3 = rxData;
        	   break;
             case 5:  Packet_Checksum = rxData;
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
             this->SwitchPacket();
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

// To send packet from tower to TxFIFO
bool Packet_t::Packet_Put()
{
 //After calling the handleGet(), tower has got valid packet
 // 5th byte is calculated by checksum
Packet_Checksum =  Packet_Command ^ Packet_Parameter1 ^ Packet_Parameter2 ^ Packet_Parameter3;
 // call UART_OutChar() 5times
	UART_OutChar(Packet_Command);
	UART_OutChar(Packet_Parameter1);
	UART_OutChar(Packet_Parameter2);
	UART_OutChar(Packet_Parameter3);
	UART_OutChar(Packet_Checksum);
  return true;
}


uint8_t& Packet_t::CheckChecksum()
{
Packet_Checksum = Packet_Command^Packet_Parameter1^Packet_Parameter2^Packet_Parameter3;
return Packet_Checksum;
}
#endif
