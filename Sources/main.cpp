/* ###################################################################
 * test only remove later
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
*/
/*!
**  @addtogroup main_module main module documentation
**  @{
*/
// CPU module - contains low level hardware initialization routines
#include "Cpu.h"
// include packet module
#include "Packet(2).h"



/* MODULE main */

// constant for 3 handling cases
#define  CMD_STARTUP 0x04
#define  CMD_TOWERVERSION 0x09
#define  CMD_TOWERNUMBER 0x0B

void HandlePacket(Packet_t &packet)
{

 auto Packet_Command = packet.RxPacket.begin();

  if(*Packet_Command & PACKET_ACK_MASK )
 {
    if( HandleCommandPacket(packet) )
   *Packet_Command |= PACKET_ACK_MASK;
    else
  // to indicate the process of packet is failed
   (*Packet_Command) &= ~PACKET_ACK_MASK;
 }
 else
 {
   HandleCommandPacket(packet);
 }

}

void Packet_t::HandleStartupPacket()
{
  auto para = RxPacket.begin();
   for(int j = 0; j<3;j++)
  {
   *(para++) = 0x0;
  }
}

void Packet_t::HandleTowerVersionPacket()
{
 // tower version: v1.0

   auto para = RxPacket.begin();
   // Parameter 1
   *(para++) = 0x76;
   // Parameter 2
   *(para++) = 0x01;
   // Parameter 3
   *(para++) = 0x00;
}

void Packet_t::HandleTowerNumberPacket()
{
  auto para = RxPacket.begin();
  // Parameter 1
   *(para++) = 0x01;
  // Parameter 2
   *(para++) = 0x0;
  // Parameter 3
   *(para++) = 0x01;
}

// Handling packet protocol (Tower to PC)
bool HandleCommandPacket(Packet_t packet)
{
 auto Packet_Command = packet.RxPacket.begin();

 switch(*Packet_Command)
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


/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
  /* Write your local variable definition here */

  Packet_t packet(38400, CPU_BUS_CLK_HZ);

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/

  /* Write your code here */
  for (;;)
  {
    if( packet.Packet_Get() )
   HandlePacket(packet);
    UART_Poll();
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












