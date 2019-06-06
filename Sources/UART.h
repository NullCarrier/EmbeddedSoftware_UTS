
/*! @file UART.h
 *
 *  @brief I/O routines for UART communications on the TWR-K70F120M.
 *
 *  This contains the functions for operating the UART (serial port).
 *
 *  @author Chao Li
 *  @date 07/05/2019
 *  Copyright (c) Chao Li. All rights reserved.
 */

#ifndef UART_H
#define UART_H

// new types
#include "type_cpp.h"

// _EI() _DI(),
#include "PE_Types.h"

// include FIFO module
#include "FIFO.h"

// involve mask for all registers
#include "MK70F12.h"

#include "critical.h" //critical section



class UART_t
{
  private:
  uint32_t baudRate;  /*!< baudRate The desired baud rate in bits/sec. */
  uint32_t moduleClk;  /*!< moduleClk The module clock rate in Hz. */

  protected:
  /*! @brief Sets up the UART interface before first use.
   *
   *  @return bool - TRUE if the UART was successfully initialized.
   */
  bool Init() const;

  /*! @brief Constructor for the UART interface before first use.
     *
     *  @param rate The desired baud rate in bits/sec.
     *  @param clock The module clock rate in Hz.
     *
     */
  UART_t(const uint32_t rate, const uint32_t clock):
  baudRate{rate}, moduleClk{clock}
  {
   this->Init();
  }

  UART_t() = default;

#if 0
  protected:
  uint8_t txData;
  uint8_t &rxData;
#endif
  public:
  /*! @brief Put a byte in the transmit FIFO if it is not full.
   *
   *  @param rxData The byte to be placed in the transmit FIFO.
   *  @return bool - TRUE if the data was placed in the transmit FIFO.
   *  @note Assumes that UART_Init has been called.
   */
  bool InChar(uint8_t &rxData);

  /*! @brief Put a byte in the transmit FIFO if it is not full.
   *
   *  @param txData The byte to be placed in the transmit FIFO.
   *  @return bool - TRUE if the data was placed in the transmit FIFO.
   *  @note Assumes that UART_Init has been called.
   */
  bool OutChar(const uint8_t txData);
};



//static void RxThread(void* pData);


//static void TxThread(void* pData);


/*! @brief Poll the UART status register to try and receive and/or transmit one character.
 *
 *  @return void
 *  @note Assumes that UART_Init has been called.
 */
//void UART_Poll(void);

/*! @brief Interrupt service routine for the UART.
 *
 *  @note Assumes the transmit and receive FIFOs have been initialized.
 */
void __attribute__ ((interrupt)) UART_ISR(void);

#endif

