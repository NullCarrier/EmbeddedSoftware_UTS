/*! @file OS_cpp.h
 *
 *  @brief Routines to implement a simple real-time operating system (RTOS).
 *
 *  @author Chao Li
 *  @date 08/06/2016
 *  Copyright (c) Chao Li. All rights reserved.
 */

#ifndef OS_CPP_H_INCLUDED
#define OS_CPP_H_INCLUDED

#include "Cpu.h"
#include "PE_Types.h"


// This tells your c++ compiler not to use c++-style name mangling when defining the required symbols from the header, allowing the linker to successfully find them in the c-compiled .o files
 extern "C"{
   #include "OS.h" // Simple OS
 }


class critical
{
   public:
     critical()
     {
    	 OS_DisableInterrupts(); //Start critical section
     }

    ~critical()
    {
    	OS_EnableInterrupts(); //End critical section
    }
};

class OS
{
  public:
    OS()
    {
      OS_ISREnter(); //notifies the RTOS that an ISR is being processed
    }

    ~OS()
    {
      OS_ISRExit(); // notifies the RTOS that an ISR has completed
    }

};


#endif // CRITICAL_H_INCLUDED
