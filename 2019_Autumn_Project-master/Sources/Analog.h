/*
 * Analog.h
 *
 *  Created on: 14 Jun 2019
 *      Author: paul
 */

#ifndef SOURCES_ANALOG_H_
#define SOURCES_ANALOG_H_

#include "FixPoint32.h"


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

class Analog_t
{
  private:
    int16_t adcReading;
    const int32_t resolution = 20 ; // in 32Q16

  public:
    static int32_t maxVp; // in 32Q16

    Analog_t(const uint32_t clock);

    //Get a sample from channel 0
    bool GetSample();

    void GetVmax();

    bool PutSample(const int16_t &value);

};

}

#endif /* SOURCES_ANALOG_H_ */