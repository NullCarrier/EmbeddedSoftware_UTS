/*! @file
 *
 *  @brief Routines to implement packet encoding and decoding for the serial port.
 *
 *  This contains the functions for implementing the "Tower to PC Protocol" 5-byte packets.
 *
 *  @author : PMcl
 *  @date 01/04/2019
 *  Copyright (c) UTS. All rights reserved.
 */

#ifndef PACKET_H
#define PACKET_H

// New types
#include "type_cpp.h"
#include "UART(2).h" // UART_Init()

// Acknowledgement bit mask
static constexpr uint8_t PACKET_ACK_MASK = 0b10000000;
// constant for handling 3 commands
#define  CMD_STARTUP 0x04
#define  CMD_TOWERVERSION 0x09
#define  CMD_TOWERNUMBER 0x0B

// Packet structure
class Packet_t
{
private:
  uint8_t  Packet_Command;		/*!< The packet's command */
  uint8_t  Packet_Parameter1; 	/*!< The packet's 1st parameter */
  uint8_t  Packet_Parameter2; 	/*!< The packet's 2nd parameter */
  uint8_t  Packet_Parameter3;	/*!< The packet's 3rd parameter */
  uint8_t  Packet_Checksum;	 /*!< The packet's checksum */

  // to initialize the packet module via UART_Init() function
  const uint32_t m_baudRate;
  const uint32_t m_moduleClk;

public:
    // constructor for initializing UART module
   Packet_t(const uint32_t baudRate, const uint32_t moduleClk):
     m_baudRate{baudRate}, m_moduleClk{moduleClk}
   {
      UART_Init(m_baudRate, m_moduleClk);
   }

   // initializing Ackpacket for towerstartup command
  Packet_t(const Packet_t &packet):
Packet_Command(packet.Packet_Command), Packet_Parameter1(packet.Packet_Parameter1), Packet_Parameter2(packet.Packet_Parameter2),
Packet_Parameter3(packet.Packet_Parameter3), m_baudRate(packet.m_baudRate), m_moduleClk(packet.m_moduleClk)
  {
    Packet_Command |= PACKET_ACK_MASK;
    Packet_Command |= CMD_STARTUP;
  }


  // member function of packet
  bool Packet_Get();
  bool Packet_Put();
  void Packet_HandleStartupPacket();
  void Packet_HandleTowerVersionPacket();
  void Packet_HandleTowerNumberPacket();
  inline bool Packet_CheckChecksum(){ return Packet_Checksum ==Packet_Command^Packet_Parameter1^Packet_Parameter2^Packet_Parameter3; } //it is only return
  friend void Packet_HandlePacket(Packet_t &packet);                                                                                                                        //true when check_sum matches
  void Packet_HandleCommandPacket(); // functions for handling packets
  void Packet_SwitchPacket(); // to discard first byte and add the new byte
};


/*! @brief Initializes the packets by calling the initialization routines of the supporting software modules.
 *
 *  @param baudRate The desired baud rate in bits/sec.
 *  @param moduleClk The module clock rate in Hz.
 *  @return bool - TRUE if the packet module was successfully initialized.
 */
//bool Packet_Init(const uint32_t baudRate, const uint32_t moduleClk);

/*! @brief Attempts to get a packet from the received data.
 *
 *  @return bool - TRUE if a valid packet was received.
 */
//bool Packet_Get(void);

/*! @brief Builds a packet and places it in the transmit FIFO buffer.
 *
 *  @return bool - TRUE if a valid packet was sent.
 */
//bool Packet_Put(const uint8_t command, const uint8_t parameter1, const uint8_t parameter2, const uint8_t parameter3);

#endif
