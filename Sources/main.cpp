/* ###################################################################
**     Filename    : main.c
**     Project     : Lab1
**     Processor   : MK70FN1M0VMJ12
**     Version     : Driver 01.01
**     Compiler    : GNU C Compiler
**     Date/Time   : 2015-07-20, 13:27, # CodeGen: 0
**     Abstract    :
**         Main module.
**         This module contains user's application code.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/*!
** @file main.c
** @version 2.0
** @brief
**         Main module.
**         This module contains user's application code.
**  Copyright (c) UTS. All rights reserved.
*/
/*!
**  @addtogroup main_module main module documentation
**  @{
*/
// CPU module - contains low level hardware initialization routines
#include "Cpu.h"

//include packet module
#include "packet.h"

#include "LEDs.h"

#include "Flash.h"

#include "PIT.h"

const uint64_t BAUDRATE = 115200;
/* MODULE main */

void HandlePacketVer2::HandleStartupPacket(PacketVer2_t &packet)
{
 // Assgin value for startup command according to packet protocol

  Packet_Parameter1 = Packet_Parameter2 = Packet_Parameter3 = 0;

  packet.PacketVer2_t::Packet_Put(); //send it to FIFO

}

void HandlePacketVer2::HandleTowerVersionPacket(PacketVer2_t &packet)
{
  // Assgin value for towerversion command according to packet protocol

   Packet_Parameter1 = 0x76; // Parameter 1,//Command: Tower Version: v1.0
   Packet_Parameter2 = 0x01; // Parameter 2
   Packet_Parameter3 = 0x0;  // Parameter 3

   packet.PacketVer2_t::Packet_Put(); // send it to FIFO
}

void HandlePacketVer2::HandleTowerNumberPacket(PacketVer2_t &packet)
{
// Assgin value for towernumber command according to packet protocol

	Packet_Parameter1 = 0x01;  // Parameter 1
	Packet_Parameter2 = 0x94; // Parameter 2
	Packet_Parameter3 = 0x34; // Parameter 3

	packet.PacketVer2_t::Packet_Put(); //send it to FIFO

}

void HandlePacketVer2::HandleTowerModePacket(PacketVer2_t &packet)
{

  	Packet_Parameter1 = 0x01;  // Parameter 1
  	Packet_Parameter2 = 0x01; // Parameter 2
  	Packet_Parameter3 = 0x00; // Parameter 3

  	packet.PacketVer2_t::Packet_Put(); //send it to FIFO
}


// Handling packet protocol (Tower to PC)
void HandlePacketVer2::HandleCommandPacket(PacketVer2_t &packet)
{
    switch (Packet_Command)
    {
  // for specific command. Startup needs to send 3 packets
    case CMD_STARTUP: InitResponsePacket(packet);
                      break;
	case CMD_ACK_STARTUP: HandleACKStartupPacket(packet);
	                      break;
    case CMD_TOWERVERSION: HandleTowerVersionPacket(packet);//only responce once for version
                           break;
	case CMD_ACK_TOWERVERSION: HandleACKTowerVersionPacket(packet);
	                           break;
    case CMD_TOWERNUMBER: HandleTowerNumberPacket(packet);//only responce once for number
                          break;
	case CMD_ACK_TOWERNUMBER: HandleACKTowerNumberPacket(packet);
	                          break;
    case CMD_TOWERMODE: HandleTowerModePacket(packet);
                        break;
	case CMD_ACK_TOWERMODE: HandleACKTowerModePacket(packet);
	                        break;
    }

}

void HandlePacketVer2::InitResponsePacket(PacketVer2_t &packet)
{
  // Send tower startup packet
  HandleStartupPacket(packet);

  // Send tower version packet
  Packet_Command = HandlePacketVer2::CMD_TOWERVERSION;
  HandleTowerVersionPacket(packet);

  // Send tower number packet
  Packet_Command = HandlePacketVer2::CMD_TOWERNUMBER;
  HandleTowerNumberPacket(packet);

  // Send tower mode packet
  Packet_Command = HandlePacketVer2::CMD_TOWERMODE;
  HandleTowerModePacket(packet);
}

