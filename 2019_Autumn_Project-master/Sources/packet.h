/*! @file packet.h
 *
 *  @brief Routines to implement packet encoding and decoding for the serial port.
 *
 *  This contains the functions for implementing the "Tower to PC Protocol" 5-byte packets.
 *
 *  @author : Chao Li
 *  @date 08/06/2019
 *  Copyright (c) Chao Li. All rights reserved.
 */

#ifndef PACKET_H
#define PACKET_H

// New types
#include "type_cpp.h"

// _EI() _DI()
#include "PE_Types.h"

#include "Cpu.h"

#include "UART.h"

// Packet structure
const uint8_t PACKET_NB_BYTES = 5;

#pragma pack(push)
#pragma pack(1)

typedef union
{
  uint8_t bytes[PACKET_NB_BYTES];     /*!< The packet as an array of bytes. */
  struct
  {
    uint8_t command;		      
    union
    {
      struct
      {
        uint8_t parameter1;	      
        uint8_t parameter2;	      
        uint8_t parameter3;	      
      } separate;
      struct
      {
        uint16_t parameter12;         /*!< Parameter 1 and 2 concatenated. */
        uint8_t parameter3;
      } combined12;
      struct
      {
        uint8_t paramater1;
        uint16_t parameter23;         /*!< Parameter 2 and 3 concatenated. */
      } combined23;
    } parameters;
    uint8_t checksum;
  } packetStruct;
} TPacket;

#pragma pack(pop)

#if 0
#define Packet_Command     Packet_t::s_Packet.packetStruct.command
#define Packet_Parameter1  Packet_t::s_Packet.packetStruct.parameters.separate.parameter1
#define Packet_Parameter2  Packet_t::s_Packet.packetStruct.parameters.separate.parameter2
#define Packet_Parameter3  Packet_t::s_Packet.packetStruct.parameters.separate.parameter3
#define Packet_Parameter12 Packet_t::s_Packet.packetStruct.parameters.combined12.parameter12
#define Packet_Parameter23 Packet_t::s_Packet.packetStruct.parameters.combined23.parameter23
#define Packet_Checksum    Packet_t::s_Packet.packetStruct.checksum
#endif

//

class Packet_t: public UART_t
{
  private:
    uint8_t command; /*!< The packet's command. */
    uint8_t parameter1; /*!< The packet's 1st parameter. */
    uint8_t parameter2; /*!< The packet's 2nd parameter. */
    uint8_t parameter3; /*!< The packet's 3rd parameter. */
    uint8_t checksum; /*!< The packet's checksum. */

  public:
  //static TPacket s_Packet; /*!< The static variable packet with TPacket type */

  /*! @brief Initializes the packets by calling the initialization routines of the supporting software modules.
 *
 *  @param baudRate The desired baud rate in bits/sec.
 *  @param moduleClk The module clock rate in Hz.
 *  @return None
 */
   Packet_t(const uint32_t baudRate, const uint32_t moduleClk):
   UART_t::UART_t(baudRate, moduleClk)
   {
   }
   
   /*! @brief Initializes empty packet object for any other threads
    *
    *
    */
   Packet_t():
   UART_t::UART_t()
   {
   }
   
   
  /*! @brief Attempts to get a packet from the received data.
 *
 *  @return bool - TRUE if a valid packet was received.
 */
   bool PacketGet();

/*! @brief Builds a packet and places it in the transmit FIFO buffer.
 *  @note this pakcetPut function is used for all modules
 *
 *  @return bool - TRUE if a valid packet was sent.
 */
   bool PacketPut(uint8_t command, uint8_t parameter1, uint8_t parameter2, uint8_t parameter3);
   

  /*! @brief to handle error condition by discarding first byte and adding the new byte
 *
 *  
 */
   void SwitchPacket();

  /*! @brief To bulid a checksum
 *
 *  @return uint8_t&&  a rvalue reference to anonymous variable holding checksum
 */
   uint8_t MakeChecksum();
   
  friend class HandlePacket; 
};


// Acknowledgment bit mask
//extern const uint8_t PACKET_ACK_MASK;



#endif
