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


bool TFIFO::Put(const uint8_t data)
{

  OS_SemaphoreWait(availability, 0);

  critical section; //Enter critical section

    Buffer[End] = data; // add a byte of data into array buffer

    End++; // add a new data , then increment one for End index

    End %= FIFO_SIZE; // to make a circular array, reset End index

   // return true;

  //increment semaphore
   OS_SemaphoreSignal(nbItem);
}


bool TFIFO::Get(uint8_t &dataRef)
{
  //wait for other threads  if buffer was empty by suspending thread through a semaphore
  OS_SemaphoreWait(nbItem, 0);

  critical section; //Enter critical section

  dataRef = Buffer[Start]; // place the retrieved byte

  Start++; // removing one data, then increment Start index

  Start %= FIFO_SIZE; // to make a circular array, reset Start index

  //one semaphore has been used then it increments by 1 and return to its caller if semaphore is greater than 1
  OS_SemaphoreSignal(availability);

  return true;
}


