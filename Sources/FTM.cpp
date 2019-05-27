/*! @file FTM.cpp
 *
 *  @brief Routines for setting up the FlexTimer module (FTM) on the TWR-K70F120M.
 *
 *  This contains the functions for operating the FlexTimer module (FTM).
 *
 *  @author Chao Li
 *  @date 4/05/2019
 *  Copyright (c) Chao Li. All rights reserved.
 */

 #include "FTM.h"

namespace FTM{

 using F = void (void*);

 //Local function pointer
 static F* CallbackFunc;
 static void* CallbackArgu;

 bool FTM_t::Init()
 {
  __DI();//Disable interrupt

  // control gate FTM0
  SIM_SCGC6 = SIM_SCGC6_FTM0_MASK;

  //Disable write protection
  FTM0_MODE |= FTM_MODE_WPDIS_MASK;

  //Enable free running counter
  FTM0_MODE |= FTM_MODE_FTMEN_MASK;

  FTM0_CONF |= FTM_CONF_BDMMODE(3);

  //Init value of counter
  FTM0_CNTIN = 0x0;

  //Enable free counter
  FTM0_MOD = 0xffff;

  //Reset counter
  FTM0_CNT = 0x1;

  // Select clock source
  FTM0_SC |= FTM_SC_CLKS(2);

  // vector NVIC
  //The interrupt source is enabled in the NVIC
  // vector num = 78, IRQ = 62 means should use NVICICPR1 IRQ(32-63)
  // Clear any pending interrupts on FTM
  NVICICPR1 = NVIC_ICPR_CLRPEND(1 << 30);

  // Enable interrupts from FTM module
  NVICISER1 = NVIC_ISER_SETENA(1 << 30);

  //Assign function pointer to local one
  CallbackFunc = callbackFunction;
  CallbackArgu = callbackArguments;

  __EI();// Enable the interrupt

  return true;
 }

 bool FTM_t::Set() const
 {
  // setup channel FTM0ch0


 }


 FTM_t::FTM_t(F* callbackFunc, void* callbackArgu):
callbackFunction(callbackFunc), callbackArguments(callbackArgu)
 {
  this->Init();
 }


 void __attribute__ ((interrupt)) FTM0_ISR()
 {
  //Clear interrupt flag bit
  FTM0_SC &= ~FTM_SC_TOF_MASK;

  // then call callback function
  if (CallbackFunc)
  CallbackFunc(CallbackArgu);
 }



 }
