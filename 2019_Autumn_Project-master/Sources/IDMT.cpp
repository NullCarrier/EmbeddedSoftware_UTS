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
static characteristic curve;

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


int16_t&& IDMT_t::GetCurrent(int32_t &magV)
{
  int16_t current;
  int32_t tempData;
  FixPoint fixP;

  tempData = fixP.GetCurrentRMS(magV); // current in 32Q16

  current = (int16_t) (tempData >> 6); //convert into 16Q10

  return std::move(current);
}


static uint32_t Inverse();

static uint32_t VeryInverse(uint16_t &current)
{
  //Convert const into 32Q16 format
  const uint32_t K  = curve.KVI * 65536;
  uint32_t time;
  uint32_t currentRMS = current << 6; // turn into 32Q16 format

  time = ((int64_t) K << 16) / (currentRMS - 1 * 65536);

  return time;
}


static uint32_t ExtremelyInverse();


  //input current is in 16Q10
  //32Q16
  uint32_t&& IDMT_t::GetTripTime(uint16_t &current)
  {
    uint32_t time;

    switch (*setting)
    {
     /* case 0:

        return; */
      case 1://Very Inverse
        time = VeryInverse(current);
        return std::move(time);
     // case 2:

    }


  }




}
