/*! @file UART.cpp
 *
 *  @brief I/O routines for UART communications on the TWR-K70F120M.
 *
 *  This contains the functions for operating the UART (serial port).
 *
 *  @author Chao Li
 *  @date 07/05/2019
 *  Copyright (c) Chao Li. All rights reserved.
 */

//Declare and define flash object and pointer
#include "IDMT.h"

namespace IDMT
{

static Flash Flash;
static uint8_t* IdmtSetting;


IDMT_t::IDMT_t()
{
  success = Flash.AllocateVar( (volatile void**) &IdmtSetting, sizeof (*IdmtSetting) ); //Alocate space
}


void IDMT_t::Set(uint8_t slope)
{
  if (success)
  success = Flash.Write8(IdmtSetting, slope); //set new setting in flash memory
}

void IDMT_t::GetSetting(uint8_t &slope)
{
  if (success)
    slope = *IdmtSetting;
}









}
