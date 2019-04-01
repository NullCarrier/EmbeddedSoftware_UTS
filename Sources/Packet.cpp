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



void Packet_t::Packet_SwitchPacket()
{
    Packet_Command = Packet_Parameter1;
    Packet_Parameter1 = Packet_Parameter2;
    Packet_Parameter2 = Packet_Parameter3;
    Packet_Parameter3 = Packet_Checksum;
}

// this function is to obtain 5 bytes of data via UART_InChar()
bool Packet_t::Packet_Packet_Get(void)
{
  // local variable for holding temp value
  uint8_t rxData{0};
  static unsigned NbBytes_Packet{0};

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
               if(Packet_CheckChecksum())
             {
            	   NbBytes_Packet++;
            	   return true;
            	   break;
             }
              else
            {
            // checksum is not good , then discarding first byte, going back to state 4
             this->Packet_SwitchPacket();
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
  {
	  NbBytes_Packet = 0;
	  return false;
  }

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

 void HandleACKPacket(Packet_t &packet)
{
  Packet_t Ack_Packet(packet);

  if(Packet_Command & PACKET_ACK_MASK)
     Ack_Packet.Packet_Put;
}


void Packet_t::Packet_HandleStartupPacket()
{
  Packet_Command = 0x04;
  Packet_Parameter1 = Packet_Parameter2 = Packet_Parameter3 = 0x0;
  this->Packet_Put();
}

//Command: Tower Version: v1.0
void Packet_t::Packet_HandleTowerVersionPacket()
{

    Packet_Command = 0x09;
	Packet_Parameter1 = 0x76; // Parameter 1

	Packet_Parameter2 = 0x01; // Parameter 2

	Packet_Parameter3 = 0x0;  // Parameter 3

	this->Packet_Put(); // output 5 bytes
}

void Packet_t::Packet_HandleTowerNumberPacket()
{
    Packet_Command = 0x0B;
	Packet_Parameter1  = 0x01;  // Parameter 1

	Packet_Parameter2 = 0x94; // Parameter 2

	Packet_Parameter3 = 0x34; // Parameter 3

	this->Packet_Put();
}

// Handling packet protocol (Tower to PC)
bool Packet_t::Packet_HandlePacket()
{
    switch(Packet_Command)
  {
  // for specific command
    case CMD_STARTUP:
	                 this->Packet_HandleStartupPacket();
                     this->Packet_HandleTowerVersionPacket();
                     this->Packet_HandleTowerNumberPacket();
                     HandleACKPacket(*this);
    break;
    case CMD_TOWERVERSION: this->Packet_HandleStartupPacket();
                           HandleACKPacket(*this);
    break;
    case CMD_TOWERNUMBER: this->Packet_HandleTowerNumberPacket();
                           HandleACKPacket(*this);
   //default: ;
  }

 return true;
}
