/*! @file
 *
 *  @brief Routines for erasing and writing to the Flash.
 *
 *  This contains the functions implementation needed for accessing the internal Flash.
 *
 *  @author Chao Li
 *  @date 29/04/2019
 *  Copyright (c) Chao Li. All rights reserved.
*/
#include "Flash.h"
#include <cmath>// include fmod()

#define NB_BYTES 8

// declare array to keep track of flash memory
static bool FlashSector0[NB_BYTES];

static TFCCOB CommandObject;

inline bool Flash_Init(void)
{
 return true;
}

/*! @brief To allocate memory address to data with a byte and keep track of memory space by
 *         the vaule of element:
 *         1 signify falsh memory has been written in the postion indicated by index of array
 *         0 signify falsh memory is avaiable in the postion indicated by index of array
 *  @param variable the address of a pointer to a variable that is to be allocated space in Flash memory.
 *  @return bool - TRUE if corresponding memory address has been assgined to pointer
 *  @note Assumes Flash has been initialized.
 */
static bool FlashAllocateByte(volatile uint16union_t** variable)
{
  for (uint64_t i = 0; i < NB_BYTES; i++)
  {
	if (FlashSector0[i] == 0)
	{
	 *variable = (uint16union_t *) (FLASH_DATA_START + i);
	  FlashSector0[i] = 1;
	  return true;
	}
  }

}

/*! @brief To allocate memory address to data with two bytes and keep track of memory space by
 *         the vaule of element:
 *         1 signify falsh memory has been written in the postion indicated by index of array
 *         0 signify falsh memory is avaiable in the postion indicated by index of array
 *  @param variable the address of a pointer to a variable that is to be allocated space in Flash memory.
 *  @return bool - TRUE if corresponding memory address has been assgined to pointer
 *  @note Assumes Flash has been initialized.
 */
static bool FlashAllocateHalfWord(volatile uint16union_t** variable)
{
  for (uint64_t i = 0; i < NB_BYTES;i++)
  {
	if ((FlashSector0[i] == 0)&& fmod(i, 2)==0 )
	{
	 *variable = (uint16union_t *) (FLASH_DATA_START + i);

     FlashSector0[i] = 1;
	 FlashSector0[++i] = 1;
	 return true;
	}
  }
}

#if 0
static bool FlashAllocateWord(volatile void** variable)
{
  int counter = 0;

  // testing upper half memory block
  for (unsigned i = 0; i < NB_BYTES - 4; i++)
  {
     if (flashSector0[i] != 1)
	     counter++;
  }

  if (counter == 4)
  {
      *variable  = (uint32_t *) FLASH_DATA_START;
     for (unsigned i = 0; i < NB_BYTES - 4; i++)
     {
       flashSector0[i] = 1;
     }
       return true;
  }
  else
  {
   // testing lower half memory block
   for (unsigned i = 4; i < NB_BYTES; i++)
    {
	 if (flashSector0[i] != 1)
	      counter++;
    }

   if (counter == 4)
   {
	  *variable = (uint32_t *) (FLASH_DATA_START + 4);
      for (unsigned i = 4; i < NB_BYTES; i++)
      {
	   flashSector0[i] = 1;
      }
      return true;

   }
   else
      return false;
  }

}

#endif

bool Flash_AllocateVar(volatile uint16union_t** variable, const uint8_t &size)
{
  switch (size)
  {
    case 1: //FlashAllocateByte(variable);
       break;
    case 2: FlashAllocateHalfWord(variable);
       break;
    case 4: //FlashAllocateWord(variable);
       break;
  }

  return true;
}


bool Flash_Write32(volatile uint32_t* const address, const uint32_t &data)
{
  // local var for handling bytes
  uint64union_t phrase;

  phrase.l = static_cast<uint64_t> (data);

  return CommandObject.WritePhrase(FLASH_DATA_START, phrase);
}


bool Flash_Write16(volatile uint16_t* const address, const uint16_t &data)
{
 uint32union_t data4Bytes;

 data4Bytes.l = static_cast<uint32_t> (data);

 return Flash_Write32((uint32_t *)address, data4Bytes.l);
}


bool Flash_Write8(volatile uint8_t* const address, const uint8_t &data)
{
 uint16union_t data2Bytes;

 data2Bytes.l = static_cast<uint16_t> (data);

 return Flash_Write16((uint16_t *)address, data2Bytes.l);
}

