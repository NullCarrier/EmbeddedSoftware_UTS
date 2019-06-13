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
    uint8_t base;

  public:

    FixPoint(const uint8_t baseNum):
    base(baseNum)
    {
    }


    /*! @brief To Convert the mag of input voltage to RMS in mv with 32Q16 data format
     *  @param inputV the reference of input voltage
     *  @return bool - TRUE if corresponding voltage has been converted
     *  @note Assumes the input voltage is sinusoidal
     */
    bool GetVoltageRMS(uint16_t &inputVoltage);



    uint32_t& GetCurrentRMS(uint32_t &rmsVoltage);


    uint32_t& GetRMS(const float &baseF);


    uint32_t& Exp(const float &num, uint8_t exp);

};








#endif
