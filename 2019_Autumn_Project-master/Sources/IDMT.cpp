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
uint16_t* IDMT_t::nBTrip;

//uint16_t IDMT_t::count;

IDMT_t::IDMT_t()
{
  //Do not allocate space multiple times for the same pointer
  success = Flash.AllocateVar( (volatile void**) &setting, sizeof (*setting) ); //Alocate space for a byte

  success = Flash.AllocateVar( (volatile void**) &nBTrip, sizeof (*nBTrip) );
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


uint16_t&& IDMT_t::GetCurrent()
{
  uint16_t current;
  uint32_t tempData;
  FixPoint fixP;

  tempData = fixP.GetCurrentRMS(); // current in 32Q16

  current = (uint16_t) (tempData >> 5); //convert into 16Q11

  return std::move(current);
}


 void IDMT_t::Inverse()
 {
   FixPoint fixP;
   //uint32_t currentRMS = current << 6; // turn into 32Q16 format
   const uint32_t K  = curve.KI * 65536;

   //Calculate Irms ^ a
   currentRMS = fixP.Exp(currentRMS, 50);

   time = ((int64_t) K << 16) / (currentRMS - 1 * 65536);
 }


 void IDMT_t::VeryInverse()
 {
  //Convert const K into 32Q16 format
   const uint32_t K  = curve.KVI * 65536;
   //uint32_t currentRMS = current << 6; // turn into 32Q16 format

   time = ((int64_t) K << 16) / (currentRMS - 1 * 65536);

 }


 void IDMT_t::ExtremelyInverse()
 {
  //Convert const K into 32Q16 format
  const uint32_t K = curve.KEI * 65536;

  currentRMS = ( (int64_t) currentRMS * currentRMS ) >> 16;

  time = ((int64_t) K << 16) / (currentRMS - 1 * 65536);

 }


  //input current is in 16Q10
  //32Q16
  uint32_t&& IDMT_t::GetTripTime(uint16_t &current)
  {
    currentRMS = ( (uint32_t) current ) << 5; // turn into 32Q16 format

    switch (*setting)
    {
      case 0:
        this->Inverse();
        break;
      case 1:
        this->VeryInverse();
        break;
      case 2:
        this->ExtremelyInverse();
        break;
    }

    //Whenever the trip time has been generated, increase the number
    //count++;
    //Flash.Write16(nBTrip, count);

    return std::move(time);
  }



  bool IDMT_t::GetNbTrip(uint16_t &NbTrip)
  {
    NbTrip = *nBTrip;
  }





}
