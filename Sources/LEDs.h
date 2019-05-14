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


 class LED_t
 {
  public:
    typedef enum
  {
  LED_ORANGE = (1 << 11),
  LED_YELLOW = (1 << 28),
  LED_GREEN = (1 << 29),
  LED_BLUE = (1 << 10)
  } TLED;

 /*! @brief Turns an LED on.
 *
 *  @param color The color of the LED to turn on.
 *  @note Assumes that LEDs_Init has been called.
 */
 void On() const;

 /*! @brief Turns off an LED.
 *
 *  @param color THe color of the LED to turn off.
 *  @note Assumes that LEDs_Init has been called.
 */
 void Off() const;

 /*! @brief Toggles an LED.
 *
 *  @param color THe color of the LED to toggle.
 *  @note Assumes that LEDs_Init has been called.
 */
 void Toggle() const;

 /*! @brief constructor to Initialise the color
 *
 *  @param color THe color of the LED
 *  @note Assumes that LEDs_Init has been called.
 */
 LED_t(TLED color)
 {
   if (LEDsInit() )
   m_color = color;
 }

 /*! @brief Sets up the LEDs before first use.
  *
  *  @return bool - TRUE if the LEDs were successfully initialized.
*/
  bool Init(void);

  void Color(TLED color);

  private:
    TLED m_color; /*!< The member color with TLED type */
 };


#endif
