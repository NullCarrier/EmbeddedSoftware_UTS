/*! @file
 *
 *  @brief Routines to implement a FIFO buffer.
 *
 *  This contains "methods" for accessing a byte-wide FIFO.
 *
 *  @author PMcL
 *  @date 01/04/2019
 *  Copyright (c) UTS. All rights reserved.
 */

#include "FIFO(2).h"


 bool TFIFO::FIFO_Put(const uint8_t data)
{

 if(NbBytes < FIFO_SIZE) // To make sure the buffer is not full or overflow
{
  NbBytes++; // increment one for NbBytes as soon as Buffer is adding one byte

  Buffer[End] = data; // add a byte of data into array buffer

  ++End %= FIFO_SIZE; // to make a circular array, reset End index

 return true;
}
 else
 return false;
}


bool TFIFO::FIFO_Get(uint8_t * const dataPtr)
{

 if(NbBytes != 0) // can not retrieve if buffer is empty
{
  NbBytes--; // decrement one whenever the Buffer has been retrieved

 *dataPtr = Buffer[Start]; // place the retrieved byte

  ++Start %= FIFO_SIZE; // to make a circular array, reset Start index

 return true;
}
 else
 return false;
}


