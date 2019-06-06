/*! @file PIT.cpp
 *
 *  @brief Routines for controlling Periodic Interrupt Timer (PIT) on the TWR-K70F120M.
 *
 *  This contains the functions for operating the periodic interrupt timer (PIT).
 *
 *  @author Chao Li
 *  @date 7/05/2019
 *  Copyright (c) Chao Li. All rights reserved.
 */
#include "PIT.h"


OS_ECB* PIT_Semaphore;

namespace PIT
{
  using F = void (void*); // a function type, not a pointer

  // Local function pointer
  static F* UserFunc;
  static void* UserArgu;


  bool PIT_t::Init()
  {

   //enable clock gate
    SIM_SCGC6 |= SIM_SCGC6_PIT_MASK;

    //enable timer module
    PIT_MCR &= ~PIT_MCR_MDIS_MASK;

    //Disable timer1
    this->Enable(false);

    //Freeze the timer
    PIT_MCR |= PIT_MCR_FRZ_MASK;

    //initialize the timer: 500ms
    PIT_LDVAL1 = 0xBEBC1F;

 // Initialize NVIC
 // Vector =85, IRQ=69
 // Clear any pending interrupts on PIT0
    NVICICPR2 = (1 << (69 % 32));

 // Enable interrupts from PIT module
    NVICISER2 = (1 << (69 % 32));

 //Enable timer1
    this->Enable(true);

 // Initialize the local usefunction
    UserFunc = userFunction;
    UserArgu = userArguments;

    //Create semaphore for PIT thread
    PIT_Semaphore = OS_SemaphoreCreate(0);

    // Enable timer0 interrupt
    PIT_TCTRL1 |= PIT_TCTRL_TIE_MASK;

    return true;
  }


  void PIT_t::Set(const uint32_t& newPeriod, bool restart)
  {
    period = newPeriod * 1e6; // Convert into ns

    if (restart)
    {
      //disable timer1
      this->Enable(false);

     //reload the timer
      PIT_LDVAL1 = ( period / ( (1/ (float) moduleClk) * 1e9) ) - 1;

     //Enable timer1
      this->Enable(true);
    }
    else
      PIT_LDVAL1 = ( period / ( (1/ (float) moduleClk) * 1e9) ) - 1; //reload the timer during running the timer

  }


  void PIT_t::Enable(const bool enable)
  {
    if (enable)
      PIT_TCTRL1 |= PIT_TCTRL_TEN_MASK; //Enable the timer0
    else
      PIT_TCTRL1 &= ~PIT_TCTRL_TEN_MASK; // disable the timer0
  }


  PIT_t::PIT_t(const uint32_t mClock, void* userArgu):
  moduleClk(mClock), userArguments(userArgu)
  {
    this->Init();
  }


  void __attribute__ ((interrupt)) ISR(void)
  {
    // inform RTOS that ISR is being processed
    OS ISR;

    if (PIT_TFLG1 & PIT_TFLG_TIF_MASK)
    {
      PIT_TFLG1 = PIT_TFLG_TIF_MASK; //Clear the flag bit when interrupt trigger

      //Signal PIT thread
      OS_SemaphoreSignal(PIT_Semaphore);
    }

  }

}


