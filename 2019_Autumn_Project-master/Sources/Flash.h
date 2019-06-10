/*! @file
 *
 *  @brief Routines for erasing and writing to the Flash.
 *
 *  This contains the functions needed for accessing the internal Flash.
 *
 *  @author Chao Li
 *  @date 25/04/2019
 *  Copyright (c) Chao Li. All rights reserved.
*/

#ifndef FLASH_H
#define FLASH_H

// new types
#include "type_cpp.h"
// include mask
#include "MK70F12.h"


// FLASH data access, reading bytes from flash in main routine
#define _FB(flashAddress)  *(uint8_t  volatile *)(flashAddress)// bytes
#define _FH(flashAddress)  *(uint16_t volatile *)(flashAddress)// half of word
#define _FW(flashAddress)  *(uint32_t volatile *)(flashAddress)// words
#define _FP(flashAddress)  *(uint64_t volatile *)(flashAddress)//phase

// Address of the start of the Flash block we are using for data storage
#define FLASH_DATA_START 0x00080000LU
// Address of the end of the Flash block we are using for data storage
#define FLASH_DATA_END   0x00080007LU


// for handling two commands Program Phrase , Erase Flash Sector
class TFCCOB 
{
  private:
   uint8_t fccob0; /*!< The fccob number 0 */ //Command
   uint8_t fccob1; /*!< The fccob number 1 */ //[23:16]
   uint8_t fccob2; /*!< The fccob number 2 */ //[15:8]
   uint8_t fccob3; /*!< The fccob number 3 */ //[7:0]
   uint8_t fccob4; /*!< The fccob number 4 */
   uint8_t fccob5; /*!< The fccob number 5 */
   uint8_t fccob6; /*!< The fccob number 6 */
   uint8_t fccob7; /*!< The fccob number 7 */
   uint8_t fccob8; /*!< The fccob number 8 */
   uint8_t fccob9; /*!< The fccob number 9 */
   uint8_t fccobA; /*!< The fccob number A */
   uint8_t fccobB; /*!< The fccob number B */


  protected:
    enum Command_FCCOB
    {
      CMD_PROGRAMPHRASE = 0x07,
      CMD_ERASEFLASHSECTOR = 0x09
    };

    TFCCOB() = default;

/*! @brief the command Erase flash secctor to erase whole sector
 *  @param address The address of the data.
 *  @return bool - TRUE if the Flash sector was erased successfully
 *  @note Assumes Flash has been initialized.
 */
    bool EraseSector(const uint32_t &address);

/*! @brief to write phrase into flash sector
 *  @param address The address of the data.
 *  @param phase the 8 byte data
 *  @return bool - TRUE if the phrase was written successfully
 *  @note Assumes Flash has been initialized.
 */
    bool WritePhrase(const uint32_t &address, const uint64union_t &phase);


    bool LaunchCommand();
};

class Flash: public TFCCOB  
{
 
  public:
    Flash():
    TFCCOB()
    {
    }

/*! @brief Enables the Flash module.
 *
 *  @return bool - TRUE if the Flash was setup successfully.
 */
    inline bool Init() {return true;}
	
	/*! @brief Allocates space for a non-volatile variable in the Flash memory.
 *
 *  @param variable is the address of a pointer to a variable that is to be allocated space in Flash memory.
 *         The pointer will be allocated to a relevant address:
 *         If the variable is a byte, then any address.
 *         If the variable is a half-word, then an even address.
 *         If the variable is a word, then an address divisible by 4.
 *         This allows the resulting variable to be used with the relevant Flash_Write function which assumes a certain memory address.
 *         e.g. a 16-bit variable will be on an even address
 *  @param size The size, in bytes, of the variable that is to be allocated space in the Flash memory. Valid values are 1, 2 and 4.
 *  @return bool - TRUE if the variable was allocated space in the Flash memory.
 *  @note Assumes Flash has been initialized.
 */
    bool AllocateVar(volatile void** variable, const uint8_t &size);
	
