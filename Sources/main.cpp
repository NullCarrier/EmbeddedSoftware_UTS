/* ###################################################################
**     Filename    : main.cpp
**     Project     : Lab2
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
*/         
/*!
**  @addtogroup main_module main module documentation
**  @{
*/         
/* MODULE main */


// CPU module - contains low level hardware initialization routines
#include "Cpu.h"

//include packet module
#include "packet.h"

#include "LEDs.h"

#include "Flash.h"

#define BAUDRATE 115200


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

void HandlePacketVer2::HandleTowerMode(PacketVer2_t &packet)
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
    case CMD_STARTUP:      InitResponsePacket(packet);
                           break;
    case CMD_TOWERVERSION: HandleTowerVersionPacket(packet);//only responce once for version
                           break;
    case CMD_TOWERNUMBER:  HandleTowerNumberPacket(packet);//only responce once for number
                           break;
    case CMD_TOWERMODE:    HandleTowerMode(packet);
                           break;
    }

}


void HandlePacketVer2::InitResponsePacket(PacketVer2_t &packet)
{
  HandleStartupPacket(packet);
  HandleTowerVersionPacket(packet);
  HandleTowerNumberPacket(packet);
  HandleTowerMode(packet);
}




/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
  /* Write your local variable definition here */

  PacketVer2_t packet(BAUDRATE, CPU_BUS_CLK_HZ);
  bool successNb;
  // bool successMode;

  //The non-volatile Tower number pointer to be allocated memory space
  volatile uint16union_t* NvTowerNb;

  // declare and define obj led
  LED_t led(LED_t::LED_ORANGE);
  led.LEDs_On();

  // to allocate  memory address to NVTowerNb
  successNb = Flash_AllocateVar(&NvTowerNb, sizeof(*NvTowerNb));
  //successMode = Flash_AllocateVar(&NvTowerNb, sizeof(*NvTowerNb));

  if(successNb && Flash_Erase())
  {
    // assign the value to para 2 and 3
    Packet_Parameter23 = HandlePacketVer2::CMD_MYTOWERNUMBER;
    // set the tower number
    successNb = Flash_Write16((uint16_t *)NvTowerNb, Packet_Parameter23);
  }

  // read through flash memory
  //Flash_Read();

  // send 4 packets once the tower is turned on
  HandlePacketVer2::InitResponsePacket(packet);

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/

  /* Write your code here */
  for (;;)
  {
    if ( packet.PacketVer2_t::Packet_Get() )
    HandlePacketVer2::HandleCommandPacket(packet);
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

