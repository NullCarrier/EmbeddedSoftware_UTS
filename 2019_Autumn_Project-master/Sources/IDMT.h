/*! @file IDMT.h
 *
 *  @brief the basic routine for IDMT and related calculation
 *
 *  This contains the functions for IDMT
 *
 *  @author Chao Li
 *  @date 07/05/2019
 *  Copyright (c) Chao Li. All rights reserved.
 */
#ifndef IDMT_H_
#define IDMT_H_

#include "type_cpp.h"

#include "Flash.h"

#include "FixPoint32.h"

#include <utility>

#include "Analog.h"

namespace IDMT
{

  typedef struct
  {
    const float A = 0.02;
    const uint8_t AVI = 1;
    const uint8_t AEI = 2;
    const float KI = 0.14;
    const float KVI = 13.5;
    const uint8_t KEI = 80;
  }characteristic;



  class IDMT_t
  {
    private:
      static uint8_t* setting; /*!< The setting. */
      static uint16_t* nBTrip; /*!< The numbr of trip. */
      uint32_t time;  /*!< The trip time. */
      uint32_t currentRMS; /*!< The RMS current. */

    protected:

      /*! @brief Calculate trip time for inverse case
         *
         *
         */
      void Inverse();
      /*! @brief Calculate trip time for veryinverse case
         *
         *
         */
      void VeryInverse();
      /*! @brief Calculate trip time for Extremelyinverse case
         *
         *
         */
      void ExtremelyInverse();

    public:
      /*! @brief Constructor for initializing IDMT module
         *
         *
         */
      IDMT_t();
      /*! @brief Sets up the UART interface before first use.
         *
         *
         */
      void Set(uint8_t slope);
      /*! @brief Sets up the UART interface before first use.
         *
         *
         */
      void GetSetting(uint8_t &slope);

      /*! @brief To get the Current
         *
         *  @return uint16_t - Rvalue reference
         */
      uint16_t&& GetCurrent();

      /*! @brief To get number of trip time
         *
         *  @return bool - TRUE if get a number from flash memory
         */
      bool GetNbTrip(uint16_t &NbTrip);

      /*! @brief Sets up the UART interface before first use.
         *
         *  @return uint32_t - Rvalue reference
         */
      //the return value is in 32Q16 with uni: sec
      uint32_t&& GetTripTime(uint16_t &current);
};



}




#endif
