/*! @file
 *
 *  @brief Routines to implement packet encoding and decoding for the serial port.
 *
 *  This contains the functions for implementing the "Tower to PC Protocol" 5-byte packets.
 *
 *  @author : PMcl
 *  @date 01/04/2019
 *  Copyright (c) UTS. All rights reserved.
 */

#include "Packet(2).h"

// constant for handling 3 commands
#define  CMD_STARTUP 0x04
#define  CMD_TOWERVERSION 0x09
#define  CMD_TOWERNUMBER 0x0B

// this function will return true only if checksum is good
inline bool Check_Checksum()
{
 return Packet_Checksum ==Packet_Command^Packet_Parameter1^Packet_Parameter2^Packet_Parameter3;
}

void Packet_t::switch_packet()
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
  uint8_t rxData = 0;
static unsigned NbBytes_Packet = 0;

  while(NbBytes_Packet <= 5)
 {
    //whenever the UART_Inchar has been called , incrementing  NbBytes_Packet
       if (UART_InChar(&rxData))
     {
         NbBytes_Packet++;
            switch(NbBytes_Packet)
          {
             case 1: Packet_Command = rxData;
        	   break;
             case 2: Packet_Parameter1 = rxData;
        	   break;
             case 3: Packet_Parameter2 = rxData;
		       break;
             case 4: Packet_Parameter3 = rxData;
        	   break;
             case 5:  Packet_Checksum = rxData;
               if(Check_Checksum())
             {
            	   NbBytes_Packet++;
            	   return true;
            	   break;
             }
              else
            {
            // checksum is not good , then discarding first byte, going back to state 4
             this->switch_packet();
             NbBytes_Packet--;
            }

          }

     }
       else
     {
       return false;
       break;
     }

 }
// reset Nbbytes_packet
  if(NbBytes_Packet > 5)
   NbBytes_Packet =0;

}

// To send packet from tower to PC
bool Packet_t::Packet_Put()
{
 //After calling the handlePacket(), tower has got valid packet
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

/*
void Packet_t::HandlePacket()
{
  if(Packet_Command & PACKET_ACK_MASK)
 {
    if(this->HandleCommandPacket())
   Packet_Command |= PACKET_ACK_MASK;
    else
  // to indicate the process of packet is failed
   Packet_Command &= ~PACKET_ACK_MASK;
 }
 else
this->HandleCommandPacket();

}
*/

void Packet_t::HandleStartupPacket()
{
  Packet_Parameter1 = Packet_Parameter2 = Packet_Parameter3 = 0x0;
}

//Command: Tower Version: v1.0
void Packet_t::HandleTowerVersionPacket()
{

	Packet_Parameter1 = 0x76; // Parameter 1

	Packet_Parameter2 = 0x01; // Parameter 2

	Packet_Parameter3 = 0x0;  // Parameter 3
}

void Packet_t::HandleTowerNumberPacket()
{
	Packet_Parameter1  = 0x01;  // Parameter 1

	Packet_Parameter2 = 0x0; // Parameter 2

	Packet_Parameter3 = 0x01; // Parameter 3
}

// Handling packet protocol (Tower to PC)
bool Packet_t::HandleCommandPacket()
{

 switch(Packet_Command)
{
  // for specific command
   case CMD_STARTUP: this->HandleStartupPacket();
    break;
   case CMD_TOWERVERSION: this->HandleTowerVersionPacket();
    break;
   case CMD_TOWERNUMBER: this->HandleTowerNumberPacket();
  // default: error;
}
 this->Packet_Put(); // output 5 packets
 return true;
}