	/*! @brief Writes a 32-bit number to Flash.
 *
 *  @param address The address of the data.
 *  @param data The 32-bit data to write.
 *  @return bool - TRUE if Flash was written successfully, FALSE if address is not aligned to a 4-byte boundary or if there is a programming error.
 *  @note Assumes Flash has been initialized.
 */
    bool Write32(volatile uint32_t* const address, const uint32_t &data);
	
/*! @brief Writes a 16-bit number to Flash.
 *
 *  @param address The address of the data.
 *  @param data The 16-bit data to write.
 *  @return bool - TRUE if Flash was written successfully, FALSE if address is not aligned to a 2-byte boundary or if there is a programming error.
 *  @note Assumes Flash has been initialized.
 */
    bool Write16(volatile uint16_t* const address, const uint16_t &data);
	
/*! @brief Writes an 8-bit number to Flash.
 *
 *  @param address The address of the data.
 *  @param data The 8-bit data to write.
 *  @return bool - TRUE if Flash was written successfully, FALSE if there is a programming error.
 *  @note Assumes Flash has been initialized.
 */
    bool Write8(volatile uint8_t* const address, const uint8_t &data);
/*! @brief Erases the entire Flash sector.
 *
 *  @return bool - TRUE if the Flash "data" sector was erased successfully.
 *  @note Assumes Flash has been initialized.
 */
    bool Erase(void);

};






//inline bool Flash_Init(void);

/*! @brief Allocates space for a non-volatile variable in the Flash memory.
 *
 *  @param variable is the address of a pointer to a variable that is to be allocated space in Flash memory.
 *         The pointer will be allocated to a relevant address:
 *         If the variable is a byte, then any address.
 *         If the variable is a half-word, then an even address.
 *         If the variable is a word, then an address divisible by 4.
 *         This allows the resulting variable to be used with the relevant Flash_Write function which assumes a certain memory address.
 *         e.g. a 16-bit variable will be on an even address
 *  @param size The size, in bytes, of the variable that is to be allocated space in the Flash memory. Valid values are 1, 2 and 4.
 *  @return bool - TRUE if the variable was allocated space in the Flash memory.
 *  @note Assumes Flash has been initialized.
 */
//bool Flash_AllocateVar(volatile void** variable, const uint8_t &size);

/*! @brief Writes a 32-bit number to Flash.
 *
 *  @param address The address of the data.
 *  @param data The 32-bit data to write.
 *  @return bool - TRUE if Flash was written successfully, FALSE if address is not aligned to a 4-byte boundary or if there is a programming error.
 *  @note Assumes Flash has been initialized.
 */

//  write to RAM buffer
//bool Flash_Write32(volatile uint32_t* const address, const uint32_t &data);

/*! @brief Writes a 16-bit number to Flash.
 *
 *  @param address The address of the data.
 *  @param data The 16-bit data to write.
 *  @return bool - TRUE if Flash was written successfully, FALSE if address is not aligned to a 2-byte boundary or if there is a programming error.
 *  @note Assumes Flash has been initialized.
 */
//bool Flash_Write16(volatile uint16_t* const address, const uint16_t &data);

/*! @brief Writes an 8-bit number to Flash.
 *
 *  @param address The address of the data.
 *  @param data The 8-bit data to write.
 *  @return bool - TRUE if Flash was written successfully, FALSE if there is a programming error.
 *  @note Assumes Flash has been initialized.
 */
//bool Flash_Write8(volatile uint8_t* const address, const uint8_t &data);


/*! @brief Erases the entire Flash sector.
 *
 *  @return bool - TRUE if the Flash "data" sector was erased successfully.
 *  @note Assumes Flash has been initialized.
 */
//bool Flash_Erase(void);

/*! @brief Read data from flash memory
 *
 *  @return bool - TRUE if the Flash "data" sector was read successfully.
 *  @note Assumes Flash has been initialized.
 */
//bool Flash_Read(void);

#endif
