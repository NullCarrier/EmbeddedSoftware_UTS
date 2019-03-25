#include "FIFO.h"



 bool TFIFO::FIFO_Put( const uint8_t data )
{
// To make sure the buffer is not full or overflow
 if( NbBytes <= FIFO_SIZE)
{
   NbBytes++;
 // ?: why is not a const uint8_t array
 // add a byte of data into array
  *End = data;
 // to make a circular array
  ++End %= FIFO_SIZE;
 return true;
}
 else
 return false;

}

bool FIFO_Get( uint8_t * const dataPtr )
{
// can not retrieve if buffer is empty
 if( NbBytes != 0 )
{
   NbBytes--;
 // place the retrieved byte
 *dataPtr = *Start;
 // to make a circular array, reset Start index
  ++Start %= FIFO_SIZE;
 return true;
}
 else
 return false;

}









