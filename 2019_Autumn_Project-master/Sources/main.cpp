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

#include "analog.h"

#include "PIT.h"

#include "IDMT.h"

#include "Analog.h"

const uint64_t BAUDRATE = 115200;
const uint16_t OVERCURRENT = 1.03 * 2048; // in 16Q11 notation

// ----------------------------------------
// Thread set up
// ----------------------------------------
// Arbitrary thread stack size - big enough for stacking of interrupts and OS use.
const uint8_t THREAD_STACK_SIZE = 100;
const uint8_t NB_ANALOG_CHANNELS = 3; // Three Channels


// ----------------------------------------
// Thread stack
// ----------------------------------------
OS_THREAD_STACK(InitModulesThreadStack, THREAD_STACK_SIZE); /*!< The stack for the LED Init thread. */
static uint32_t AnalogThreadStacks[NB_ANALOG_CHANNELS][THREAD_STACK_SIZE] __attribute__ ((aligned(0x08)));

OS_THREAD_STACK(HandlePacketThreadStack, THREAD_STACK_SIZE); /*!< The stack for the handlePacket thread. */
OS_THREAD_STACK(OutputSignalThreadStack, THREAD_STACK_SIZE); /*!< The stack for the  thread. */


// ----------------------------------------
// Thread priorities
// 0 = highest priority
// ----------------------------------------
const uint8_t ANALOG_THREAD_PRIORITIES[NB_ANALOG_CHANNELS] = {3, 4, 5};

/* MODULE main */


IDMT::IDMT_t Idmt;

uint16union_t Current;

static uint32_t Counter;

uint32_t Time;

static OS_ECB* SignalFlag;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

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
	                          prio);
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
 //
  packet.PacketPut(CMD_DOR_CURRENT, 0x01, Current.s.Lo, Current.s.Hi);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------


/*! @brief Data structure used to pass Analog configuration to a user thread
 *
 */
typedef struct AnalogThreadData
{
  OS_ECB* semaphore;
  uint8_t channelNb;
} TAnalogThreadData;


/*! @brief Analog thread configuration data
 *
 */
static TAnalogThreadData AnalogThreadData[NB_ANALOG_CHANNELS] =
{
  {
    0, //semaphore
    0 // channelNb
  },
  {
    0,
    1
  },
  {
    0,
    2
  }
};


/*! @brief Initializes modules.
 *
 */
static void InitModulesThread(void* pData)
{
  // Analog
  Analog::Analog_t analog( (uint32_t) CPU_BUS_CLK_HZ);

  //Initialze UART
  Packet_t packet(BAUDRATE, CPU_BUS_CLK_HZ);

  // Generate the global analog semaphores
  for (uint8_t analogNb = 0; analogNb < NB_ANALOG_CHANNELS; analogNb++)
    AnalogThreadData[analogNb].semaphore = OS_SemaphoreCreate(0);

  // Initialize PIT module
  PIT::PIT_t pit(CPU_BUS_CLK_HZ);
  pit.Set(1, 0); // set timer period as 1ms which is roughly 52.3Hz

  // We only do this once - therefore delete this thread
  OS_ThreadDelete(OS_PRIORITY_SELF);
}


static void OutputSignalThread(void* pData)
{
  static Analog::Analog_t analogTrip( (uint8_t) 2 ); //Initialize analog channel 2 for tripping signal
  analogTrip.PutSample(0); //Idie mode

  for (;;)
  {
	(void)OS_SemaphoreWait(SignalFlag, 0);

	if (Counter == 0)
	  analogTrip.PutSample(5); //Send tripping signal
	else
	  Counter -= 1;
  }
}


void __attribute__ ((interrupt)) PIT::ISR()
{
  if (PIT_TFLG1 & PIT_TFLG_TIF_MASK)
  {
    PIT_TFLG1 = PIT_TFLG_TIF_MASK; //Clear the flag bit when interrupt trigger

    // Signal the analog channels to take a sample
    for (uint8_t analogNb = 0; analogNb < NB_ANALOG_CHANNELS; analogNb++)
      (void)OS_SemaphoreSignal(AnalogThreadData[analogNb].semaphore);
  }

}




/*! @brief Samples a value on an ADC channel and sends it to the corresponding DAC channel.
 *
 */
void AnalogLoopbackThread(void* pData)
{


  // Make the code easier to read by giving a name to the typecast'ed pointer
  #define analogData ((TAnalogThreadData*)pData)
  static LED_t led;
  static Analog::Analog_t analogTime( (uint8_t) 1);	//Initialize analog channel 1 for timing signal

  if (Time != 0)
    Counter--;

  for (;;)
  {
	//OS_DisableInterrupts();

    int16_t analogInputValue;

    //Waiting from ISR from PIT
    (void)OS_SemaphoreWait(analogData->semaphore, 0);

    Analog::Analog_t analogIO(analogData->channelNb);

    // Get analog sample, if 20 sample received then calculate RMS and the current
    if (analogIO.GetSignal() )
    {
      Current.l = Idmt.GetCurrent(); //Current in 16Q11

    }


    if (Current.l > OVERCURRENT)
    {
      led.Color(LED_t::ORANGE);
      led.On(); //Turn on warming LED

      Time = Idmt.GetTripTime(Current.l); //triptime in 32Q16 in sec
      Counter = Time *1000 / 65536; // triptime in ms

      analogTime.PutSample(5); //Send timing signal

      if (Counter == 0)
        (void)OS_SemaphoreSignal(SignalFlag);//signal output thread via semaphore
    }
    else
    {
      Time = 0; // Reset trip time
      led.Off(); //Turn on warming LED
    }


    //OS_EnableInterrupts();
  }
}


/*! @brief Handle packet thread
     *
     *  @param pData might not use but is required by the OS to create a thread.
     *
     */
static void HandlePacketThread(void* pData)
{
  // initialize the packet module
  Packet_t packet;

  for (;;)
  {
    if (packet.Packet_t::PacketGet())
      HandlePacket::HandleCommandPacket(packet);
  }

}


#if 0
namespace CallBack
{


 //function description
  void PIT(void* argu)
  {


      AnalogIO.GetVoltage(); //analog get a sample

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
#endif


/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
  /* Write your local variable definition here */
  
  OS_ERROR error;

    // Initialise low-level clocks etc using Processor Expert code
    PE_low_level_init();

    // Initialize the RTOS
    OS_Init(CPU_CORE_CLK_HZ, true);

    SignalFlag = OS_SemaphoreCreate(0);

    // Create module initialization thread
    error = OS_ThreadCreate(InitModulesThread,
                            NULL,
                            &InitModulesThreadStack[THREAD_STACK_SIZE - 1],
                            0); // Highest priority

    // Create module OutputSignal thread
        error = OS_ThreadCreate(OutputSignalThread,
                                NULL,
                                &OutputSignalThreadStack[THREAD_STACK_SIZE - 1],
                                1);


    // Create threads for 3 analog loopback channels
    for (uint8_t threadNb = 0; threadNb < NB_ANALOG_CHANNELS; threadNb++)
    {
      error = OS_ThreadCreate(AnalogLoopbackThread,
                              &AnalogThreadData[threadNb],
                              &AnalogThreadStacks[threadNb][THREAD_STACK_SIZE - 1],
                              ANALOG_THREAD_PRIORITIES[threadNb]);
    }

    // Create sending packet thread
    HandlePacket packetThread(HandlePacketThread, 0, 6);

    // Start multithreading - never returns!
    OS_Start();

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



