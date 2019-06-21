/*
 * Analog.cpp
 *
 *  Created on: 14 Jun 2019
 *      Author: paul
 */

#include "Analog.h"

namespace Analog
{
// const int32_t ThreShold = 0.05 * 65536; //threshold in +- 0.01

  std::vector<int32_t> Analog_t::inputSinusoid;


  Analog_t::Analog_t(const uint32_t clock):
  preSample(0)
  {
    Analog_Init(clock);
  }


  bool Analog_t::GetSample()
  {
    return Analog_Get(0, &adcReading); // get a value from ADC
  }


  void Analog_t::GetVoltage()
  {
    int64_t read;
    int32_t tempData;


    if (this->GetSample() )
    {
      read = adcReading * 65536; //Convert into 32Q16

      tempData = (read * resolutionAD) >> 16; //Convert adcReading into actual voltage

      //Put one sample into vector
      if (inputSinusoid.size() < 18)
        inputSinusoid.push_back(tempData);
    }

  }


  bool Analog_t::PutSample(const int16_t value)
  {
    int16_t inputValue = value;

    //Convert into discrete level
    inputValue *= resolutionDA;

    return Analog_Put(0, inputValue);
  }


//To find the zero crossing point from negative to positive
//True: find the zero crossing point
  bool Analog_t::ZeroCrossDetector()
  {

    if (preSample == 0)
    {
      if (this->GetSample() )
        preSample = adcReading;
    }
    else
    {
      if (this->GetSample() )
      {
    	if (preSample * adcReading < 0) // this is the zero crossing point
    	  return true;
    	else
    	  preSample = adcReading;
      }

    }

    return false;
  }


  uint16_t&& Analog_t::GetFrequency()
  {
    this->ZeroCrossDetector();


  }





}
