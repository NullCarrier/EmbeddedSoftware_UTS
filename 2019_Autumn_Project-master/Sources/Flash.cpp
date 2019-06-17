/*! @file
 *
 *  @brief Routines for erasing and writing to the Flash.
 *
 *  This contains the functions implementation needed for accessing the internal Flash.
 *
 *  @author Chao Li
 *  @date 09/06/2019
 *  Copyright (c) Chao Li. All rights reserved.
*/
#include "Flash.h"

const uint8_t NB_BYTES  = 8;

// declare array to keep track of flash memory
static bool FlashSector0[NB_BYTES];


/*! @brief To allocate memory address to data with a byte and keep track of memory space by
 *         the vaule of element:
 *         1 signify falsh memory has been written in the postion indicated by index of array
 *         0 signify falsh memory is avaiable in the postion indicated by index of array
 *  @param variable the address of a pointer to a variable that is to be allocated space in Flash memory.
 *  @return bool - TRUE if corresponding memory address has been assgined to pointer
 *  @note Assumes Flash has been initialized.
 */
static bool FlashAllocateByte(volatile void** variable)
{
  for (uint64_t i = 0; i < NB_BYTES; i++)
  {
	if (FlashSector0[i] == 0)
	{
	  *variable = (void *) (FLASH_DATA_START + i);
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
static bool FlashAllocateHalfWord(volatile void** variable)
{
  for (uint64_t i = 0; i < NB_BYTES;i++)
  {
	if ((FlashSector0[i] == 0) && (i % 2) == 0 )
	{
	  *variable = (void *) (FLASH_DATA_START + i);

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

bool Flash::AllocateVar(volatile void** variable, const uint8_t &size)
{
  switch (size)
  {
    case 1: 
	  return FlashAllocateByte(variable); 
    case 2: 
	  return FlashAllocateHalfWord(variable);
    case 4: 
	  //FlashAllocateWord(variable);
      break;
  }

  return false;
}


bool Flash::Write32(volatile uint32_t* const address, const uint32_t &data)
{
  // local var for handling bytes
  uint64union_t phrase;

   //First read the whole 32 bits of interest, into a temp variable.
  uint32_t *address32Start = (uint32_t*) (address-1);

  uint64_t *add64Bit;

  //Then overwrite either the top or lower 32 bits.
  if(((uint32_t) address) % 8 == 0)
  {
     phrase.s.Lo = data;
     phrase.s.Hi = _FW(address + 1);
     add64Bit = (uint64_t*) address;
  }
  else
  {
     phrase.s.Lo = _FW(address - 1);
     phrase.s.Hi = data;
     add64Bit = (uint64_t*) address32Start;
  }

  return this->WritePhrase((uint32_t) add64Bit, phrase);
}


bool Flash::Write16(volatile uint16_t* const address, const uint16_t &data)
{
  uint32union_t data4Bytes;
  
  //First read the whole 16 bits of interest, into a temp variable.
  uint16_t *address16Start = (uint16_t*) (address-1);
  
  //Storage and pointer variable for Write32
  uint32_t *add32Bit;
  
  
  if(( (uint32_t) address ) % 4 == 0)
  {
	//Rewrite the lower 2bytes
    data4Bytes.s.Lo = data;
    data4Bytes.s.Hi = _FH(address + 1);
    add32Bit = (uint32_t*) address;
  }
  else
  {
	 //Rewrite the higher 2bytes
     data4Bytes.s.Lo = _FH(address - 1);
     data4Bytes.s.Hi = data;
     
     add32Bit = (uint32_t*) address16Start;
  }
 

  return this->Write32(add32Bit, data4Bytes.l);
}


bool Flash::Write8(volatile uint8_t* const address, const uint8_t &data)
{
  uint16union_t data2Bytes;
  uint8_t *address8Start;
  uint16_t *add16Bit;
  
  address8Start = (uint8_t*) (address-1);

  if(( (uint32_t) address ) % 2 == 0)//?
  {
	//Rewrite the lower byte
    data2Bytes.s.Lo = data;
    data2Bytes.s.Hi = _FB(address + 1);
    add16Bit = (uint16_t*) address;
  }
  else
  {
	//Rewrite the higher byte
    data2Bytes.s.Lo = _FB(address - 1);
    data2Bytes.s.Hi = data;
    //reset address to start of 16bit fields, make even to pass to write16
    add16Bit = (uint16_t*) address8Start;
  }

  return this->Write16(add16Bit, data2Bytes.l);
}

/*! @brief Writes the command into FCCOB register and launch the command
 *
 *  @return bool - TRUE if Command was written successfully
 *  @note Assumes Flash has been initialized.
 */
 bool TFCCOB::LaunchCommand()
{
  while (1)
  {
	if (FTFE_FSTAT & FTFE_FSTAT_CCIF_MASK)
	{
	  if ((FTFE_FSTAT & FTFE_FSTAT_ACCERR_MASK) || (FTFE_FSTAT & FTFE_FSTAT_FPVIOL_MASK))
        FTFE_FSTAT = 0x30; // clear the old errors
      else
	  {
 	   // write to FCCOB to load required command parameter
	   // assgin FCMD
        FTFE_FCCOB0 = fccob0;

	    // assgin flash address to FCCOB
        FTFE_FCCOB1 = fccob1;
        FTFE_FCCOB2 = fccob2;
        FTFE_FCCOB3 = fccob3;

        // assign data into byte0-7 in FCCOB
        FTFE_FCCOB4 = fccobB; //dataByte7
        FTFE_FCCOB5 = fccobA; //dataByte6
        FTFE_FCCOB6 = fccob5; //dataByte0
        FTFE_FCCOB7 = fccob4; //dataByte1
        FTFE_FCCOB8 = fccob6; //dataByte2
        FTFE_FCCOB9 = fccob7; //dataByte2
        FTFE_FCCOBA = fccob8; //dataByte1
        FTFE_FCCOBB = fccob9; //dataByte0
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

  // local union_t to handle bytes
  uint32union_t flashAddress;
  uint16union_t loMidByte;

  if (EraseSector( (uint32_t) FLASH_DATA_START) )
  {
      // using union to hold memory address
     flashAddress.l = address;

      // assgin lower and mid byte to fccob3 , fccob2
     loMidByte.l = flashAddress.s.Lo;
     fccob3 = loMidByte.s.Lo;
     fccob2 = loMidByte.s.Hi;

      // assign higher byte to fccob1
     fccob1 = flashAddress.s.Hi;

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

    this->LaunchCommand();
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

  return this->LaunchCommand();
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

bool Flash::Erase()
{
  return this->TFCCOB::EraseSector((uint32_t) FLASH_DATA_START);
}

