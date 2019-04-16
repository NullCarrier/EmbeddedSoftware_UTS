#include "Flash.h"
#include <array>

#define NB_BYTES 8

// declare array to keep track of flash memory
static std::array<int, NB_BYTES> flashSector0;

static TFCCOB commandObject;

inline bool Flash_Init(void)
{
 return true;
}

static bool FlashAllocateByte(volatile void** variable)
{
  for (unsigned i = 0; i < NB_BYTES;i++)
  {
	if (flashSector0[i] == 0)
	{
	 *variable = FLASH_DATA_START + i;
	  flashSector0[i] = 1;
	  return true;
	}
  }

}


static bool FlashAllocateHalfWord(volatile void** variable)
{
  for (unsigned i = 0; i < NB_BYTES;i++)
  {
	if ( (flashSector0[i] == 0) && (i % i == 0) )
	{
	 *variable = FLASH_DATA_START + i;
     flashSector0[i] = 1;
	 flashSector0[++i] = 1;
	 return true;
	}
  }

}


static bool FlashAllocateWord(volatile void** variable)
{
  int counter = 0;

  // testing upper half memory block
  for (unsigned i = 0; i < NB_BYTES - 4; i++)
  {
	 (flashSector0[i] != 1)?counter++ : ;
  }

  if (counter == 4)
  {
      *variable = FLASH_DATA_START;
	  for (unsigned p = 0; p < NB_BYTES - 4; p++)
	  {
		flashSector0[p] = 1;
	  }
	    return true;
  }
  else
  {
   // testing lower half memory block
   for (unsigned j = 4; i < NB_BYTES; j++)
    {
	  (flashSector0[j] != 1)?counter++ : ;
    }

   if (counter == 4)
   {
	  *variable = FLASH_DATA_START + 4;
      for (unsigned k = 4; i < NB_BYTES; k++)
	  {
	   flashSector0[k] = 1;
      }
      return true;

   }
   else
      return false;
  }

}


bool Flash_AllocateVar(volatile void** variable, const uint8_t &size)
{
  switch (size)
  {
    case 1: FlashAllocateByte(variable);
       break;
    case 2: FlashAllocateHalfWord(variable);
       break;
	case 4: FlashAllocateWord(variable);
       break;
  }

    return true;
}


bool Flash_Write32(volatile uint32_t* const address, const uint32_t &data)
{
  commandObject.EraseSector(FLASH_DATA_START);

  commandObject.WritePhrase(FLASH_DATA_START, data);

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

 bool LaunchCommand(TFCCOB &commonCommandObject)
{
  while (1)
  {
	if (FTFE_FSTAT & FTFE_FSTAT_CCIF_MASK)
	{
	  if ((FTFE_FSTAT & FTFE_FSTAT_ACCERR_MASK) || (FTFE_FSTAT & FTFE_FSTAT_FPVIOL_MASK))
      {
	   FTFE_FSTAT |= 0x30; // clear the old errors
	  }
      else
	  {
	// more parameter?
 	// write to FCCOB to load required command parameter
	// assgin FCMD
	   FTFE_FCCOB0 |= commonCommandObject.fccob0;
	   // assgin flash address to FCCOB
	   FTFE_FCCOB3 |= commonCommandObject.fccob3;
       FTFE_FCCOB2 |= commonCommandObject.fccob2;
	   FTFE_FCCOB1 |= commonCommandObject.fccob1;
       // assign data into byte0-7 in FCCOB
       FTFE_FCCOB4 |= commonCommandObject.dataByte7;
       FTFE_FCCOB5 |= commonCommandObject.dataByte6;
       FTFE_FCCOB6 |= commonCommandObject.dataByte5;
       FTFE_FCCOB7 |= commonCommandObject.dataByte4;
       FTFE_FCCOB8 |= commonCommandObject.dataByte3;
       FTFE_FCCOB9 |= commonCommandObject.dataByte2;
       FTFE_FCCOBA |= commonCommandObject.dataByte1;
       FTFE_FCCOBB |= commonCommandObject.dataByte0;
	  }

       FTFE_FSTAT &= ~0x80; //clear CCIF to launch command
	   break;
    }
  }

       return true;//(FTFE_FSTAT & FTFE_FSTAT_CCIF_MASK)? true : false;
}

 // Program Phrase command
 bool TFCCOB::WritePhrase(const uint32_t &address, const uint64union_t &phase)
{
   //union type local var to handle bytes
   uint32union_t word;
   uint16union_t halfWord;

   // to access lower 4bytes in phase
   word = phase.s.Lo;

   // access first, two bytes in phase
   halfWord.l = word.s.Lo;

   //assign data to parameter of FCCOB
   dataByte0 = halfWord.s.Lo;
   dataByte1 = halWord.s.Hi;

   // access third , fourth byte in phase
   halfWord.l = word.s.Hi;

   // assign data to Byte 2, Byte3 in FCCOB
   dataByte2 = halWord.s.Lo;
   dataByte3 = halWord.s.Hi;

   // to access higher 4bytes in phase
   word = phase.s.Hi;
   halfWord.l = word.s.Lo;

   // access fifth , sixth byte in phase
   halfWord.l = word.s.Lo;

   // assign data to Byte 4, Byte5 in FCCOB
   dataByte4 = halWord.s.Lo;
   dataByte5 = halWord.s.Hi;

   // to access last two bytes in phase
   halfWord.l = word.s.Hi;
   dataByte6 = halWord.s.Lo;
   dataByte7 = halWord.s.Hi;

   // FCMD for Program Phrase command
   fccob0 = 0x07;

   LaunchCommand(*this);
   return true;
}

 bool TFCCOB::EraseSector(const uint32_t &address)
{
  // local union_t to handle bytes
  uint32union_t flashAddress;
  uint16union_t loMidByte;

  // using union to hold memory address
  flashAddress.l = address;

  // assgin lower and mid byte to fccob3 , fccob2
  loMidByte.l = flashAddress.Lo;
  fccob3 = loMidByte.Lo;
  fccob2 = loMidByte.Hi;

  // assign higher byte to fccob1
  fccob1 = flashAddress.Hi;

  // FCMD for  Erase Flash Sector Command
  fccob0 = 0x09;

  LaunchCommand(*this);

  return true;
}

static bool ModifyPhrase(const uint32_t address, const uint64union_t phrase)
{
  // allocate the memory space for RAM
  uint64union_t ram;
  // read into a RAM
  ram.l = phrase.l;

  // modfication
  //....

  Flash_Write32(address, ram);

}
