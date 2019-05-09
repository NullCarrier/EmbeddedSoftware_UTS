/*! @file RTC.cpp
 *
 *  @brief Routines for controlling the Real Time Clock (RTC) on the TWR-K70F120M.
 *
 *  This contains the functions for operating the real time clock (RTC).
 *
 *  @author Chao Li
 *  @date 4/05/2019
 *  Copyright (c) Chao Li. All rights reserved.
 */

#include "RTC.h"
#include <cassert>

namespace RTC{

using F = void(void*); // a function type, not a pointer

// Local function pointer
static F* UserFunc;
static void* UserArgu;

bool RTC_t::RTC_Init()
{
 __DI();//Disable interrupt

 RTC_CR |= RTC_CR_SC16P_MASK; // Required load capacitances 16pf

 RTC_CR |= RTC_CR_SC2P_MASK; // Required load capacitances 2pf

 RTC_CR |= RTC_CR_OSCE_MASK; //32.768kHz is enabled

 // Wait for startup time for osc
 for(uint32_t count; count < 60e6; count++)
    ;

 RTC_LR &= ~RTC_LR_CRL_MASK; //Lock the control register

 RTC_IER |= RTC_IER_TSIE_MASK; // Enable the sec interrupt

 SIM_SCGC6 |= SIM_SCGC6_RTC_MASK; //Enable clock gate

 //The interrupt source is enabled in the NVIC
 // vector num = 83, IRQ = 67
 // Clear any pending interrupts on UART2
 NVICICPR2 = NVIC_ICPR_CLRPEND(1 << 3);

 // Enable interrupts from UART2 module
 NVICISER2 = NVIC_ISER_SETENA(1 << 3);

 // Enable RTC ?
 RTC_SR |= RTC_SR_TCE_MASK;

 // Initialize the local usefunction
 UserFunc = userFunction;
 UserArgu = userArguments;

 __EI();// Enable the interrupt

 return true;
}

void RTC_t::RTC_Set(const uint8_t hour, const uint8_t mins, const uint8_t sec)
{
 EnterCritical(); //Start critical section

 uint32_t second = sec + mins*60 + hour * 3600; //Convert into seconds

 RTC_SR &= ~RTC_SR_TCE_MASK; //Disable time counter

 RTC_TPR = 0x00; // Write to Prescaler Register

 RTC_TSR = second;  // Write to time second Register

 RTC_SR |= RTC_SR_TCE_MASK; //Enable time counter

 ExitCritical(); //End critical section

}

void RTC_t::RTC_Get(uint8_t &hours, uint8_t &mins, uint8_t &sec)
{
 EnterCritical(); //Start critical section

 uint32_t timeValue = RTC_TSR;

 hours = timeValue / 3600;

 mins = (timeValue % 3600) / 60;

 sec = timeValue % 3600 % 60;

 ExitCritical(); //End critical section

}


RTC_t::RTC_t(F* userFunc, void* userArgu):
userFunction(userFunc), userArguments(userArgu)
{
  assert(this->RTC_Init());
}

void __attribute__ ((interrupt)) RTC_ISR()
{
  // call callback function
  if (UserFunc)
  UserFunc (UserArgu);
}

}
