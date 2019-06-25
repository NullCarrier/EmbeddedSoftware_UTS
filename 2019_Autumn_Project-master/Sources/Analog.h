/*
 * Analog.h
 *
 *  Created on: 14 Jun 2019
 *      Author: paul
 */

#ifndef SOURCES_ANALOG_H_
#define SOURCES_ANALOG_H_

#include "FixPoint32.h"
#include <vector>

extern "C"
{
  #include "analog.h"

  //Declare lib functions
  bool Analog_Init(const uint32_t moduleClock);
  bool Analog_Get(const uint8_t channelNb, int16_t* const valuePtr);
  bool Analog_Put(uint8_t const channelNb, int16_t const value);
}

namespace Analog
{

  const int32_t resolutionAD = 20 ; // in 32Q16
  const uint16_t resolutionDA = 3277;
  const uint8_t NB_Sample = 20; //Size of array


class Analog_t
{
  private:
    int16_t adcReading;
    int16_t prevSample;
    uint16_t period;
    uint8_t zeroPoint;
    static uint16_t frequency;

  public:

    static std::vector<int32_t> inputSignal;

    Analog_t(const uint32_t clock);

    //Get a sample from channel 0
    bool GetSample();

    bool GetSignal();

    bool ZeroCrossDetector();

    uint16_t&& GetFrequency();

    uint16_t returnFrequency();

    bool PutSample(const int16_t value, const uint8_t channelNb);

};

}

#endif /* SOURCES_ANALOG_H_ */
