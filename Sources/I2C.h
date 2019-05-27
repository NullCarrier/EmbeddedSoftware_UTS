/*! @file I2C.h
 *
 *  @brief I/O routines for the K70 I2C interface.
 *
 *  This contains the functions for operating the I2C (inter-integrated circuit) module.
 *
 *  @author Chao Li
 *  @date 27/05/2019
 *  Copyright (c) Chao Li. All rights reserved.
 */

#ifndef I2C_H
#define I2C_H

// new types
#include "type_cpp.h"
#include "MK70F12.h"
#include "PE_Types.h"
#include "Cpu.h"

namespace I2C{

class I2C_t
{
  using F = void (void*);

  private:
  uint8_t primarySlaveAddress; /*!<The primary slave address>*/
  uint32_t moduleClk; /*!<The module clock in Hz.>*/
  static F* readCompleteCallbackFunction;  /*!< The user's read complete callback function. */
  static void* readCompleteCallbackArguments; /*!< The user's read complete callback function arguments. */

  protected:
  /*! @brief constructor for initializing I2C module
   *
   *  @param clock The module clock in Hz.
   *  @param readCompleteCallbackFunc The user's read complete callback function.
   *  @param readCompleteCallbackArgu The user's read complete callback function arguments.
   *  @return BOOL - TRUE if the I2C module was successfully initialized.
   */
  I2C_t(const uint32_t clock, F* readCompleteCallbackFunc, void* readCompleteCallbackArgu);

  /*! @brief Sets up the I2C before first use.
    *
    *
   */
  bool Init() const;


  public:
  /*! @brief Selects the current slave device
   *
   * @param slaveAddress The slave device address.
   */
  void SelectSlaveDevice(const uint8_t slaveAddress);

  /*! @brief Write a byte of data to a specified register
   *
    * @param registerAddress The register address.
   * @param data The 8-bit data to write.
   */
  void Write(const uint8_t registerAddress, const uint8_t data);

  /*! @brief Reads data of a specified length starting from a specified register
   *
   * Uses polling as the method of data reception.
   * @param registerAddress The register address.
   * @param data A pointer to store the bytes that are read.
   * @param nbBytes The number of bytes to read.
   */
  void PollRead(const uint8_t registerAddress, uint8_t* const data, const uint8_t nbBytes);

  /*! @brief Reads data of a specified length starting from a specified register
   *
   * Uses interrupts as the method of data reception.
   * @param registerAddress The register address.
   * @param data A pointer to store the bytes that are read.
   * @param nbBytes The number of bytes to read.
   */
  void IntRead(const uint8_t registerAddress, uint8_t* const data, const uint8_t nbBytes);
};


/*! @brief Interrupt service routine for the I2C.
 *
 *  Only used for reading data.
 *  At the end of reception, the user callback function will be called.
 *  @note Assumes the I2C module has been initialized.
 */
void __attribute__ ((interrupt)) ISR(void);

}

#endif
