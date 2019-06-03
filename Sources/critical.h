#ifndef CRITICAL_H_INCLUDED
#define CRITICAL_H_INCLUDED


#include "OS.h"

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
      OS_ISREnter();
    }

    ~OS()
    {
      OS_ISRExit();
    }

};






#endif // CRITICAL_H_INCLUDED
