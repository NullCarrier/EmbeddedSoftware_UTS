/*! @file
 *
 *  @brief Routines to implement packet encoding and decoding for the serial port.
 *
 *  This contains the functions for implementing the "Tower to PC Protocol" 5-byte packets.
 *
 *  @author PMcL
 *  @date 2015-07-23
 */

#ifndef PACKET_H
#define PACKET_H

// New types
#include "type_cpp.h"
#include "UART(2).h" // UART_Init()


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
      UART_Init(m_baudRate, m_moduleClk) ;
   }

  // member function of packet
  bool Packet_Get();
  bool Packet_Put();
  void HandleStartupPacket();
  void HandleTowerVersionPacket();
  void HandleTowerNumberPacket();
  //it is only return true when check_sum matches
  inline bool Check_Checksum() {return (Packet_Checksum == Packet_Command ^ Packet_Parameter1 ^ Packet_Parameter2 ^ Packet_Parameter3); }
  // functions for handling packets
  bool HandleCommandPacket();
 // void HandlePacket();
 void switch_packet();
};

// Acknowledgement bit mask
extern const uint8_t PACKET_ACK_MASK ;

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
