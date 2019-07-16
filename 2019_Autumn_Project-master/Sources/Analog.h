/*! @file Analog.h
 *
 *  @brief Routines for Analog module
 *
 *  This contains the functions for operating the ADC and DAC
 *
 *  @author Chao Li
 *  @date 7/05/2019
 *  Copyright (c) Chao Li. All rights reserved.
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
    int16_t adcReading; /*!< The adcReading */
    int16_t prevSample; /*!< The previous sample */
    uint16_t period; /*!< The period of signal. */
    uint8_t zeroPoint; /*!< The zero crossing point of signal. */
    static uint16_t frequency; /*!< The frequency of signal. */

  public:

    static std::vector<int32_t> inputSignal; /*!< The inputSignal buffer */

    /*! @brief Constructor for initializing analog module
           *
           *
           */
    Analog_t(const uint32_t clock);

    /*! @brief To get a sample from ADC
           *
           *  @return bool - TRUE if get a sample successfully
           */
    bool GetSample();

    /*! @brief To get a sample from ADC and put into buffer
           *
           *  @return bool - TRUE if the sample has been put into buffer
           */
    bool GetSignal();

    /*! @brief To find the zero crossing point of signal
       *
       *  @return bool - TRUE if the zero crossing point has been found it.
       */
    bool ZeroCrossDetector();

    /*! @brief calculate frequency based on period
       *
       *  @return uint16_t - Rvalue of frequency
       */
    uint16_t&& GetFrequency();

    /*! @brief return frequency of signal
       *
       *  @return uint16_t - the value of frequency
       */
    uint16_t returnFrequency();

    /*! @brief Send a sample to DAC
       *
       *  @param value The data to be sent to DAC
       *  @param channelNb the channel number of DAC
       *  @return bool - TRUE if the data was placed in the transmit FIFO.
       *  @note Assumes Analog has been initialized
       */
    bool PutSample(const int16_t value, const uint8_t channelNb);

};

}

#endif /* SOURCES_ANALOG_H_ */
