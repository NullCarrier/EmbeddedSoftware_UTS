/*! @file
 *
 *  @brief Routines to implement packet encoding and decoding for the serial port.
 *
 *  This contains the functions for implementing the "Tower to PC Protocol" 5-byte packets.
 *
 *  @author : Chao Li
 *  @date 25/04/2019
 *  Copyright (c) Chao Li. All rights reserved.
 */

#ifndef PACKET2_H
#define PACKET2_H

// New types
#include "types.h"
#include "UART(2).h" // UART_Init()
#include <cassert>

// need to be in enum
// Acknowledgement bit mask
//static constexpr uint8_t PACKET_ACK_MASK = 0b10000000;
//extern const uint8_t PACKET_ACK_MASK;


// Packet structure
class Packet_t
{

protected:
// to initialize the packet module via UART_Init() function
  const uint32_t m_baudRate; /*!< The baudRate for UART module */
  const uint32_t m_moduleClk; /*!< The moduleClock for UART module */

public:

 /*! @brief Constructor initializes the packets by calling the initialization routines of the supporting software modules.
 *
 *  @param baudRate The desired baud rate in bits/sec.
 *  @param moduleClk The module clock rate in Hz.
 *  @return bool - TRUE if the packet module was successfully initialized.
 */
  Packet_t(const uint32_t baudRate, const uint32_t moduleClk);

  /*! @brief Attempts to get a packet from the received data.
 *
 *  @return bool - TRUE if a valid packet was received.
 */
 virtual bool Packet_Get() = 0;

  /*! @brief Builds a packet and places it in the transmit FIFO buffer.
 *
 *  @return bool - TRUE if a valid packet was sent.
 */
 virtual bool Packet_Put() = 0;

   /*! @brief to handle error condition by discarding first byte and adding the new byte
 *
 *  @return None
 */
 virtual void SwitchPacket() = 0;

  /*! @brief To determine whether checksum is good or bad
     *
     *  @return bool - TRUE if a checksum is correct
    */
 virtual uint8_t MakeChecksum() = 0;

};


#endif
