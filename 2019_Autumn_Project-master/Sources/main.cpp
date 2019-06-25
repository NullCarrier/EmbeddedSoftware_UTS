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

const uint16_t OVERLOAD = 2109; // in 16Q11

/* MODULE main */

static Analog::Analog_t AnalogIO(CPU_BUS_CLK_HZ);

static IDMT::IDMT_t Idmt;

static uint32_t Counter;

static uint32_t Time;

static uint16union_t Current;



class HandlePacket
{
  using F = void (void*); // a function type, not a pointer

  public:
  
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

    /*! @brief To set up Characteristic of curve
       *
       *  @param packet the Packet object
      */
    static void HandleIDMTCharacteristic(Packet_t &packet);

    /*! @brief To send the frequency to user
       *
       *  @param packet the Packet object
      */
    static void HandleFrequency(Packet_t &packet);

    /*! @brief To return number of trip time via packet
       *
       *  @param packet the Packet object
      */
    static void HandleNbTimeTripped(Packet_t &packet);
	
	  //static void HandleFaultType(Packet_t &packet);
	
  /*! @brief To return current value via packet
   *  
   *  @param packet the Packet object
  */
	  static void HandleCurrent(Packet_t &packet);
	
  /*! @brief To decide how to send packet depending on packet command ID
   *  @param packet the Packet object
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
        HandleFrequency(packet);
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
  IDMT::IDMT_t idmt;
  uint8_t setting;

  if (packet.parameter2 == 0x01)
  {
    idmt.GetSetting(setting); //User get IDMT char
    packet.PacketPut(CMD_DOR, 0, 1, setting);
  }
  else
  {
    //User set IDMT char
    idmt.Set(packet.parameter3);
    packet.PacketPut(CMD_DOR, 0, 1, packet.parameter3);
  }
}


void HandlePacket::HandleFrequency(Packet_t &packet)
{
  uint16union_t frequency;

  frequency.l = AnalogIO.returnFrequency(); //Calculate frequency

  packet.PacketPut(CMD_DOR, 0x01, frequency.s.Lo, frequency.s.Hi);
}

void HandlePacket::HandleCurrent(Packet_t &packet)
{
  //critical section; //Enter critical section
  packet.PacketPut(CMD_DOR_CURRENT, 0x02, Current.s.Lo, Current.s.Hi);
}


/*! @brief To calculate inverse timing in fixpoint calculaiton
   *
   *  @param tripTime the tripTime in 32Q16
   *  @return time in ms
  */
static uint32_t GetInverTime(uint32_t tripTime)
{
  uint16_t newTime;
  uint32_t TimeR = (int64_t)Counter * 65536 / 1000; //The rest of time in s
  uint32_t percent;

  //calculate percentage
  percent = (int64_t)TimeR << 16 / Time ;

  newTime = (percent * tripTime) >> 16;

  return (newTime * 1000 / 65536); // Convert back to original vlaue in ms
}


namespace CallBack
{

  static LED_t led;
  static uint8_t PrevCurrent;
  static bool success;
  static bool Flag_Trip;

  //function description
  void PIT(void* argu)
  {
	if (Time != 0)
	{
	  Counter--;

	  if (Counter == 0)
	  {
		//Only send outsignal once
		if (!Flag_Trip)
		  Flag_Trip = AnalogIO.PutSample(5, 2); //Send trip time signal
		//Increment nBTriptime
	  }

	}


    if (AnalogIO.GetSignal() ) //Got 20 samples
      Current.l = Idmt.GetCurrent(); //calculate current

    //Initialize previous current
    if (!success)
    {
    	PrevCurrent = Current.s.Hi; // Get higher byte
    	success = 1;
    }


    if (Current.l > OVERLOAD)
    {
     // Turn on LED for overload condition
      led.Color(LED_t::ORANGE);
      led.On();

      AnalogIO.PutSample(5, 1); //Send timing signal via channel 1

      //Compare to find whether current has significant changes
      if (Current.s.Hi != PrevCurrent)
      {
    	//Inverse timing
    	if (Counter != 0)
    	  Counter = GetInverTime( Idmt.GetTripTime(Current.l) ); // Calculate new trip time based on new current

    	//Update previous current
        PrevCurrent = Current.s.Hi;
        Flag_Trip = 0;
      }
      else
      {
    	//New trip time
    	if (Counter == 0)
    	{
    	  Time = Idmt.GetTripTime(Current.l); //triptime in 32Q16 in sec
    	  Counter = Time *1000 / 65536; // triptime in ms
    	  Flag_Trip = 0;
    	}

      }

    }
    else
    {
      Time = 0; //Reset trip time
      led.Off();

      if (AnalogIO.PutSample(0, 1) )// Stop sending signal on both channel
    	AnalogIO.PutSample(0, 2);
    }

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