void HandlePacketVer2::HandleACKStartupPacket(PacketVer2_t &packet)
{
  // Send tower startup packet
  Packet_Command = HandlePacketVer2::CMD_STARTUP;
  InitResponsePacket(packet);

  // Send ack tower startup packet
  Packet_Command = HandlePacketVer2::CMD_ACK_STARTUP; // to modify the packet command ID
  HandleStartupPacket(packet); // to send ack pakcet
}

void HandlePacketVer2::HandleACKTowerVersionPacket(PacketVer2_t &packet)
{
  // Send tower version packet
  Packet_Command = HandlePacketVer2::CMD_TOWERVERSION;

  HandleTowerVersionPacket(packet);

  // Send ack tower version packet
  Packet_Command = HandlePacketVer2::CMD_ACK_TOWERVERSION;

  HandleTowerVersionPacket(packet);
}

void HandlePacketVer2::HandleACKTowerNumberPacket(PacketVer2_t &packet)
{
  // Send tower number packet
  Packet_Command = HandlePacketVer2::CMD_TOWERNUMBER;

  HandleTowerVersionPacket(packet);

  // Send ack tower version packet
  Packet_Command = HandlePacketVer2::CMD_ACK_TOWERNUMBER;

  HandleTowerNumberPacket(packet);
}

void HandlePacketVer2::HandleACKTowerModePacket(PacketVer2_t &packet)
{
  // Send tower mode packet
  Packet_Command = HandlePacketVer2::CMD_TOWERMODE;

  HandleTowerVersionPacket(packet);

  // Send ack tower mode packet
  Packet_Command = HandlePacketVer2::CMD_ACK_TOWERMODE;

  HandleTowerModePacket(packet);
}

/*! @brief Overload function of InitResponsePacket
 *
 *  @param packet the PacketVert2 object
 *  @param Parameter2 the reference to hold the vaule read from flash memory
 *  @param Parameter3 the reference to hold the vaule read from flash memory
 */
static void InitResponsePacket(PacketVer2_t &packet, volatile uint8_t &Parameter2, volatile uint8_t &Parameter3)
{
  // Send tower startup packet
  Packet_Command = HandlePacketVer2::CMD_STARTUP;
  HandlePacketVer2::HandleStartupPacket(packet);

  // Send tower version packet
  Packet_Command = HandlePacketVer2::CMD_TOWERVERSION;
  HandlePacketVer2::HandleTowerVersionPacket(packet);

  // Send tower number packet
  Packet_Command = HandlePacketVer2::CMD_TOWERNUMBER;
  Packet_Parameter1 = 0x01;
  Packet_Parameter2 = Parameter2;
  Packet_Parameter3 = Parameter3;
  packet.PacketVer2_t::Packet_Put(); //send it to FIFO

  // Send tower mode packet
  Packet_Command = HandlePacketVer2::CMD_TOWERMODE;
  HandlePacketVer2::HandleTowerModePacket(packet);
}

//function description
void PITCallback(void* argu)
{
  LED_t led(LED_t::LED_GREEN);
  led.LEDs_Toggle();
}

/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
  /* Write your local variable definition here */

  PacketVer2_t packet(BAUDRATE, CPU_BUS_CLK_HZ); // initialize the packet obejct
/*
  PIT_t pit(CPU_BUS_CLK_HZ, PITCallback, 0); // initialize PIT module
  pit.PIT_Set(500); // period 500ms
*/
   __DI();//Disable interrupt

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/

  __EI(); //Enable the interrupt
  /* Write your code here */
  for (;;)
  {
    if ( packet.PacketVer2_t::Packet_Get())
    HandlePacketVer2::HandleCommandPacket(packet);

    UART_ISR();
  }

  /*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of RTOS startup code.  ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;){}
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

/* END main */
/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.5 [05.21]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/












