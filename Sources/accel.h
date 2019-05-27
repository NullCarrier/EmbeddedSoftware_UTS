/*! @file accel.h
 *
 *  @brief HAL for the accelerometer.
 *
 *  This contains the functions for interfacing to the MMA8451Q accelerometer.
 *
 *  @author Chao Li
 *  @date 27/05/2019
 *  Copyright (c) Chao Li. All rights reserved.
 */

#ifndef ACCEL_H
#define ACCEL_H

// New types
#include "type_cpp.h"

// Inter-Integrated Circuit
#include "I2C.h"

namespace Accel{

// Accelerometer registers
#define ADDRESS_OUT_X_MSB 0x01

#define ADDRESS_INT_SOURCE 0x0C // system interrupt status reg

typedef enum
{
  POLL,
  INT
} TAccelMode;

#if 0
typedef struct
{
  uint32_t moduleClk;
  void (*dataReadyCallbackFunction)(void*);
  void* dataReadyCallbackArguments;
  void (*readCompleteCallbackFunction)(void*);	/*!< The user's read complete callback function. */
  void* readCompleteCallbackArguments;		/*!< The user's read complete callback function arguments. */
} TAccelSetup;
#endif

#pragma pack(push)
#pragma pack(1)

typedef union
{
  uint8_t bytes[3];	/*!< The accelerometer data accessed as an array. */
  struct
  {
    uint8_t x, y, z; /*!< The accelerometer data accessed as individual axes. */
  } axes;
} TAccelData;

#pragma pack(pop)

 class Accel_t: public I2C::I2C_t
 {
  using F = void (void*);

  private:
  TAccelMode mode; /*!< The mode of accelerometer. */
  static F* dataReadyCallbackFunction; /*!< The user's data ready callback function. */
  static void* dataReadyCallbackArguments; /*!< The user's data ready callback function arguments. */

  public:

  /*! @brief Initializes accelerometer via writing to configuration registers.
   *
   */
  bool Init();

  /*! @brief Initializes the accelerometer by calling the initialization routines of the supporting software modules.
   *
   *  @param clock  The module clock rate in Hz.
   *  @param readCompleteCallbackFunc The user's read complete callback function.
   *  @param readCompleteCallbackArgu The user's read complete callback arguments.
   *  @param dataReadyCallbackFunc The user's data ready callback function.
   *  @param dataReadyCallbackArgu The user's data ready callback function arguments.
   *  @return bool - TRUE if the accelerometer module was successfully initialized.
  */
  Accel_t(const uint32_t clock, F* readCompleteCallbackFunc, void* readCompleteCallbackArgu, F* dataReadyCallbackFunc, void* dataReadyCallbackArgu);

  /*! @brief Reads X, Y and Z accelerations.
   *  @param data is a an array of 3 bytes where the X, Y and Z data are stored.
   */
  void ReadXYZ(uint8_t data[3]);

  /*! @brief Set the mode of the accelerometer.
   *  @param mode specifies either polled or interrupt driven operation.
   */
  void SetMode(const TAccelMode mode);

 };


/*! @brief Initializes the accelerometer by calling the initialization routines of the supporting software modules.
 *
 *  @param accelSetup is a pointer to an accelerometer setup structure.
 *  @return bool - TRUE if the accelerometer module was successfully initialized.
 */
//bool Accel_Init(const TAccelSetup* const accelSetup);

/*! @brief Reads X, Y and Z accelerations.
 *  @param data is a an array of 3 bytes where the X, Y and Z data are stored.
 */
//void Accel_ReadXYZ(uint8_t data[3]);

/*! @brief Set the mode of the accelerometer.
 *  @param mode specifies either polled or interrupt driven operation.
 */
//void Accel_SetMode(const TAccelMode mode);

/*! @brief Interrupt service routine for the accelerometer.
 *
 *  The accelerometer has data ready.
 *  The user callback function will be called.
 *  @note Assumes the accelerometer has been initialized.
 */
void __attribute__ ((interrupt)) DataReady_ISR(void);

}

#endif
