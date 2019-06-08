/* ###################################################################
 **     Filename    : main.cpp
 **     Project     : Lab6
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
 ** @version 6.0
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

// Simple OS
#include "OS_cpp.h"

// Analog functions
#include "analog.h"

//include packet module
#include "packet.h"

// ----------------------------------------
// Thread set up
// ----------------------------------------
// Arbitrary thread stack size - big enough for stacking of interrupts and OS use.
const uint8_t THREAD_STACK_SIZE = 100;
const uint8_t NB_ANALOG_CHANNELS = 2;

const uint64_t BAUDRATE = 115200;

// Thread stacks
OS_THREAD_STACK(InitModulesThreadStack, THREAD_STACK_SIZE); /*!< The stack for the LED Init thread. */
static uint32_t AnalogThreadStacks[NB_ANALOG_CHANNELS][THREAD_STACK_SIZE] __attribute__ ((aligned(0x08)));

OS_THREAD_STACK(HandlePacketThreadStack, THREAD_STACK_SIZE); /*!< The stack for the handlePacket thread. */

// ----------------------------------------
// Thread priorities
// 0 = highest priority
// ----------------------------------------
const uint8_t ANALOG_THREAD_PRIORITIES[NB_ANALOG_CHANNELS] = {1, 2};


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
    semaphore = NULL,
    channelNb = 0
  },
  {
    semaphore = NULL,
    channelNb = 1
  }
};


void LPTMRInit(const uint16_t count)
{
  // Enable clock gate to LPTMR module
  SIM_SCGC5 |= SIM_SCGC5_LPTIMER_MASK;

  // Disable the LPTMR while we set up
  // This also clears the CSR[TCF] bit which indicates a pending interrupt
  LPTMR0_CSR &= ~LPTMR_CSR_TEN_MASK;

  // Enable LPTMR interrupts
  LPTMR0_CSR |= LPTMR_CSR_TIE_MASK;
  // Reset the LPTMR free running counter whenever the 'counter' equals 'compare'
  LPTMR0_CSR &= ~LPTMR_CSR_TFC_MASK;
  // Set the LPTMR as a timer rather than a counter
  LPTMR0_CSR &= ~LPTMR_CSR_TMS_MASK;

  // Bypass the prescaler
  LPTMR0_PSR |= LPTMR_PSR_PBYP_MASK;
  // Select the prescaler clock source
  LPTMR0_PSR = (LPTMR0_PSR & ~LPTMR_PSR_PCS(0x3)) | LPTMR_PSR_PCS(1);

  // Set compare value
  LPTMR0_CMR = LPTMR_CMR_COMPARE(count);

  // Initialize NVIC
  // see p. 91 of K70P256M150SF3RM.pdf
  // Vector 0x65=101, IRQ=85
  // NVIC non-IPR=2 IPR=21
  // Clear any pending interrupts on LPTMR
  NVICICPR2 = NVIC_ICPR_CLRPEND(1 << 21);
  // Enable interrupts from LPTMR module
  NVICISER2 = NVIC_ISER_SETENA(1 << 21);

  //Turn on LPTMR and start counting
  LPTMR0_CSR |= LPTMR_CSR_TEN_MASK;
}

void __attribute__ ((interrupt)) LPTimer_ISR(void)
{
  // Clear interrupt flag
  LPTMR0_CSR |= LPTMR_CSR_TCF_MASK;

  // Signal the analog channels to take a sample
  for (uint8_t analogNb = 0; analogNb < NB_ANALOG_CHANNELS; analogNb++)
    (void)OS_SemaphoreSignal(AnalogThreadData[analogNb].semaphore);
}

/*! @brief Initialises modules.
 *
 */
static void InitModulesThread(void* pData)
{
  // Analog
  (void)Analog_Init(CPU_BUS_CLK_HZ);
  
  //Initialze UART
  Packet_t packet(BAUDRATE, CPU_BUS_CLK_HZ);

  // Generate the global analog semaphores
  for (uint8_t analogNb = 0; analogNb < NB_ANALOG_CHANNELS; analogNb++)
    AnalogThreadData[analogNb].semaphore = OS_SemaphoreCreate(0);

  // Initialise the low power timer to tick every 10 ms
  LPTMRInit(10);

  // We only do this once - therefore delete this thread
  OS_ThreadDelete(OS_PRIORITY_SELF);
}

/*! @brief Samples a value on an ADC channel and sends it to the corresponding DAC channel.
 *
 */
void AnalogLoopbackThread(void* pData)
{
  // Make the code easier to read by giving a name to the typecast'ed pointer
  #define analogData ((TAnalogThreadData*)pData)

  for (;;)
  {
    int16_t analogInputValue;

    (void)OS_SemaphoreWait(analogData->semaphore, 0);
    // Get analog sample
    Analog_Get(analogData->channelNb, &analogInputValue);
    // Put analog sample
    Analog_Put(analogData->channelNb, analogInputValue);
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


/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
  OS_ERROR error;

  // Initialise low-level clocks etc using Processor Expert code
  PE_low_level_init();

  // Initialize the RTOS
  OS_Init(CPU_CORE_CLK_HZ, true);

  // Create module initialisation thread
  error = OS_ThreadCreate(InitModulesThread,
                          NULL,
                          &InitModulesThreadStack[THREAD_STACK_SIZE - 1],
                          0); // Highest priority

  // Create threads for 2 analog loopback channels
  for (uint8_t threadNb = 0; threadNb < NB_ANALOG_CHANNELS; threadNb++)
  {
    error = OS_ThreadCreate(AnalogLoopbackThread,
                            &AnalogThreadData[threadNb],
                            &AnalogThreadStacks[threadNb][THREAD_STACK_SIZE - 1],
                            ANALOG_THREAD_PRIORITIES[threadNb]);
  }
  
  // Create sending packet thread with highest priority
  HandlePacket packetThread(HandlePacketThread, 0, 3);
  
  // Start multithreading - never returns!
  OS_Start();
}

/*!
 ** @}
 */
