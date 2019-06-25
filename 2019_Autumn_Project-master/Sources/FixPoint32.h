/*! @file FixPoint32.h
 *
 *  @brief Routines for calculation based on fix point
 *
 *  This contains the functions for calculating RMS, Current
 *
 *  @author Chao Li
 *  @date 7/05/2019
 *  Copyright (c) Chao Li. All rights reserved.
 */

#ifndef FIXPOINT32_H_
#define FIXPOINT32_H_

#include "type_cpp.h"

#include "Analog.h"


//32Q16
class FixPoint
{
  private:
    const uint32_t ratio = 22938; // 1 Irms = 0.35Vrms
    const uint8_t NB_ITERATIONS = 6;

  public:
    /*! @brief To calculate true rms of input signal
     *  @return uint32_t - the voltage in 32Q16
     *
     */
    uint32_t&& GetVoltageRMS();

    /*! @brief To calculate current based on scale
         *  @return uint32_t - the current in 32Q16
         *  @note Assumes the input rms voltage is calculated
         */
    uint32_t&& GetCurrentRMS();

    /*! @brief To calculate squareroot in 32Q16 notation
         *  @param base the input value in 32Q16 notation
         *  @return uint32_t - the result in 32Q16
         */
    uint32_t&& SquareRoot(uint32_t &base);

    /*! @brief To Convert the mag of input voltage to RMS in mv with 32Q16 data format
         *  @param base the input value in 32Q16 notation
         *  @param exp the exponet
         *  @note Assumes the calculation is based on rational exponent
         *  @return uint32_t - the voltage in 32Q16
         */
    uint32_t&& Exp(const uint32_t &base, uint8_t exp);

};








#endif
