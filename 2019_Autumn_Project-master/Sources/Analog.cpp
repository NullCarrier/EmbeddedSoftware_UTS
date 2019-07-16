/*! @file Analog.cpp
 *
 *  @brief Routines for Analog module
 *
 *  This contains the functions for operating the ADC and DAC
 *
 *  @author Chao Li
 *  @date 7/05/2019
 *  Copyright (c) Chao Li. All rights reserved.
 */

#include "Analog.h"

namespace Analog
{
// const int32_t ThreShold = 0.05 * 65536; //threshold in +- 0.01

  std::vector<int32_t> Analog_t::inputSignal;
  uint16_t Analog_t::frequency;

  Analog_t::Analog_t(const uint32_t clock):
  prevSample(0)
  {
    Analog_Init(clock);
  }


  bool Analog_t::GetSample()
  {
    return Analog_Get(0, &adcReading); // get a value from ADC channel 0
  }


  bool Analog_t::GetSignal()
  {
    int64_t read;
    int32_t tempData;

    if (this->GetSample() )
    {
      //To find the zeroCrossing point
      if ( this->ZeroCrossDetector() )
        zeroPoint++;

      //Increment period
      if (zeroPoint < 2)
        period++;
      else
    	frequency = this->GetFrequency();//calculate frequency and reset period


      read = adcReading * 65536; //Convert into 32Q16

      tempData = (read * resolutionAD) >> 16; //Convert adcReading into actual voltage

      //Put one sample into signal
      if (inputSignal.size() < NB_Sample)
        inputSignal.push_back(tempData);
      else
        return true;

    }

    return false;
  }


  bool Analog_t::PutSample(const int16_t value, const uint8_t channelNb)
  {
    int16_t inputValue = value;

    //Convert into discrete level
    inputValue *= resolutionDA;

    return Analog_Put(channelNb, inputValue);
  }



//True: find the zero crossing point
  bool Analog_t::ZeroCrossDetector()
  {

    if (prevSample == 0)
    {
      prevSample = adcReading;
    }
    else
    {
      if (prevSample * adcReading < 0) // this is the zero crossing point
        return true;
      else
    	prevSample = adcReading;
    }

    return false;
  }

  //16Q4
  uint16_t&& Analog_t::GetFrequency()
  {
    uint16_t frequency;

    period *= 16; // in ms

    frequency = (int32_t) (1000 * 16) / period >> 4; //in Hz

    period = zeroPoint = 0;

    return std::move(frequency);
  }

  uint16_t Analog_t::returnFrequency()
  {
    return frequency;
  }



}
