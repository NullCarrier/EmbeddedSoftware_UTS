#ifndef CRITICAL_H_INCLUDED
#define CRITICAL_H_INCLUDED

#include "Cpu.h"
#include "PE_Types.h"


class critical
{
   public:
     critical()
     {
        EnterCritical(); //Start critical section
     }

    ~critical()
    {
       ExitCritical(); //End critical section
    }
};


#endif // CRITICAL_H_INCLUDED
