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

  critical section; //Enter critical section

  if (NbBytes < FIFO_SIZE) // To make sure the buffer is not full or overflow
  {

    NbBytes++; // increment one for NbBytes as soon as Buffer is adding one byte

    Buffer[End] = data; // add a byte of data into array buffer

    End++; // add a new data , then increment one for End index

    End %= FIFO_SIZE; // to make a circular array, reset End index

    return true;
  }
  else
	//wait for buffer is not full by suspending thread through a semaphore
    return false;
}


bool TFIFO::Get(uint8_t &dataRef)
{
  critical section; //Enter critical section

  if (NbBytes != 0) // can not retrieve if buffer is empty
  {
	//EnterCritical(); //Start critical section

    NbBytes--; // decrement one whenever the Buffer has been retrieved

    dataRef = Buffer[Start]; // place the retrieved byte

    Start++; // removing one data, then increment Start index

    Start %= FIFO_SIZE; // to make a circular array, reset Start index

    //ExitCritical(); //End critical section

    return true;
  }
  else
  //wait for buffer is not empty by suspending thread through a semaphore

	  return false;


}


