#ifndef CRITICAL_H_INCLUDED
#define CRITICAL_H_INCLUDED


// This tells your c++ compiler not to use c++-style name mangling when defining the required symbols from the header, allowing the linker to successfully find them in the c-compiled .o files
 extern "C"{
   #include "OS.h"
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