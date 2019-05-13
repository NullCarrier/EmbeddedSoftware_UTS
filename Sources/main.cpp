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

//#include "Flash.h"

#include "PIT.h"

#include "RTC.h"

const uint64_t BAUDRATE = 115200;
/* MODULE main */


namespace HandlePacket
{

 enum Command{
  CMD_STARTUP = 0x04,
  CMD_TOWERVERSION = 0x09,
  CMD_TOWERNUMBER = 0x0B,
  CMD_SETTIME = 0x0C,
  CMD_TOWERMODE = 0x0D,
  CMD_ACK_STARTUP = 0x84,
  CMD_ACK_TOWERVERSION = 0x89,
  CMD_ACK_TOWERNUMBER = 0x8B,
  CMD_ACK_TOWERMODE = 0x8D,
  CMD_MYTOWERNUMBER = 0x9434
 };
  /*! @brief To handle startup packet
   *  @param packet the PacketVert2 object
   *  @return void
  */
  static void HandleStartupPacket(Packet_t &packet);

  /*! @brief To handle acknowledgement startup packet
   *  @param packet the PacketVert2 object
   *  @return void
  */
  static void HandleACKStartupPacket(Packet_t &packet);

  /*! @brief To handle tower version packet
   *  @param packet the PacketVert2 object
   *  @return void
  */
  static void HandleTowerVersionPacket(Packet_t &packet);

  /*! @brief To handle acknowledgement tower version packet
   *  @param packet the PacketVert2 object
   *  @return void
  */
  static void HandleACKTowerVersionPacket(Packet_t &packet);

  /*! @brief To handle tower number packet
   *  @param packet the PacketVert2 object
   *  @return void
  */
  static void HandleTowerNumberPacket(Packet_t &packet);

  /*! @brief To handle acknowledgement tower number packet
   *  @param packet the PacketVert2 object
   *  @return void
  */
  static void HandleACKTowerNumberPacket(Packet_t &packet);

  /*! @brief To handle tower mode packet
   *  @param packet the PacketVert2 object
   *  @return void
  */
  static void HandleTowerModePacket(Packet_t &packet);

  /*! @brief To handle acknowledgement tower mode packet
   *  @param packet the PacketVert2 object
   *  @return void
  */
  static void HandleACKTowerModePacket(Packet_t &packet);

  /*! @brief To send 4 packets initially and the repsonse for startup protocol
   *  @param packet the PacketVert2 object
   *  @return void
  */
  static void InitResponsePacket(Packet_t &packet);

 /*! @brief To decide how to send packet depending on packet command ID
   *  @param packet the PacketVert2 object
   *  @return void
  */
  static void HandleCommandPacket(Packet_t &packet);