/*! @brief Writes the command into FCCOB register and launch the command
 *
 *  @param commonCommandObject the TFCCOB object contains parameter of fccob register
 *  @return bool - TRUE if Command was written successfully
 *  @note Assumes Flash has been initialized.
 */
 bool LaunchCommand(TFCCOB &commonCommandObject)
{
  while (1)
  {
	if (FTFE_FSTAT & FTFE_FSTAT_CCIF_MASK)
	{
	  if ((FTFE_FSTAT & FTFE_FSTAT_ACCERR_MASK) || (FTFE_FSTAT & FTFE_FSTAT_FPVIOL_MASK))
	  {
        FTFE_FSTAT = 0x30; // clear the old errors
	  }
      else
	  {
 	   // write to FCCOB to load required command parameter
	   // assgin FCMD
        FTFE_FCCOB0 = commonCommandObject.fccob0;

	    // assgin flash address to FCCOB
        FTFE_FCCOB1 = commonCommandObject.fccob1;
        FTFE_FCCOB2 = commonCommandObject.fccob2;
        FTFE_FCCOB3 = commonCommandObject.fccob3;

        // assign data into byte0-7 in FCCOB
        FTFE_FCCOB4 = commonCommandObject.fccobB; //dataByte7
        FTFE_FCCOB5 = commonCommandObject.fccobA; //dataByte6
        FTFE_FCCOB6 = commonCommandObject.fccob4; //dataByte0
        FTFE_FCCOB7 = commonCommandObject.fccob5; //dataByte1
        FTFE_FCCOB8 = commonCommandObject.fccob6; //dataByte2
        FTFE_FCCOB9 = commonCommandObject.fccob7; //dataByte2
        FTFE_FCCOBA = commonCommandObject.fccob8; //dataByte1
        FTFE_FCCOBB = commonCommandObject.fccob9; //dataByte0
	  }

        FTFE_FSTAT = FTFE_FSTAT_CCIF_MASK; //clear CCIF to launch command
        break;
      }
  }
        // wait until the command been processed
        while ( !(FTFE_FSTAT & FTFE_FSTAT_CCIF_MASK))
         ;
        return true;
}

 // Program Phrase command
 bool TFCCOB::WritePhrase(const uint32_t &address, const uint64union_t &phrase)
{
   //union type local var to handle bytes
   uint32union_t word;
   uint16union_t halfWord;

  if (EraseSector(static_cast<uint32_t> (FLASH_DATA_START)) )
  {
  // to access lower 4bytes in phrase
   word.l = phrase.s.Lo;

   // access first, two bytes in phrase
   halfWord.l = word.s.Lo;

   //assign data to parameter of FCCOB
   fccob4 = halfWord.s.Lo; // data Byte 0
   fccob5 = halfWord.s.Hi; // data Byte 1

   // access third , fourth byte in phrase
   halfWord.l = word.s.Hi;

   // assign data to Byte 2, Byte3 in FCCOB
   fccob6 = halfWord.s.Lo; //dataByte2
   fccob7 = halfWord.s.Hi; //dataByte3

   // to access higher 4bytes in phrase
   word.l = phrase.s.Hi;
   halfWord.l = word.s.Lo;

   // access fifth , sixth byte in phase
   halfWord.l = word.s.Lo;

   // assign data to Byte 4, Byte5 in FCCOB
   fccob8 = halfWord.s.Lo; //dataByte4
   fccob9 = halfWord.s.Hi; //dataByte5

   // to access last two bytes in phrase
   halfWord.l = word.s.Hi;
   fccobA = halfWord.s.Lo; //dataByte6
   fccobB = halfWord.s.Hi; //dataByte7

   // FCMD for Program Phrase command
   fccob0 = CMD_PROGRAMPHRASE;

   LaunchCommand(*this);
  }
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
  loMidByte.l = flashAddress.s.Lo;
  fccob3 = loMidByte.s.Lo;
  fccob2 = loMidByte.s.Hi;

  // assign higher byte to fccob1
  fccob1 = flashAddress.s.Hi;

  // FCMD for  Erase Flash Sector Command
  fccob0 = CMD_ERASEFLASHSECTOR;

  return LaunchCommand(*this);
}

#if 0
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
#endif

bool Flash_Erase()
{
  return CommandObject.TFCCOB::EraseSector(static_cast<uint32_t> (FLASH_DATA_START));
}

