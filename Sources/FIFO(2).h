/*! @file
 *
 *  @brief Routines to implement a FIFO buffer.
 *
 *  This contains the structure and "methods" for accessing a byte-wide FIFO.
 *
 *  @author PMcL
 *  @date 01/04/2019
 *  Copyright (c) UTS. All rights reserved.
 */

#ifndef FIFO_H
#define FIFO_H

// new types
#include "type_cpp.h"
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
      TFIFO()// default constructor for initializing FIFO
      {
      }
 bool FIFO_Put(const uint8_t &data);
 bool FIFO_Get(uint8_t * const dataPtr);

};

/*! @brief Initialize the FIFO before first use.
 *
 *  @param fifo A pointer to the FIFO that needs initializing.
 *  @return bool - TRUE if the FIFO was successfully initialised
 */
//bool FIFO_Init(TFIFO * const fifo);

/*! @brief Put one character into the FIFO.
 *
 *  @param fifo A pointer to a FIFO struct where data is to be stored.
 *  @param data A byte of data to store in the FIFO buffer.
 *  @return bool - TRUE if data is successfully stored in the FIFO.
 *  @note Assumes that FIFO_Init has been called.
 */
//bool FIFO_Put(TFIFO * const fifo, const uint8_t data);

/*! @brief Get one character from the FIFO.
 *
 *  @param fifo A pointer to a FIFO struct with data to be retrieved.
 *  @param dataPtr A pointer to a memory location to place the retrieved byte.
 *  @return bool - TRUE if data is successfully retrieved from the FIFO.
 *  @note Assumes that FIFO_Init has been called.
 */
//bool FIFO_Get(TFIFO * const fifo, uint8_t * const dataPtr);

#endif
