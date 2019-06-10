/* ###################################################################
**     Filename    : main.cpp
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
** @version 1.0
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



#include "PIT.h"



const uint64_t BAUDRATE = 115200;
/* MODULE main */

class HandlePacket
{
  using F = void (void*); // a function type, not a pointer

  private:
    uint8_t priority;
	  OS_ERROR error;

  public:
  
  /*! @brief Constructor for creating the thread
   *  @param thread the thread needed to create
   *  @param pData data pointer passed to thread
   *  @param prio the proiority of thread
  */
   	HandlePacket(F* thread, void* pData, const uint8_t prio):
   	priority(prio)
    {
   	  // Create HandlePacket thread
   	  error = OS_ThreadCreate(thread,
   		               pData,
   		               &HandlePacketThreadStack[THREAD_STACK_SIZE - 1],
					   prio); // Highest priority
    }

    enum Command
	  {
	    IDMT = 0,
	    CURRENT = 0x01,
	    FREQUENCY = 0x02,
	    NB_TIME_TRIPPED = 0x03,
	    FAULT_TYPE = 0x04,
      CMD_DOR = 0x70,
      CMD_DOR_CURRENT = 0x71
    };
  
    static void HandleIDMTCharacteristic(Packet_t &packet);
	
    static void HandleFrequency(Packet_t &packet);
	
    static void HandleNbTimeTripped(Packet_t &packet);
	
	  static void HandleFaultType(Packet_t &packet);
	
  /*! @brief To return RMS value of current of each channel
   *  
   *  @param packet the PacketVert2 object
  */
	  static void HandleCurrent(Packet_t &packet);
	
  /*! @brief To decide how to send packet depending on packet command ID
   *  @param packet the PacketVert2 object
  */
	  static void HandleCommandPacket(Packet_t &packet);
};



void HandlePacket::HandleCommandPacket(Packet_t &packet)
{
  if (packet.Command == CMD_DOR)  
  {
	  switch (packet.parameter1)
	  {
	    case IDMT:
        HandleIDMTCharacteristic(packet);
        break;
      case FREQUENCY:
        HandleFrequency(packet);	
       	break;
      case CURRENT:
        HandleCurrent(packet);
        break;
      case NB_TIME_TRIPPED:
        HandleNbTimeTripped(packet);
        break;
      case FAULT_TYPE:
	      HandleFaultType(packet);
        break;
	  }
  }

}




void HandleIDMTCharacteristic(Packet_t &packet)
{
  IDMT idmt;
  uint8_t setting;

  if (packet.parameter2 == 0x01)
  {
    idmt.GetSetting(setting); //User get IDMT char
    packet.PacketPut(CMD_DOR, 0, 1, setting);
  }
  else
  //User set IDMT char
    idmt.Set(packet.parameter3);

}


namespace CallBack
{

  static LED_t Led;

 //function description
  void PIT(void* argu)
  {
    Led.Color(LED_t::GREEN);
    Led.Toggle();

  }

}



/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
  /* Write your local variable definition here */

  __DI(); //Disable interrupt

  PIT::PIT_t pit(CPU_BUS_CLK_HZ, CallBack::PIT, 0); // Initialize PIT module
  pit.Set(20, true); // set timer period as 20ms which is roughly 50Hz
  
  //Initialze UART
  Packet_t packet(BAUDRATE, CPU_BUS_CLK_HZ);

  //Initialize ADC
  Analog_Init(CPU_BUS_CLK_HZ);

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/

  __EI(); //Enable the interrupt

  /* Write your code here */
  for (;;)
  {
    if ( packet.PacketGet())
    HandlePacket::HandleCommandPacket(packet);

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



