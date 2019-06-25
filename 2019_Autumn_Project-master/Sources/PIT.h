/*! @file PIT.h
 *
 *  @brief Routines for controlling Periodic Interrupt Timer (PIT) on the TWR-K70F120M.
 *
 *  This contains the functions for operating the periodic interrupt timer (PIT).
 *
 *  @author Chao Li
 *  @date 7/05/2019
 *  Copyright (c) Chao Li. All rights reserved.
 */

#ifndef PIT_H
#define PIT_H

// new types
#include "type_cpp.h"

#include "PE_Types.h" // _EI() _DI()

// involve mask for all registers
#include "MK70F12.h"

namespace PIT
{

class PIT_t
{
 using F = void (void*); // a function type, not a pointer

 private:
  uint32_t moduleClk; /*!<The module clock rate in Hz.*/
  uint32_t period; /*!<set a period in nanosec */
  //F* userFunction; /*!<a pointer to a user callback function.*/
  //void* userArguments; /*!<a pointer to the user arguments to use with the user callback function.*/

 public:
/*! @brief Constructor for initializing all member variables in PIT class
 *
 *  Enables the PIT and freezes the timer when debugging.
 *  @param mClock The module clock rate in Hz.
 *  @param userFunc is a pointer to a user callback function.
 *  @param userArgu is a pointer to the user arguments to use with the user callback function.
 *  @note Assumes that moduleClk has a period which can be expressed as an integral number of nanoseconds.
 */
  PIT_t(const uint32_t mClock);

/*! @brief Sets up the PIT before first use.
 *
 *  Enables the PIT and freezes the timer when debugging.
 *
 *  @return bool - TRUE if the PIT was successfully initialized.
 *  @note Assumes that moduleClk has a period which can be expressed as an integral number of nanoseconds.
 */
  bool Init();

/*! @brief Sets the value of the desired period of the PIT.
 *
 *  @param newPeriod The desired value of the timer period in nanoseconds.
 *  @param restart TRUE if the PIT is disabled, a new value set, and then enabled.
 *                 FALSE if the PIT will use the new value after a trigger event.
 *  @note The function will enable the timer and interrupts for the PIT.
 */
  void Set(const uint32_t& newPeriod, bool restart);

/*! @brief Enables or disables the PIT.
 *
 *  @param enable - TRUE if the PIT is to be enabled, FALSE if the PIT is to be disabled.
 */
  void Enable(const bool enable);

};

/*! @brief Interrupt service routine for the PIT.
 *
 *  The periodic interrupt timer has timed out.
 *  The user callback function will be called.
 *  @note Assumes the PIT has been initialized.
 */
void __attribute__ ((interrupt)) ISR(void);

}


#endif
