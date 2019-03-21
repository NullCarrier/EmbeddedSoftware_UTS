// The implementation of functions in FIFO
// future feature : specfic error condition

#include "FIFO.h"

//initialize the FIFO using top level const object fifo

bool FIFO_Init(TFIFO * const fifo)
{
// assume the initial index of array starts at 0
// and the FIFO Buffer is empty
// return true if fifo has a valid address;
 fifo-> Start = fifo-> End = 0;
 fifo-> NbBytes = 0 ;

 return true;
}

// add one char into FIFO buffer

bool FIFO_Put(TFIFO * const fifo, const uint8_t data)
{
// To make sure the buffer is not full or overflow
 if(fifo-> NbBytes <= FIFO_SIZE)
{
  fifo-> NbBytes++;
 // ?: why is not a const uint8_t array
 // add a byte of data into array
  fifo-> Buffer[fifo-> End] = data;

 // to make a circular array
 fifo-> End = (fifo-> End + 1) % FIFO_SIZE;
 return true;
}
 else
 return false;

}


bool FIFO_Get(TFIFO * const fifo, const uint8_t dataPtr)
{
// can not retrieve if buffer is empty
 if(fifo-> NbBytes != 0)
{
  fifo-> NbBytes--;
 // place the retrieved byte
 dataPtr = fifo-> Buffer[fifo-> Start];

 // to make a circular array, reset Start index
 fifo-> Start = (fifo-> Start +1 ) % FIFO_SIZE;
}
 else
 return false;

}

