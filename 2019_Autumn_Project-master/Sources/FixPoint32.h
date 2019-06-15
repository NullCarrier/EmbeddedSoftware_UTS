/*
 * Analog_FTC.h
 *
 *  Created on: 10 Jun 2019
 *      Author: paul
 */

#ifndef FIXPOINT32_H_
#define FIXPOINT32_H_

#include "type_cpp.h"


const uint8_t NB_ITERATIONS = 6;

//32Q16
class FixPoint
{
  private:
    const uint32_t RMS_COE = 46334; //RMS coe for sinusoidal
    const uint32_t ratio = 22938;

  public:


    /*! @brief To Convert the mag of input voltage to RMS in mv with 32Q16 data format
     *  @param inputV the reference of input voltage
     *  @return bool - TRUE if corresponding voltage has been converted
     *  @note Assumes the input voltage is sinusoidal
     */
    int32_t&& GetVoltageRMS(const int32_t &inputVoltage);


    int32_t&& GetCurrentRMS(const int32_t &rmsVoltage);


    uint32_t& GetRMS(const float &baseF);


    uint32_t& Exp(const float &num, uint8_t exp);

};








#endif
