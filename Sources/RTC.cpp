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

namespace RTC
{

  using F = void(void*); // a function type, not a pointer

  // Local function pointer
  static F* UserFunc;
  static void* UserArgu;

bool RTC_t::RTC_Init()
{

  SIM_SCGC6 |= SIM_SCGC6_RTC_MASK; //Enable clock gate

  RTC_CR |= (RTC_CR_SC16P_MASK | RTC_CR_SC2P_MASK); // Required load capacitances 16pf and load capacitances 2pf

  RTC_CR |= RTC_CR_OSCE_MASK; //32.768kHz is enabled

  //Wait for startup time for osc
  for(uint64_t count; count < 600000; count++)
    ;

  RTC_LR &= ~RTC_LR_CRL_MASK; //Lock the control register

  RTC_IER |= RTC_IER_TSIE_MASK; // Enable the sec interrupt

 // Enable time counter
  RTC_SR |= RTC_SR_TCE_MASK;

 //The interrupt source is enabled in the NVIC
 // vector num = 83, IRQ = 67
 // Clear any pending interrupts on UART2
  NVICICPR2 = (1 << 3);

 // Enable interrupts from UART2 module
  NVICISER2 = (1 << 3);

 // Initialize the local usefunction
  UserFunc = userFunction;
  UserArgu = userArguments;


  return true;
}

void RTC_t::RTC_Set(const uint8_t hour, const uint8_t mins, const uint8_t sec)
{

  uint32_t second = sec + mins*60 + hour * 3600; //Convert into seconds

  RTC_SR &= ~RTC_SR_TCE_MASK; //Disable time counter

 //RTC_TPR = 0x00; // Write to Prescaler Register

  RTC_TSR = second;  // Write to time second Register

  RTC_SR |= RTC_SR_TCE_MASK; //Enable time counter

}

void RTC_t::RTC_Get(uint8_t &hours, uint8_t &mins, uint8_t &sec)
{

  uint32_t timeValue = RTC_TSR;

  uint32_t timeValueDay = timeValue % 86400;

  hours = timeValueDay / 3600;

  mins = (timeValueDay % 3600) / 60;

  sec = timeValueDay % 3600 % 60;

}


RTC_t::RTC_t(F* userFunc, void* userArgu):
userFunction(userFunc), userArguments(userArgu)
{
  this->RTC_Init();
}



void __attribute__ ((interrupt)) ISR()
{
  // inform RTOS that ISR is being processed
  OS ISR;

  //Signal PIT thread
  //OS_SemaphoreSignal(OS_ECB* const pEvent);
}

}



