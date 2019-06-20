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

#include "OS_cpp.h"

//#include "analog.h"

#include "PIT.h"

#include "IDMT.h"

#include "Analog.h"

const uint64_t BAUDRATE = 115200;

/* MODULE main */

Analog::Analog_t AnalogIO(CPU_BUS_CLK_HZ);

IDMT::IDMT_t Idmt;

static uint32_t counter;

static uint16_t TimeCounter;

uint32_t time;

class HandlePacket
{
  using F = void (void*); // a function type, not a pointer

  private:
    uint8_t priority;
	 // OS_ERROR error;

  public:
  
  /*! @brief Constructor for creating the thread
   *  @param thread the thread needed to create
   *  @param pData data pointer passed to thread
   *  @param prio the proiority of thread
  */
#if 0
	  HandlePacket(F* thread, void* pData, const uint8_t prio):
   	priority(prio)
    {
   	  // Create HandlePacket thread
   	  error = OS_ThreadCreate(thread,
   		               pData,
   		               &HandlePacketThreadStack[THREAD_STACK_SIZE - 1],
					   prio); // Highest priority
    }
#endif
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
  if (packet.command == CMD_DOR)
  {
	  switch (packet.parameter1)
	  {
	    case IDMT:
        HandleIDMTCharacteristic(packet);
        break;
      case FREQUENCY:
       // HandleFrequency(packet);
       	break;
      case CURRENT:
        HandleCurrent(packet);
        break;
      case NB_TIME_TRIPPED:
        HandleNbTimeTripped(packet);
        break;
      case FAULT_TYPE:
	    //  HandleFaultType(packet);
        break;
	  }
  }

}


void HandlePacket::HandleNbTimeTripped(Packet_t &packet)
{
  uint16union_t nBTimeTripped;

  Idmt.GetNbTrip(nBTimeTripped.l);
  packet.PacketPut(CMD_DOR, 3, nBTimeTripped.s.Lo, nBTimeTripped.s.Hi);
}


void HandlePacket::HandleIDMTCharacteristic(Packet_t &packet)
{
  uint8_t setting;

  if (packet.parameter2 == 0x01)
  {
    Idmt.GetSetting(setting); //User get IDMT char
    packet.PacketPut(CMD_DOR, 0, 1, setting);
  }
  else
  {
    //User set IDMT char
    Idmt.Set(packet.parameter3);
    packet.PacketPut(CMD_DOR, 0, 1, packet.parameter3);
  }
}



void HandlePacket::HandleCurrent(Packet_t &packet)
{

  uint16union_t current;

  current.l = Idmt.GetCurrent();

  time = Idmt.GetTripTime(current.l); //triptime in 32Q16 in sec
  counter = time *1000 / 65536; // triptime in ms

  packet.PacketPut(CMD_DOR_CURRENT, 0x01, current.s.Lo, current.s.Hi);

}



namespace CallBack
{


 //function description
  void PIT(void* argu)
  {

	//Start measurement from zero crossing point
	if (AnalogIO.ZeroCrossDetector() )
	  ;
	TimeCounter++;

	if (TimeCounter == 2)
		AnalogIO

    //AnalogIO.GetVoltage(); //analog get a sample, and retrieve a Vmax value

    //calculate current

    if (counter != 0)
    {
      counter -= 1;
      AnalogIO.PutSample(5);
    }
    else
      AnalogIO.PutSample(0);


    /*if (I > 1.03)
       generate timing signal
       calculate tripping time
       generate trip time signal
      else
       send0V via DAC
    */
  }

}



/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
  /* Write your local variable definition here */


  PIT::PIT_t pit(CPU_BUS_CLK_HZ, CallBack::PIT, 0); // Initialize PIT module
  pit.Set(1, 0); // set timer period as 1ms which is roughly 52.3Hz
  
  //Initialze UART
  Packet_t packet(BAUDRATE, CPU_BUS_CLK_HZ);

  //Initialize ADC
  //

  __DI(); //Disable interrupt

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



