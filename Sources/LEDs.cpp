/*! @file LEDS.cpp
 *
 *  @brief Routines to access the LEDs on the TWR-K70F120M.
 *
 *  This contains the functions implementtation for operating the LEDs.
 *
 *  @author Chao Li
 *  @date 29/04/2019
 *  Copyright (c) Chao Li. All rights reserved.
 */
#include "LEDs.h"



 bool LED_t::Init()
{
  // Enable the clock gating for PortA
  SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;

  //pin multiplexing
  PORTA_PCR11 |= PORT_PCR_MUX(1);
  PORTA_PCR28 |= PORT_PCR_MUX(1);
  PORTA_PCR29 |= PORT_PCR_MUX(1);
  PORTA_PCR10 |= PORT_PCR_MUX(1);

  //  Enable DSE
  PORTA_PCR11 |= PORT_PCR_DSE_MASK;
  PORTA_PCR28 |= PORT_PCR_DSE_MASK;
  PORTA_PCR29 |= PORT_PCR_DSE_MASK;
  PORTA_PCR10 |= PORT_PCR_DSE_MASK;

  // configure 4 pins as output
  GPIOA_PDDR = GPIO_PDDR_PDD(11);
  GPIOA_PDDR = GPIO_PDDR_PDD(28);
  GPIOA_PDDR = GPIO_PDDR_PDD(29);
  GPIOA_PDDR = GPIO_PDDR_PDD(10);

 /*  // Reset all LED
  GPIOA_PSOR = LEDALL;
 */
  return true;
}

void LED_t::Color(TLED color)
{
  m_color |= color;
}

void LED_t::On() const
{
  GPIOA_PCOR = m_color;
}

void LED_t::Off() const
{
  GPIOA_PSOR = m_color;
}

void LED_t::Toggle() const
{
  GPIOA_PTOR = m_color;
}

