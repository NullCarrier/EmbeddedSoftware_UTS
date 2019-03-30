
// implement functions : FIFO_Put, FIFO_Get
#include "FIFO(2).h"
#include <array> // begin() , end()



 bool TFIFO::FIFO_Put( const uint8_t& data )
{
	auto *offTheEnd = std::end(Buffer);
	auto *begin = std::begin(Buffer);
// To make sure the buffer is not full or overflow
 if( NbBytes <= FIFO_SIZE)
{
 // increment one for NbBytes as soon as Buffer is adding one byte
   NbBytes++;
 // add a byte of data into array
  *End = data;
 // to make a circular array, reset the pointer
  End = ( End != offTheEnd )? End++ : begin ;
 return true;
}
 else
 return false;

}


bool TFIFO::FIFO_Get( uint8_t * const dataPtr )
{
	auto *offTheEnd = std::end(Buffer);
	auto *begin = std::begin(Buffer);
// can not retrieve if buffer is empty
 if( NbBytes != 0 )
{
  // decrement one whenever the Buffer has been retrieved
   NbBytes--;
 // place the retrieved byte
 *dataPtr = *Start;
 // to make a circular array, reset Start pointer
  Start = (Start != offTheEnd )? Start++ : begin ;
 return true;
}
 else
 return false;

}


