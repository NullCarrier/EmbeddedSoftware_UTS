/*
 * FixPoint32.cpp
 *
 *  Created on: 13 Jun 2019
 *      Author: paul
 */


#include "FixPoint32.h"



bool FixPoint::GetVoltageRMS(uint16_t &vol)
{
  unt32_t rmsVoltage;
  int64_t voltage = vol * 1000 * 65536; //Convert input voltage into mv in 32Q16

  //const int32_t rms_Coe = 0.707 * 65536; //Convert const into 32Q16

  rmsVoltage = (uint32_t) ((voltage * RMS_COE) >> base); //multiplication of two 32Q16 figures

  return rmsVoltage;
}


int32_t GetCurrentRMS(int32_t rmsVoltage)
{
  //Convert voltage in ms into current
  const int32_t ratio = 350 * 65536;
  int32_t current;

  current = ((int64_t) rmsVoltage << 16 ) / ratio;

  return current;
}


uint32_t& FixPoint::GetRMS(const float &baseF)
{
  int64_t base = baseF * 65536;
  uint32_t temp;
  uint32_t result;

  result = (base * 32768) >> 16; //Initial guess


  for (uint8_t i = 0; i < NB_ITERATIONS; i++)
    result = ( ( ( base << 16) / result + result ) * 32768 ) >> 16;

  return result;
}



uint32_t FixPoint_Pow(const float num, uint8_t exp)
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
