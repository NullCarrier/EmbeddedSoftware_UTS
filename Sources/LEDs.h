/*! @file
 *
 *  @brief Routines to access the LEDs on the TWR-K70F120M.
 *
 *  This contains the functions for operating the LEDs.
 *
 *  @author PMcL
 *  @date 2015-08-15
 */

#ifndef LEDS_H
#define LEDS_H

// new types
#include "types.h"

/*! @brief LED to pin mapping on the TWR-K70F120M
 *
 */
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
 void LEDs_On() const;

 /*! @brief Turns off an LED.
 *
 *  @param color THe color of the LED to turn off.
 *  @note Assumes that LEDs_Init has been called.
 */
 void LEDs_Off() const;

 /*! @brief Toggles an LED.
 *
 *  @param color THe color of the LED to toggle.
 *  @note Assumes that LEDs_Init has been called.
 */
 void LEDs_Toggle() const;

 /*! @brief constructor to Initialise the color
 *
 *  @param color THe color of the LED
 *  @note Assumes that LEDs_Init has been called.
 */
 LED_t(TLED color): m_color{color}
   {
     LEDsInit();
   }

 /*! @brief Sets up the LEDs before first use.
  *
  *  @return bool - TRUE if the LEDs were successfully initialized.
*/
  bool LEDsInit(void);

  private:
    TLED m_color;

 };





#endif
