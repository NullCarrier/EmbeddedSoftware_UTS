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
static bool success = 0;

uint8_t* IDMT_t::setting;
uint16union_t* IDMT_t::nBTrip;

IDMT_t::IDMT_t()
{
  //Do not allocate space multiple times for the same pointer
  if (!success)
  {
     success = Flash.AllocateVar( (volatile void**) &setting, sizeof (*setting) ); //Alocate space for a byte

     success = Flash.AllocateVar( (volatile void**) &nBTrip, sizeof (*nBTrip) );
  }

}


void IDMT_t::Set(uint8_t slope)
{
  if (success)
  success = Flash.Write8(setting, slope); //set new setting in flash memory
}



void IDMT_t::GetSetting(uint8_t &slope)
{
  if (success)
    slope = *setting;
}


uint16_t&& IDMT_t::GetCurrent(uint16_t &magV)
{
  uint32_t voltageRMS;
  uint16_t current;
  uint32_t tempData;
  FixPoint fixP;

  voltageRMS = fixP.GetVoltageRMS(magV); //RMS mv in 32Q16

  tempData = fixP.GetCurrentRMS(voltageRMS); // current in 32Q16

  current = (uint16_t) (tempData >> 6); //convert into 16Q10

  return std::move(current);
}






}
