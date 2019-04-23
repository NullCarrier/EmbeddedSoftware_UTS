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
#include "types.h"
//include deque
//#include <deque>

//include packet module in Lab1
#include "Packet(2).h"



// Packet structure
static constexpr uint8_t PACKET_NB_BYTES =  5;

#pragma pack(push)
#pragma pack(1)

typedef union
{
  uint8_t bytes[PACKET_NB_BYTES];     /*!< The packet as an array of bytes. */
  struct
  {
    uint8_t command;		      /*!< The packet's command. */
    union
    {
      struct
      {
        uint8_t parameter1;	      /*!< The packet's 1st parameter. */
        uint8_t parameter2;	      /*!< The packet's 2nd parameter. */
        uint8_t parameter3;	      /*!< The packet's 3rd parameter. */
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

#define Packet_Command     PacketVer2_t::s_Packet.packetStruct.command
#define Packet_Parameter1  PacketVer2_t::s_Packet.packetStruct.parameters.separate.parameter1
#define Packet_Parameter2  PacketVer2_t::s_Packet.packetStruct.parameters.separate.parameter2
#define Packet_Parameter3  PacketVer2_t::s_Packet.packetStruct.parameters.separate.parameter3
#define Packet_Parameter12 PacketVer2_t::s_Packet.packetStruct.parameters.combined12.parameter12
#define Packet_Parameter23 PacketVer2_t::s_Packet.packetStruct.parameters.combined23.parameter23
#define Packet_Checksum    PacketVer2_t::s_Packet.packetStruct.checksum

class PacketVer2_t : public Packet_t
{
  public:
     static TPacket s_Packet;

  /*! @brief Initializes the packets by calling the initialization routines of the supporting software modules.
 *
 *  @param baudRate The desired baud rate in bits/sec.
 *  @param moduleClk The module clock rate in Hz.
 *  @return None
 */
   PacketVer2_t(const uint32_t baudRate, const uint32_t moduleClk):
     Packet_t::Packet_t(baudRate, moduleClk)
      {
      }
  /*! @brief Attempts to get a packet from the received data.
 *
 *  @return bool - TRUE if a valid packet was received.
 */
   bool Packet_Get() override;

  /*! @brief Builds a packet and places it in the transmit FIFO buffer.
 *
 *  @return bool - TRUE if a valid packet was sent.
 */
   bool Packet_Put() override;

  /*! @brief to handle error condition by discarding first byte and adding the new byte
 *
 *  @return None
 */
   void SwitchPacket() override;
  /*! @brief To bulid a checksum
 *
 *  @return None
 */
  inline static void MakeChecksum();

};


class HandlePacketVer2
{
  public:
    enum Command
    {
     CMD_STARTUP = 0x04,
     CMD_TOWERVERSION = 0x09,
     CMD_TOWERNUMBER = 0x0B,
     CMD_TOWERMODE = 0x0D,
     CMD_MYTOWERNUMBER = 0x9434
    };

  /*! @brief There 3 functions below for handling 3 different command packets
   *
   *  @return void
  */
  static  void HandleStartupPacket(PacketVer2_t &packet);
  static  void HandleTowerVersionPacket(PacketVer2_t &packet);
  static  void HandleTowerNumberPacket(PacketVer2_t &packet);
  static  void HandleTowerMode(PacketVer2_t &packet);
  static  void InitResponsePacket(PacketVer2_t &packet);

  static void HandleCommandPacket(PacketVer2_t &packet); // functions for handling packets

};


// Acknowledgment bit mask
extern const uint8_t PACKET_ACK_MASK;

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
