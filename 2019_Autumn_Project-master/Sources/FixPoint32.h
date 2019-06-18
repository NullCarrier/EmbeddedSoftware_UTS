/*
 * Analog_FTC.h
 *
 *  Created on: 10 Jun 2019
 *      Author: paul
 */

#ifndef FIXPOINT32_H_
#define FIXPOINT32_H_

#include "type_cpp.h"

#include "Analog.h"


//32Q16
class FixPoint
{
  private:
    //const uint32_t RMS_COE = 46334; //RMS coe for sinusoidal
    const uint32_t ratio = 22938; // 1 Irms = 0.35Vrms
    const uint8_t NB_ITERATIONS = 6;

  public:

    /*! @brief To Convert the mag of input voltage to RMS in mv with 32Q16 data format
     *  @param inputV the reference of input voltage
     *  @return bool - TRUE if corresponding voltage has been converted
     *  @note Assumes the input voltage is sinusoidal
     */
    uint32_t&& GetVoltageRMS();


    uint32_t&& GetCurrentRMS();


    uint32_t&& SquareRoot(uint32_t &base);


    uint32_t&& Exp(const uint32_t &base, uint8_t exp);

};








#endif
