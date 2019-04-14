#include "Flash.h"
#include <array>

#define NB_BYTES 8

static std::array<int, NB_BYTES> flashSector0;

inline bool Flash_Init(void)
{
 return true;
}

// applying the concept of program counter
bool Flash_AllocateVar(volatile void** variable, const uint8_t &size)
{
   
  if ( size == 4 && pc == 0) 
  {
	*variable = FLASH_DATA_START;
  	pc += size;
  }
  else if(size == 4)
  {  
    *variable = FLASH_DATA_START + size;
	pc += size;
  }
  else if(size == 2)
  {	  
   switch (pc)
   {
    case 0: *variable = FLASH_DATA_START;
	        pc +=size; 
       break;	
    case 2: *variable = FLASH_DATA_START + size;
	        pc +=size;
       break;
	case 4: *variable = FLASH_DATA_START + 2 * size;
       break;
    case 6: *variable = FLASH_DATA_START + 3 * size;
	   break;
    }
  }


  return true;
}

bool Flash_Write32(volatile uint32_t* const address, const uint32_t &data)
{
  EraseSector();
  
  WritePhrase(*address, );
    
  return true;
}

bool Flash_Write16(volatile uint16_t* const address, const uint16_t &data)
{
 Flash_write32((uint32_t *)address, data);
 return true;
}

bool Flash_Write8(volatile uint8_t* const address, const uint8_t &data)
{
 Flash_Write16((uint16_t *)address, data);	
 return true;	
}


static bool LaunchCommand(TFCCOB &commonCommandObject)
{ 
  while (1)
  {
	if (FTFE_FSTAT & FTFE_FSTAT_CCIF_MASK)
	{
	  if ((FTFE_FSTAT & FTFE_FSTAT_ACCERR_MASK) || (FTFE_FSTAT & FTFE_FSTAT_FPVIOL_MASK))
       FTFE_FSTAT |= 0x30; // clear the old errors
      else
	 {
	// more parameter?	
 	// write to FCCOB to load required command parameter
	 }
	
       FTFE_FSTAT |= 0x80;
	   break;
    }
  }
  
}

static bool WritePhrase(const uint32_t &address, const uint64union_t &phase)
{
   	
	
}

static bool EraseSector(const uint32_t &address)
{


}
