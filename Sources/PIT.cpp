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


namespace PIT{

using F = void (void*); // a function type, not a pointer

// Local function pointer
static F* UserFunc;
static void* UserArgu;


 bool PIT_t::PIT_Init()
 {
 __DI();//Disable interrupt

 //enable clock gate
 SIM_SCGC6 |= SIM_SCGC6_PIT_MASK;

 //Disable timer0
 this->PIT_Enable(false);

 //enable timer module
 PIT_MCR &= ~PIT_MCR_MDIS_MASK;

 //Freeze the timer
 PIT_MCR |= PIT_MCR_FRZ_MASK;

 this->PIT_Set(1000 ,false); // period 1000ms

 // Initialize NVIC
 // Vector =84, IRQ=68
 // Clear any pending interrupts on PIT0
 NVICICPR2 = (1 << (68 % 32));

 // Enable interrupts from PIT module
 NVICISER2 = (1 << (68 % 32));

 //Enable timer0
 this->PIT_Enable(true);

 // Initialize the local usefunction
 UserFunc = userFunction;
 UserArgu = userArguments;

 __EI();// Enable the interrupt

 return true;
 }


 void PIT_t::PIT_Set(const uint32_t& period, bool restart)
 {

  if (restart){

  //disable timer0
  this->PIT_Enable(false);

  //reload the timer, unit: ns
  PIT_LDVAL0 = ( period * 1e6 / ((1/moduleClk)*1e9) ) - 1;

  //Enable timer0
  this->PIT_Enable(true);
  }
  else{
  //reload the timer during running the timer
  PIT_LDVAL0 = ( period * 1e6 / ((1/moduleClk)*1e9) ) - 1;
  }

  // Enable timer0 interrupt
  PIT_TCTRL0 |= PIT_TCTRL_TIE_MASK;
 }


void PIT_t::PIT_Enable(const bool enable)
{
 if (enable)
 PIT_TCTRL0 |= PIT_TCTRL_TEN_MASK; //Enable the timer0
 else
 PIT_TCTRL0 &= ~PIT_TCTRL_TEN_MASK; // disable the timer0
}


PIT_t::PIT_t(const uint32_t mClock, const uint32_t T, F* userFunc, void* userArgu):
moduleClk(mClock) , userFunction(userFunc), userArguments(userArgu)
{
  this->PIT_Init();
}

void __attribute__ ((interrupt)) PIT_ISR(void)
{
 if (PIT_TFLG0 & PIT_TFLG_TIF_MASK)
 {
  PIT_TFLG0 = PIT_TFLG_TIF_MASK; //Clear the flag bit when interrupt trigger

 // then call callback function
  if (UserFunc)
  UserFunc(UserArgu);
 }

}

}


