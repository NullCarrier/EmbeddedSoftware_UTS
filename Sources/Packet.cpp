/*! @file
 *
 *  @brief Routines to implement packet encoding and decoding for the serial port.
 *
 *  This contains the functions for implementing the "Tower to PC Protocol" 5-byte packets.
 *
 *  @author : Chao Li
 *  @date 02/04/2019
 *  Copyright (c) UTS. All rights reserved.
 */

#include "Packet(2).h"
#include <iostream> // std::cout, std::cerr

// in the condition of receiving bad checksum
// need to discard first byte and add a new byte at the end of packet
void Packet_t::Packet_SwitchPacket()
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
  static unsigned NbBytes_Packet{0};

  while(NbBytes_Packet <= 5)
 {
    //whenever the UART_Inchar has been called , incrementing  NbBytes_Packet
       if (UART_InChar(&rxData))
     {
         NbBytes_Packet++;
            switch(NbBytes_Packet)
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
               if(Packet_CheckChecksum())
             {
                // checksum is good, then check it out
            	   NbBytes_Packet++;
            	   return true;
            	   break;
             }
              else
            {
            // checksum is not good , then discarding first byte, going back to case 4
             this->Packet_SwitchPacket();
             NbBytes_Packet--;
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
  if(NbBytes_Packet > 5)
  {
	  NbBytes_Packet = 0; // reset Nbbytes_packet
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

// To handle all of conditions regarding to send packets
// this function only can send ACK command packet for startup
// future improvement will address the issue of sending ACK command for other commands
 void Packet_HandlePacket(Packet_t &packet)
{
   if(packet.Packet_Command & PACKET_ACK_MASK)
  {
    Packet_t Ack_Packet(packet);// initialize Ack_Packet for startup packet

    // send 3 packets for startup command and acknowledgement packet
    packet.Packet_HandleStartupPacket();
    packet.Packet_HandleTowerVersionPacket();
    packet.Packet_HandleTowerNumberPacket();
    Ack_Packet.Packet_Put();
  }
 else
  packet.Packet_HandleCommandPacket();
}

void Packet_t::Packet_HandleStartupPacket()
{
 // Assgin value for startup command according to packet protocol
  Packet_Command = CMD_STARTUP;
  Packet_Parameter1 = Packet_Parameter2 = Packet_Parameter3 = 0;

  this->Packet_Put(); //send it to FIFO
}

void Packet_t::Packet_HandleTowerVersionPacket()
{
  // Assgin value for towerversion command according to packet protocol
    Packet_Command = CMD_TOWERVERSION;
	Packet_Parameter1 = 0x76; // Parameter 1,//Command: Tower Version: v1.0
	Packet_Parameter2 = 0x01; // Parameter 2
	Packet_Parameter3 = 0x0;  // Parameter 3

    this->Packet_Put();// send it to FIFO
}

void Packet_t::Packet_HandleTowerNumberPacket()
{
// Assgin value for towernumber command according to packet protocol
    Packet_Command = CMD_TOWERNUMBER;
	Packet_Parameter1 = 0x01;  // Parameter 1
	Packet_Parameter2 = 0x94; // Parameter 2
	Packet_Parameter3 = 0x34; // Parameter 3

	this->Packet_Put(); // send it to FIFO

}

// Handling packet protocol (Tower to PC)
int Packet_t::Packet_HandleCommandPacket()
{
    switch(Packet_Command)
  {
  // for specific command. Startup needs to send 3 packets
    case CMD_STARTUP:  this->Packet_HandleStartupPacket();
                       this->Packet_HandleTowerVersionPacket();
                       this->Packet_HandleTowerNumberPacket();
                      break;
    case CMD_TOWERVERSION: this->Packet_HandleTowerVersionPacket();//only responce once for version

    break;
    case CMD_TOWERNUMBER: this->Packet_HandleTowerNumberPacket();//only responce once for number
   // Error condition
   default: std::cout << "Error: Can't identify the command\n";
            return EXIT_FAILURE;
  }

}

// initializing Ackpacket for towerstartup command
Packet_t::Packet_t(const Packet_t &packet):
Packet_Command(packet.Packet_Command),
Packet_Parameter1(packet.Packet_Parameter1),
Packet_Parameter2(packet.Packet_Parameter2),
Packet_Parameter3(packet.Packet_Parameter3),
m_baudRate(packet.m_baudRate), m_moduleClk(packet.m_moduleClk)
  {
    Packet_Command |= PACKET_ACK_MASK;
    Packet_Command |= CMD_STARTUP;
  }

// constructor for initializing UART module
Packet_t::Packet_t(const uint32_t baudRate, const uint32_t moduleClk):
     m_baudRate{baudRate}, m_moduleClk{moduleClk}
   {
      UART_Init(m_baudRate, m_moduleClk);
   }






