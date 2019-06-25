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
  const uint16_t resolutionDA = 3277; // resolution for converting actual voltage into DAC reading



class Analog_t
{
  private:
    int16_t adcReading;
    int16_t preSample;
    uint8_t channelNb;

  public:

    static std::vector<int32_t> inputSinusoid;

    Analog_t(const uint32_t clock);

    Analog_t(const uint8_t chNb);

    //Get a sample from channel 0
    inline bool GetSample();

    bool GetSignal();

    bool ZeroCrossDetector();

    bool PutSample(const int16_t value);

};

}

#endif /* SOURCES_ANALOG_H_ */
