#include "PIT.h"

#include "LEDs.h"


namespace PIT{

using F = void (void*); // a function type, not a pointer

static F* userFunc;
static void* userArgu;


 bool PIT_t::PIT_Init(const uint32_t &moduleClk)
{

 __DI();//Disable interrupt

 //enable clock gate
 SIM_SCGC6 |= SIM_SCGC6_PIT_MASK;

 //Disable timer0
 this->PIT_Enable(false);

 //enable timer module
 PIT_MCR &= ~PIT_MCR_MDIS_MASK ;

 //Freeze the timer
 PIT_MCR |= PIT_MCR_FRZ_MASK;

 //initialize the timer: 500ms
 PIT_LDVAL0 = 0xBEBC1F;

 this->PIT_Set(500, false); // period 500ms

 // Initialize NVIC
 // Vector =84, IRQ=68
 // Clear any pending interrupts on PIT0
 NVICICPR2 = (1 << (68 % 32));

 // Enable interrupts from PIT module
 NVICISER2 = (1 << (68 % 32));

 //Enable timer0
 this->PIT_Enable(true);

 userFunc = userFunction;
 userArgu = userArguments;

 __EI();// Enable the interrupt

 return true;
}


void PIT_t::PIT_Set(const uint32_t &p, bool restart)
{
 period = p ; // set a new value for period in sec

 if (restart)
 {
  //disable timer0
  this->PIT_Enable(false);

  //reload the timer
  PIT_LDVAL0 = (period / (1 / moduleClk) ) - 1;

  //Enable timer0
  this->PIT_Enable(true);
 }
 else
 {
  //reload the timer during running the timer
  //PIT_LDVAL0 = ( (period*1e-3) / (1 / moduleClk) ) - 1;
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


PIT_t::PIT_t(const uint32_t mClock, F* userFunc, void* userArgu):
moduleClk(mClock) , userFunction(userFunc), userArguments(userArgu)
{
  this->PIT_Init(moduleClk);
}

void __attribute__ ((interrupt)) PIT_ISR(void)
{
 if (PIT_TFLG0 & PIT_TFLG_TIF_MASK)
 {
  PIT_TFLG0 |= PIT_TFLG_TIF_MASK; //Clear the flag bit when interrupt trigger

 // then call callback function
  if (userFunc)
  userFunc(userArgu);
 }

}

}


