/*
 * FixPoint32.cpp
 *
 *  Created on: 13 Jun 2019
 *      Author: paul
 */


#include "FixPoint32.h"
#include <utility>


uint32_t&& FixPoint::GetVoltageRMS()
{
	using A = Analog::Analog_t;

	  uint32_t rmsVoltage; //32Q16
	  uint16_t sum = 0; //16Q7


	  //Square each sample in 16Q7
	  for (auto sample: A::inputSignal)
	  {
		int32_t sample_16Q7 = sample >> 9;
		uint16_t tempData;

	    tempData = ( sample_16Q7 * sample_16Q7 ) >> 7 ;
	    sum += tempData;
	  }

	  uint8_t num = A::inputSignal.size();

	  rmsVoltage = ( (int32_t) sum) << 9;

	  rmsVoltage = ( ( (int64_t) rmsVoltage ) << 16 ) / (num * 65536); //18 samples

	  A::inputSignal.clear();

	  return this->SquareRoot(rmsVoltage); //bind a variable to the rvalue reference

}


//Convert voltage in v into current
uint32_t&& FixPoint::GetCurrentRMS()
{

  uint32_t current;

  current = ( (int64_t) (this->GetVoltageRMS() ) << 16 ) / ratio;

  return std::move(current); //bind a variable to the rvalue reference
}



uint32_t&& FixPoint::SquareRoot(uint32_t &base)
{
  uint32_t result;
  const uint32_t oneHalf = 32768;

  result = ((int64_t) base * oneHalf) >> 16; //Initial guess

  //
  for (uint8_t i = 0; i < NB_ITERATIONS; i++)
    result = ( ( ( (int64_t) base << 16) / result + result ) * oneHalf ) >> 16;

  return std::move(result);
}



uint32_t&& FixPoint::Exp(const uint32_t &base, uint8_t exp)
{
  uint32_t temp = base; //32Q16


  while (exp)
  {
    temp = this->SquareRoot(temp);
    exp /= 2;
  }

  return std::move(temp);
}

