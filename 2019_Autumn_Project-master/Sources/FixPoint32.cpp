/*
 * FixPoint32.cpp
 *
 *  Created on: 13 Jun 2019
 *      Author: paul
 */


#include "FixPoint32.h"
#include <utility>



uint32_t&& FixPoint::GetVoltageRMS(const uint32_t &vol)
{
  uint32_t rmsVoltage;
  int64_t voltage = vol;

  //const int32_t rms_Coe = 0.707 * 65536; //Convert const into 32Q16

  rmsVoltage = (voltage * RMS_COE) >> 16; //multiplication of two 32Q16 figures

  return std::move(rmsVoltage); //bind a variable to the rvalue reference
}


//Convert voltage in v into current
uint32_t&& FixPoint::GetCurrentRMS(const uint32_t &magV)
{

  uint32_t current;

  current = ( (int64_t) (this->GetVoltageRMS(magV) ) << 16 ) / ratio;

  return std::move(current); //bind a variable to the rvalue reference
}



uint32_t&& FixPoint::SquareRoot(uint32_t &base)
{
  uint32_t result;
  const uint32_t oneHalf = 0.5 * 65536;

  result = ((int64_t) base * oneHalf) >> 16; //Initial guess

  //
  for (uint8_t i = 0; i < NB_ITERATIONS; i++)
    result = ( ( ( (int64_t) base << 16) / result + result ) * oneHalf ) >> 16;

  return std::move(result);
}



uint32_t&& FixPoint::Exp(const uint32_t &base, uint8_t exp)
{
  uint32_t temp = base; //32Q16


  while (exp)
  {
    temp = this->SquareRoot(temp);
    exp /= 2;
  }

  return std::move(temp);
}

