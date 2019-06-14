/*
 * Analog.cpp
 *
 *  Created on: 14 Jun 2019
 *      Author: paul
 */

#include "Analog.h"

namespace Analog
{

  uint16_t Analog_t::maxVp;


  Analog_t::Analog_t(const uint32_t clock)
  {
    Analog_Init(clock);
  }


  bool Analog_t::GetSample(int16_t &analogInput)
  {
    bool success;

    success = Analog_Get(0, &magV); // get a value from ADC

    analogInput = magV;

    return success;
  }


  void Analog_t::GetVmax()
  {
    int16_t tempData = 0;

    if (this->GetSample(tempData) )
      maxVp = (maxVp > tempData)? maxVp : tempData;

  }


  bool Analog_t::PutSample(const int16_t &value)
  {
    return Analog_Put(0, value);
  }

}
