#include "PIT.h"


bool PIT_t::PIT_Init()
{
 //enable clock gate
 SIM

 //enable timer module
 PIT_MCR &= ~PIT_MCR_MDIS_MASK;

 //Freeze the timer
 PIT_MCR |= PIT_MCR_FRZ_MASK;

 //initialize the timer: 500ms
 PIT_LDVAL = ( moduleClk / 2) - 1;

 return true;
}


void PIT_t::PIT_Set(const uint32_t &Tp)
{
 period = Tp; // set a new value for period in ns

 //Enable timer0
 this->PIT_Enable(true);

 //reload the timer
 PIT_LDVAL = (period * E-9 / moduleClk) - 1;

 // Enable timer0 interrupt
 PIT_TCTRL0 |= PIT_TCTRL_TIE_MASK;

 return true;
}


void PIT_t::PIT_Enable(const bool &enable)
{
 if (enable)
 PIT_TCTRL0 |= PIT_TCTRL_TEN_MASK; //Enable the timer0
 else
 PIT_TCTRL0 &= ~PIT_TCTRL_TEN_MASK; // disable the timer0
}


PIT_t::(const uint32_t mClock, void (*callback)(void*), void* userArgu):
moduleClk(mClock), userFunction(callback), userArguments(userArgu)
{
  // period = 500ms;
  this->PIT_Init();
}

void __attribute__ ((interrupt)) PIT_ISR(void)
{

 PIT_TFLG0 = PIT_TFLG_TIF_MASK; //Write to TIF bit to clear it

 // then call callback function
 if (userFunction)
 (userFunction)(userArguments);
}




