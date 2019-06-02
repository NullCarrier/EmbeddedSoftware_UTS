/*! @file RTC.h
 *
 *  @brief Routines for controlling the Real Time Clock (RTC) on the TWR-K70F120M.
 *
 *  This contains the functions for operating the real time clock (RTC).
 *
 *  @author Chao Li
 *  @date 4/05/2019
 *  Copyright (c) Chao Li. All rights reserved.
 */

#ifndef RTC_H
#define RTC_H

// new types
#include "type_cpp.h"

#include "MK70F12.h"

// _EI() _DI()
#include "PE_Types.h"

#include "Cpu.h"

#include "critical.h"

namespace RTC{

class RTC_t
{
 using F = void(void*); // a function type, not a pointer

 private:
  F* userFunction;
  void* userArguments;

 public:
/*! @brief Initialize constructor for initializing userFunction and userArguments
 *
 *  @param uF is a pointer to a user callback function.
 *  @param userArgu is a pointer to the user arguments to use with the user callback function.
 */
  RTC_t(F* uF, void* userArgu);

/*! @brief Default constructor for initializing RTC object
 *
 *
 */
  RTC_t() = default;

/*! @brief Sets the value of the real time clock.
 *
 *  @param hour The desired value of the real time clock hours (0-23).
 *  @param mins The desired value of the real time clock minutes (0-59).
 *  @param sec The desired value of the real time clock seconds (0-59).
 *  @note Assumes that the RTC module has been initialized and all input parameters are in range.
 */
  void RTC_Set(const uint8_t hour, const uint8_t mins, const uint8_t sec);

 /*! @brief Gets the value of the real time clock.
 *
 *  @param hours The address of a variable to store the real time clock hours.
 *  @param mins The address of a variable to store the real time clock minutes.
 *  @param sec The address of a variable to store the real time clock seconds.
 *  @note Assumes that the RTC module has been initialized.
 */
  void RTC_Get(uint8_t &hours, uint8_t &mins, uint8_t &sec);

/*! @brief Initializes the RTC before first use.
 *
 *  Sets up the control register for the RTC and locks it.
 *  Enables the RTC and sets an interrupt every second.
 *
 *  @return bool - TRUE if the RTC was successfully initialized.
 */
  bool RTC_Init();

};


/*! @brief Interrupt service routine for the RTC.
 *
 *  The RTC has incremented one second.
 *  The user callback function will be called.
 *  @note Assumes the RTC has been initialized.
 */
 void __attribute__ ((interrupt)) ISR(void);


}



#endif
