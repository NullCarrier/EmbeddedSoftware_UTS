#include "PIT.h"

// involve mask for all registers
#include "MK70F12.h"

bool PIT_t::PIT_Init()
{
 uint8_t &&num;
 uint8_t IRQ = 68;
 _DI();//Disable interrupt

 //enable clock gate
 SIM_SCGC6 |= SIM_SCGC6_PIT_MASK;

 //enable timer module
 PIT_MCR &= ~PIT_MCR_MDIS_MASK;

 //Freeze the timer
 PIT_MCR |= PIT_MCR_FRZ_MASK;

 //initialize the timer: 500ms
 PIT_LDVAL = ( moduleClk / 2) - 1;

 // Initialize NVIC
 // Vector =84, IRQ=68
 num = IRQ % 32;
 // Clear any pending interrupts on PIT0
 NVICICPR2 = NVIC_ICPR_CLRPEND(1 << num);
 // Enable interrupts from PIT module
 NVICSER2 = NVIC_ISER_SETENA(1 << num);

 _EI();// Enable the interrupt
 return true;
}


void PIT_t::PIT_Set(const uint32_t &Tp)
{
 period = Tp * 1.0e-9 ; // set a new value for period in ns

 //Enable timer0
 this->PIT_Enable(true);

 //reload the timer
 PIT_LDVAL = (period / moduleClk) - 1;

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


PIT_t::PIT_t(const uint32_t mClock, F* userFunc, void* userArgu):
moduleClk(mClock), userFunction(userFunc), userArguments(userArgu)
{
  // period = 500ms;
  this->PIT_Init();
}

void __attribute__ ((interrupt)) PIT_ISR(void)
{

 PIT_TFLG0 |= PIT_TFLG_TIF_MASK; //Clear the flag bit when interrupt trigger

 // then call callback function
 if (userFunction)
 userFunction(userArguments);
}




