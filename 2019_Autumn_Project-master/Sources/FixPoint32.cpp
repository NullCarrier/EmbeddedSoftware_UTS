/*
 * FixPoint32.cpp
 *
 *  Created on: 13 Jun 2019
 *      Author: paul
 */


#include "FixPoint32.h"
#include <utility>



uint32_t&& FixPoint::GetVoltageRMS(uint16_t &vol)
{
  uint32_t rmsVoltage;
  int64_t voltage = (vol * 1000) * 65536; //Convert input voltage into mv in 32Q16

  //const int32_t rms_Coe = 0.707 * 65536; //Convert const into 32Q16

  rmsVoltage =  (voltage * RMS_COE) >> 16; //multiplication of two 32Q16 figures

  return std::move(rmsVoltage); //bind a variable to the rvalue reference
}



uint32_t&& FixPoint::GetCurrentRMS(uint32_t &rmsVoltage)
{
  //Convert voltage in ms into current
  const uint32_t ratio = 350 * 65536;
  uint32_t current;

  current = ((uint64_t) rmsVoltage << 16 ) / ratio;

  return std::move(current); //bind a variable to the rvalue reference
}

/*

uint32_t&& FixPoint::GetRMS(const float &baseF)
{
  int64_t base = baseF * 65536;
  uint32_t temp;
  uint32_t result;

  result = (base * 32768) >> 16; //Initial guess


  for (uint8_t i = 0; i < NB_ITERATIONS; i++)
    result = ( ( ( base << 16) / result + result ) * 32768 ) >> 16;

  return result;
}



uint32_t FixPoint::Exp(const float num, uint8_t exp)
{
  uint32_t temp; //32Q16

  temp = Bit32_RootMeanSquare(num);

  while (exp)
  {
    temp = FixPoint_RootMeanSquare(temp / 65536.0);
  exp /= 2;
  }

  return temp;
}
*/
