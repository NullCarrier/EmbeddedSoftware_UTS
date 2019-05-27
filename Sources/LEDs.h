/*! @file LEDs.h
 *
 *  @brief Routines to access the LEDs on the TWR-K70F120M.
 *
 *  This contains the functions for operating the LEDs.
 *
 *  @author Chao Li
 *  @date 25/04/2019
 *  Copyright (c) Chao Li. All rights reserved.
 */

#ifndef LEDS_H
#define LEDS_H

// new types
#include "type_cpp.h"

#include "MK70F12.h"

// _EI() _DI()
#include "PE_Types.h"

#include "Cpu.h"

 class LED_t
 {
  public:
  typedef enum{

  ORANGE = (1 << 11),
  YELLOW = (1 << 28),
  GREEN = (1 << 29),
  BLUE = (1 << 10)
  } TLED;

 /*! @brief Turns an LED on.
 *
 *  @note Assumes that LEDs_Init has been called.
 */
 void On() const;

 /*! @brief Turns off an LED.
 *
 *
 *  @note Assumes that LEDs_Init has been called.
 */
 void Off() const;

 /*! @brief Toggles an LED.
 *
 *
 *  @note Assumes that LEDs_Init has been called.
 */
 void Toggle() const;

 /*! @brief constructor to Initialize the color
 *
 *
 *  @note Assumes that LEDs_Init has been called.
 */
 LED_t()
 {
  if (Init())
  GPIOA_PSOR = ORANGE | YELLOW | GREEN | BLUE; // Reset all LED
 }

 /*! @brief Sets up the LEDs before first use.
  *
  *  @return bool - TRUE if the LEDs were successfully initialized.
*/
  bool Init(void);

  /*! @brief Assign color for LED depending on pins
    *
    *  @param color the color of LED
  */
  void Color(TLED color);

  private:
    int m_color; /*!< The member color with int type */
 };


#endif
