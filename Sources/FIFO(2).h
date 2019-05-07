/*! @file FIFO(2).h
 *
 *  @brief Routines to implement a FIFO buffer.
 *
 *  This contains the structure and "methods" for accessing a byte-wide FIFO.
 *
 *  @author Chao Li
 *  @date 07/05/2019
 *  Copyright (c) Chao Li. All rights reserved.
 */

#ifndef FIFO_H
#define FIFO_H

// new types
#include "type_cpp.h"

// _EI() _DI()
#include "PE_Types.h"

// involve mask for all registers
#include "MK70F12.h"

#include "Cpu.h"

// Number of bytes in a FIFO
#define FIFO_SIZE 256

/*!
 * @class TFIFO
 */
class TFIFO
{
  private:

  uint8_t Start;		/*!< The index of the position of the oldest data in the FIFO */
  uint8_t End; 		/*!< The index of the next available empty position in the FIFO */
  uint16_t volatile NbBytes;	/*!< The number of bytes currently stored in the FIFO */
  uint8_t Buffer[FIFO_SIZE];	/*!< The actual array of bytes to store the data */

  public:

/*! @brief Put one character into the FIFO.
 *
 *
 *  @param data A byte of data to store in the FIFO buffer.
 *  @return bool - TRUE if data is successfully stored in the FIFO.
 *  @note Assumes that FIFO_Init has been called.
 */
 bool FIFO_Put(const uint8_t data);

 /*! @brief Get one character from the FIFO.
 *
 *
 *  @param dataPtr A pointer to a memory location to place the retrieved byte.
 *  @return bool - TRUE if data is successfully retrieved from the FIFO.
 *  @note Assumes that FIFO_Init has been called.
 */
 bool FIFO_Get(uint8_t* const dataPtr);

};


#endif
