
// implement FIFO functions
#include "FIFO.h"

 bool TFIFO::FIFO_Put( const uint8_t &data )
{
// To make sure the buffer is not full or overflow
 if( NbBytes <= FIFO_SIZE)
{
   NbBytes++;
 // add a byte of data into array
  Buffer[End] = data;
 // to make a circular array, reset the index
  ++End %= FIFO_SIZE;
 return true;
}
 else
 return false;

}

bool TFIFO::FIFO_Get( uint8_t * const dataPtr )
{
// can not retrieve if buffer is empty
 if( NbBytes != 0 )
{
   NbBytes--;
 // place the retrieved byte
 *dataPtr = Buffer[Start];
 // to make a circular array, reset Start index
  ++Start %= FIFO_SIZE;
 return true;
}
 else
 return false;

}