 /*! @brief To handle set time packet
   *  @param packet the PacketVert2 object
   *  @return void
  */
  static void SetTimePacket(Packet_t &packet);



void HandlePacket::HandleStartupPacket(Packet_t &packet)
{
 // Assgin value for startup command according to packet protocol
  EnterCritical(); //Start critical section

  Packet_Parameter1 = Packet_Parameter2 = Packet_Parameter3 = 0;

  ExitCritical(); //End critical section

  packet.Packet_t::PacketPut(); //send it to FIFO

}

void HandlePacket::HandleTowerVersionPacket(Packet_t &packet)
{
  // Assgin value for towerversion command according to packet protocol
  EnterCritical(); //Start critical section

  Packet_Parameter1 = 0x76; // Parameter 1,//Command: Tower Version: v1.0
  Packet_Parameter2 = 0x01; // Parameter 2
  Packet_Parameter3 = 0x0;  // Parameter 3

  ExitCritical(); //End critical section

  packet.Packet_t::PacketPut(); // send it to FIFO
}

void HandlePacket::HandleTowerNumberPacket(Packet_t &packet)
{
// Assgin value for towernumber command according to packet protocol
  EnterCritical(); //Start critical section

  Packet_Parameter1 = 0x01;  // Parameter 1
  Packet_Parameter2 = 0x94; // Parameter 2
  Packet_Parameter3 = 0x34; // Parameter 3

  ExitCritical(); //End critical section

  packet.Packet_t::PacketPut(); //send it to FIFO

}

void HandlePacket::HandleTowerModePacket(Packet_t &packet)
{
  EnterCritical(); //Start critical section

  Packet_Parameter1 = 0x01;  // Parameter 1
  Packet_Parameter2 = 0x01; // Parameter 2
  Packet_Parameter3 = 0x00; // Parameter 3

  ExitCritical(); //End critical section

  packet.Packet_t::PacketPut(); //send it to FIFO
}

void HandlePacket::SetTimePacket(Packet_t &packet)
{
 RTC::RTC_t rtc;

 rtc.RTC_Set(Packet_Parameter1, Packet_Parameter2, Packet_Parameter3);

 packet.Packet_t::PacketPut(); //send it to FIFO
}

// Handling packet protocol (Tower to PC)
void HandlePacket::HandleCommandPacket(Packet_t &packet)
{
    switch (Packet_Command){

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
	case CMD_SETTIME: SetTimePacket(packet);
                      break;
    }

}

void HandlePacket::InitResponsePacket(Packet_t &packet)
{
  // Send tower startup packet
  HandleStartupPacket(packet);

  // Send tower version packet
  EnterCritical(); //Start critical section

  Packet_Command = HandlePacket::CMD_TOWERVERSION;

  ExitCritical(); //End critical section

  HandleTowerVersionPacket(packet);

  // Send tower number packet
  EnterCritical(); //Start critical section

  Packet_Command = HandlePacket::CMD_TOWERNUMBER;

  ExitCritical(); //End critical section

  HandleTowerNumberPacket(packet);

  // Send tower mode packet
  EnterCritical(); //Start critical section

  Packet_Command = HandlePacket::CMD_TOWERMODE;

  ExitCritical(); //End critical section

  HandleTowerModePacket(packet);
}

void HandlePacket::HandleACKStartupPacket(Packet_t &packet)
{
  // Send tower startup packet
  EnterCritical(); //Start critical section

  Packet_Command = HandlePacket::CMD_STARTUP;

  ExitCritical(); //End critical section

  InitResponsePacket(packet);

  // Send ack tower startup packet
  EnterCritical(); //Start critical section

  Packet_Command = HandlePacket::CMD_ACK_STARTUP; // to modify the packet command ID

  ExitCritical(); //End critical section

  HandleStartupPacket(packet); // to send ack pakcet
}

void HandlePacket::HandleACKTowerVersionPacket(Packet_t &packet)
{
  // Send tower version packet
  EnterCritical(); //Start critical section

  Packet_Command = HandlePacket::CMD_TOWERVERSION;

  ExitCritical(); //End critical section

  HandleTowerVersionPacket(packet);

  // Send ack tower version packet
  EnterCritical(); //Start critical section

  Packet_Command = HandlePacket::CMD_ACK_TOWERVERSION;

  ExitCritical(); //End critical section

  HandleTowerVersionPacket(packet);
}

void HandlePacket::HandleACKTowerNumberPacket(Packet_t &packet)
{
  // Send tower number pacHandlePacketket
  EnterCritical(); //Start critical section

  Packet_Command = HandlePacket::CMD_TOWERNUMBER;

  ExitCritical(); //End critical section

  HandleTowerVersionPacket(packet);

  // Send ack tower version packet
  EnterCritical(); //Start critical section

  Packet_Command = HandlePacket::CMD_ACK_TOWERNUMBER;

  ExitCritical(); //End critical section

  HandleTowerNumberPacket(packet);
}

void HandlePacket::HandleACKTowerModePacket(Packet_t &packet)
{
  // Send tower mode packet
  EnterCritical(); //Start critical section

  Packet_Command = HandlePacket::CMD_TOWERMODE;

  ExitCritical(); //End critical section

  HandleTowerVersionPacket(packet);

  // Send ack tower mode packet
  EnterCritical(); //Start critical section

  Packet_Command = HandlePacket::CMD_ACK_TOWERMODE;

  ExitCritical(); //End critical section

  HandleTowerModePacket(packet);
}

}

static Packet_t Packet(BAUDRATE, CPU_BUS_CLK_HZ); // initialize the packet obejct

namespace CallBack{

LED_t LedPIT(LED_t::LED_GREEN);
LED_t LedRTC(LED_t::LED_YELLOW);


//LED_t LedRTC(LED_t::LED_BLUE);

//function description
 void PIT(void* argu)
 {
  LedPIT.LEDs_Toggle();
 }

 void RTC(void* argu)
 {

  uint8_t hours, mins, sec;
  RTC::RTC_t rtc;

  LedRTC.LEDs_Toggle();

  rtc.RTC_Get(hours, mins, sec);

  Packet_Parameter1 = hours;  // Parameter 1: hours
  Packet_Parameter2 = mins; // Parameter 2: mins
  Packet_Parameter3 = sec; // Parameter 3: sec

  //Packet.Packet_t::PacketPut(); //send it to FIFO

 }

}


/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
  /* Write your local variable definition here */


 PIT::PIT_t pit(CPU_BUS_CLK_HZ, 500, CallBack::PIT, 0); // Initialize PIT module
 //RTC::RTC_t Rtc(CallBack::RTC, 0); // Initialize RTC module


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



