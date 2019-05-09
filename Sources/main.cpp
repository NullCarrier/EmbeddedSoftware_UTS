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
** @file main.cpp
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

#include "RTC.h"

const uint64_t BAUDRATE = 115200;
/* MODULE main */

void HandlePacket::HandleStartupPacket(Packet_t &packet)
{
 // Assgin value for startup command according to packet protocol
  Packet_Parameter1 = Packet_Parameter2 = Packet_Parameter3 = 0;

  packet.Packet_t::PacketPut(); //send it to FIFO

}

void HandlePacket::HandleTowerVersionPacket(Packet_t &packet)
{
  // Assgin value for towerversion command according to packet protocol

  Packet_Parameter1 = 0x76; // Parameter 1,//Command: Tower Version: v1.0
  Packet_Parameter2 = 0x01; // Parameter 2
  Packet_Parameter3 = 0x0;  // Parameter 3

  packet.Packet_t::PacketPut(); // send it to FIFO
}

void HandlePacket::HandleTowerNumberPacket(Packet_t &packet)
{
// Assgin value for towernumber command according to packet protocol

  Packet_Parameter1 = 0x01;  // Parameter 1
  Packet_Parameter2 = 0x94; // Parameter 2
  Packet_Parameter3 = 0x34; // Parameter 3

  packet.Packet_t::PacketPut(); //send it to FIFO

}

void HandlePacket::HandleTowerModePacket(Packet_t &packet)
{
  Packet_Parameter1 = 0x01;  // Parameter 1
  Packet_Parameter2 = 0x01; // Parameter 2
  Packet_Parameter3 = 0x00; // Parameter 3

  packet.Packet_t::PacketPut(); //send it to FIFO
}

void HandlePacket::SetTimePacket(Packet_t &packet)
{
 RTC::RTC_t rtc();

 rtc.RTC_Set(Packet_Parameter1, Packet_Parameter2, Packet_Parameter3);

 packet.Packet_t::PacketPut(); //send it to FIFO
}

// Handling packet protocol (Tower to PC)
void HandlePacket::HandleCommandPacket(Packet_t &packet)
{
    switch (Packet_Command){

  // for specific command. Startup needs to send 3 packets
    case CMD_STARTUP: InitResponsePacket(Packet);
                      break;
	case CMD_ACK_STARTUP: HandleACKStartupPacket(Packet);
	                      break;
    case CMD_TOWERVERSION: HandleTowerVersionPacket(Packet);//only responce once for version
                           break;
	case CMD_ACK_TOWERVERSION: HandleACKTowerVersionPacket(Packet);
	                           break;
    case CMD_TOWERNUMBER: HandleTowerNumberPacket(Packet);//only responce once for number
                          break;
	case CMD_ACK_TOWERNUMBER: HandleACKTowerNumberPacket(Packet);
	                          break;
    case CMD_TOWERMODE: HandleTowerModePacket(Packet);
                        break;
	case CMD_ACK_TOWERMODE: HandleACKTowerModePacket(Packet);
	                        break;
	case CMD_SETTIME: SetTimePacket(Packet);
                      break;
    }

}

void HandlePacket::InitResponsePacket(Packet_t &packet)
{
  // Send tower startup packet
  HandleStartupPacket(packet);

  // Send tower version packet
  Packet_Command = HandlePacket::CMD_TOWERVERSION;
  HandleTowerVersionPacket(packet);

  // Send tower number packet
  Packet_Command = HandlePacket::CMD_TOWERNUMBER;
  HandleTowerNumberPacket(packet);

  // Send tower mode packet
  Packet_Command = HandlePacket::CMD_TOWERMODE;
  HandleTowerModePacket(packet);
}

void HandlePacket::HandleACKStartupPacket(Packet_t &packet)
{
  // Send tower startup packet
  Packet_Command = HandlePacket::CMD_STARTUP;
  InitResponsePacket(packet);

  // Send ack tower startup packet
  Packet_Command = HandlePacket::CMD_ACK_STARTUP; // to modify the packet command ID
  HandleStartupPacket(packet); // to send ack pakcet
}

void HandlePacket::HandleACKTowerVersionPacket(Packet_t &packet)
{
  // Send tower version packet
  Packet_Command = HandlePacket::CMD_TOWERVERSION;

  HandleTowerVersionPacket(packet);

  // Send ack tower version packet
  Packet_Command = HandlePacket::CMD_ACK_TOWERVERSION;

  HandleTowerVersionPacket(packet);
}

void HandlePacket::HandleACKTowerNumberPacket(Packet_t &packet)
{
  // Send tower number pacHandlePacketket
  Packet_Command = HandlePacket::CMD_TOWERNUMBER;

  HandleTowerVersionPacket(packet);

  // Send ack tower version packet
  Packet_Command = HandlePacket::CMD_ACK_TOWERNUMBER;

  HandleTowerNumberPacket(packet);
}

void HandlePacket::HandleACKTowerModePacket(Packet_t &packet)
{
  // Send tower mode packet
  Packet_Command = HandlePacket::CMD_TOWERMODE;

  HandleTowerVersionPacket(packet);

  // Send ack tower mode packet
  Packet_Command = HandlePacket::CMD_ACK_TOWERMODE;

  HandleTowerModePacket(packet);
}

#if 0
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
#endif

static Packet_t Packet(BAUDRATE, CPU_BUS_CLK_HZ); // initialize the packet obejct

namespace CallBack{

LED_t LedPIT(LED_t::LED_GREEN);
LED_t LedRTC(LED_t::LED_YELLOW);
static RTC::RTC_t Rtc(RTCCallback, 0); // Initialize RTC module

//LED_t LedRTC(LED_t::LED_BLUE);

//function description
 void PITCallback(void* argu)
 {
  LedPIT.LEDs_Toggle();
 }

 void RTCCallback(void* argu)
 {
  uint8_t hours, mins, sec;

  LedRTC.LEDs_Toggle();

  Rtc.RTC_Get(hours, mins, sec);

  Packet_Parameter1 = hours;  // Parameter 1: hours
  Packet_Parameter2 = mins; // Parameter 2: mins
  Packet_Parameter3 = sec; // Parameter 3: sec

  Packet.Packet_t::PacketPut(); //send it to FIFO

 }

}


/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
  /* Write your local variable definition here */


 PIT::PIT_t pit(CPU_BUS_CLK_HZ, 500, CallBack::PITCallback, 0); // Initialize PIT module


   __DI();//Disable interrupt

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/

  __EI(); //Enable the interrupt

  /* Write your code here */
  for (;;)
  {
    if ( Packet.Packet_t::PacketGet())
    HandlePacket::HandleCommandPacket(Packet);

    Packet.UART_t::UART_ISR();
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



