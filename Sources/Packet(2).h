/*! @file
 *
 *  @brief Routines to implement packet encoding and decoding for the serial port.
 *
 *  This contains the functions for implementing the "Tower to PC Protocol" 5-byte packets.
 *
 *  @author : Chao Li
 *  @date 02/04/2019
 *  Copyright (c) Chao Li. All rights reserved.
 */

#ifndef PACKET_H
#define PACKET_H

// New types
#include "type_cpp.h"
#include "UART(2).h" // UART_Init()

// need to be in enum
// Acknowledgement bit mask
//static constexpr uint8_t PACKET_ACK_MASK = 0b10000000;
extern const uint8_t PACKET_ACK_MASK;
/* constant for handling 3 commands
#define  CMD_STARTUP 0x04
#define  CMD_TOWERVERSION 0x09
#define  CMD_TOWERNUMBER 0x0B */

// Packet structure
class Packet_t
{
private:
  uint8_t  Packet_Command;		/*!< The packet's command */
  uint8_t  Packet_Parameter1; 	/*!< The packet's 1st parameter */
  uint8_t  Packet_Parameter2; 	/*!< The packet's 2nd parameter */
  uint8_t  Packet_Parameter3;	/*!< The packet's 3rd parameter */
  uint8_t  Packet_Checksum;	 /*!< The packet's checksum */
protected:
// to initialize the packet module via UART_Init() function
  const uint32_t m_baudRate;
  const uint32_t m_moduleClk;

public:

/*! @brief Constructor initializes the packets by calling the initialization routines of the supporting software modules.
 *
 *  @param baudRate The desired baud rate in bits/sec.
 *  @param moduleClk The module clock rate in Hz.
 *  @return bool - TRUE if the packet module was successfully initialized.
 */
  Packet_t(const uint32_t baudRate, const uint32_t moduleClk):
     m_baudRate{baudRate}, m_moduleClk{moduleClk}
   {
      UART_Init(m_baudRate, m_moduleClk);
   }

  /*! @brief default constructor
 *
 *  @return None
 */
  Packet_t() = default;

  /*! @brief Attempts to get a packet from the received data.
 *
 *  @return bool - TRUE if a valid packet was received.
 */
  virtual bool Get();

  /*! @brief Builds a packet and places it in the transmit FIFO buffer.
 *
 *  @return bool - TRUE if a valid packet was sent.
 */
  virtual bool Put();

   /*! @brief to handle error condition by discarding first byte and adding the new byte
 *
 *  @return None
 */
  virtual void SwitchPacket();

  friend class HandlePacket;
};

#if 0
class HandlePacket
{
  public:

   enum Command
   {


   };

 /*! @brief There 3 functions below for handling 3 different command packets
 *
 *  @return void
 */
  void HandleStartupPacket();
  void HandleTowerVersionPacket();
  void HandleTowerNumberPacket();

  /*! @brief To determine whether checksum is good or bad
 *
 *  @return bool - TRUE if a checksum is correct
 */
  uint8_t& CheckChecksum();

  /*! @brief There 3 functions below for handling packet protocol and error condition
 *
 * @param &packet the reference to refer to the packet object
 *  @return void , int
 */
  //friend void HandlePacket(Packet_t &packet); // functions for handling ACK packets                                                                                                                       //true when check_sum matches
  int HandleCommandPacket(); // functions for handling packets

};
#endif

#endif
