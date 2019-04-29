/*! @file
 *
 *  @brief Routines to implement packet encoding and decoding for the serial port.
 *
 *  This contains the functions for implementing the "Tower to PC Protocol" 5-byte packets.
 *
 *  @author : Chao Li
 *  @date 29/04/2019
 *  Copyright (c) Chao Li. All rights reserved.
 */

#include "Packet(2).h"


Packet_t::Packet_t(const uint32_t baudRate, const uint32_t moduleClk):
 m_baudRate{baudRate}, m_moduleClk{moduleClk}
 {
  assert(UART_Init(m_baudRate, m_moduleClk));
 }


