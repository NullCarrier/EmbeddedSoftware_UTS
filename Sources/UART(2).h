
/*! @file UART(2).h
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
#include "FIFO(2).h"

// involve mask for all registers
#include "MK70F12.h"

class UART_t: public TFIFO
{
 protected:
  uint32_t baudRate;
  uint32_t moduleClk;

 public:

  UART_t(const uint32_t rate, const uint32_t clock);

  bool Init();

  bool InChar(uint8_t* const dataPtr);

  bool OutChar(const uint8_t data);

  void __attribute__ ((interrupt)) UART_ISR(void);
};

/*! @brief Sets up the UART interface before first use.
 *
 *  @param baudRate The desired baud rate in bits/sec.
 *  @param moduleClk The module clock rate in Hz.
 *  @return bool - TRUE if the UART was successfully initialized.
 */
//bool UART_Init(const uint32_t &baudRate, const uint32_t &moduleClk);

/*! @brief Get a character from the receive FIFO if it is not empty.
 *
 *  @param dataPtr A pointer to memory to store the retrieved byte.
 *  @return bool - TRUE if the receive FIFO returned a character.
 *  @note Assumes that UART_Init has been called.
 */

// bool UART_InChar(uint8_t* const dataPtr);

/*! @brief Put a byte in the transmit FIFO if it is not full.
 *
 *  @param data The byte to be placed in the transmit FIFO.
 *  @return bool - TRUE if the data was placed in the transmit FIFO.
 *  @note Assumes that UART_Init has been called.
 */

// bool UART_OutChar(const uint8_t data);

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
//void __attribute__ ((interrupt)) UART_ISR(void);

#endif

