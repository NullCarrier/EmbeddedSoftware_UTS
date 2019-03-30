// implement functions: Packet_Get, Packet_put
#include "Packet(2).h"
#include "UART(2).h"

// constant for 3 handling cases
#define  CMD_STARTUP 0x04
#define  CMD_TOWERVERSION 0x09
#define  CMD_TOWERNUMBER 0x0B

bool Packet_t::Packet_Get(void)
{
  // local variable for holding temp value
  uint8_t rxData = 0;
 unsigned NbBytes_Packet = 0;

  while( NbBytes_Packet <= 5)
{
    //whenever the UART_Inchar has been called , incrementing  NbBytes_Packet
    if ( UART_InChar( &rxData ) )
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
               if( Check_Checksum() )
             {
            	   NbBytes_Packet++;
            	   break;
             }
              else
            {
              Packet_Command = Packet_Parameter1;
              Packet_Parameter1 = Packet_Parameter2;
              Packet_Parameter2 = Packet_Parameter3;
              Packet_Parameter3 = Packet_Checksum;
             NbBytes_Packet--;
            }

        }

   }

 }

return true;
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

#if 0
void Packet_t::HandlePacket()
{
  if( Packet_Command & PACKET_ACK_MASK )
 {
    if( HandleCommandPacket() )
   Packet_Command |= PACKET_ACK_MASK;
    else
  // to indicate the process of packet is failed
   Packet_Command &= ~PACKET_ACK_MASK;
 }
 else
 {
  HandleCommandPacket();
 }



}
#endif

void Packet_t::HandleStartupPacket()
{
	Packet_Parameter1 = Packet_Parameter2 = Packet_Parameter3 = 0x0;
}


void Packet_t::HandleTowerVersionPacket()
{
 // tower version: v1.0

   // Parameter 1
	Packet_Parameter1 = 0x76;
   // Parameter 2
	Packet_Parameter2 = 0x01;
   // Parameter 3
	Packet_Parameter3 = 0x0;
}

void Packet_t::HandleTowerNumberPacket()
{

  // Parameter 1
	Packet_Parameter1  = 0x01;
  // Parameter 2
	Packet_Parameter2 = 0x0;
  // Parameter 3
	Packet_Parameter3 = 0x01;
}

// Handling packet protocol (Tower to PC)
bool Packet_t::HandleCommandPacket()
{

 switch(Packet_Command)
{
  // for specific command
   case CMD_STARTUP: HandleStartupPacket();
    break;
   case CMD_TOWERVERSION: HandleTowerVersionPacket();
    break;
   case CMD_TOWERNUMBER: HandleTowerNumberPacket();
  // default: error;
}
return true;
}


