/*! @file FIFO.cpp
 *
 *  @brief Routines to implement a FIFO buffer.
 *
 *  This contains "methods" for accessing a byte-wide FIFO.
 *
 *  @author Chao Li
 *  student ID : 12199434
 *  @date 02/04/2019
 *  Copyright (c) Chao Li. All rights reserved.
 */

#include "FIFO.h"

TFIFO::TFIFO()
{
  //Create the semaphore
  itemsAvailable = OS_SemaphoreCreate(0);
  spaceAvailable = OS_SemaphoreCreate(FIFO_SIZE);
}


bool TFIFO::Put(const uint8_t data)
{
  critical section; //Enter critical section

  OS_SemaphoreWait(spaceAvailable, 0);

    //NbBytes++; // increment one for NbBytes as soon as Buffer is adding one byte

  Buffer[End] = data; // add a byte of data into array buffer

  End++; // add a new data , then increment one for End index

  End %= FIFO_SIZE; // to make a circular array, reset End index

  OS_SemaphoreSignal(itemsAvailable);

  return true;
}


bool TFIFO::Get(uint8_t &dataRef)
{
  critical section; //Enter critical section

  OS_SemaphoreWait(itemsAvailable, 0);

    //NbBytes--; // decrement one whenever the Buffer has been retrieved

  dataRef = Buffer[Start]; // place the retrieved byte

  Start++; // removing one data, then increment Start index

  Start %= FIFO_SIZE; // to make a circular array, reset Start index

  OS_SemaphoreSignal(spaceAvailable);
  return true;
}



