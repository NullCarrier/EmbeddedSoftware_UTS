#include "LEDs.h"
#include "MK70F12.h"


 bool LEDsInit()
{
  // Enable the clock gating for PortA
  SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;

  //pin multiplexing
  PORTA_PCR11 |= PORT_PCR_MUX(1);
  PORTB_PCR28 |= PORT_PCR_MUX(1);
  PORTB_PCR29 |= PORT_PCR_MUX(1);
  PORTB_PCR10 |= PORT_PCR_MUX(1);

  //  Enable DSE
  PORTA_PCR11 |= PORT_PCR_DSE_MASK;
  PORTB_PCR28 |= PORT_PCR_DSE_MASK;
  PORTB_PCR29 |= PORT_PCR_DSE_MASK;
  PORTB_PCR10 |= PORT_PCR_DSE_MASK;

  // configure 4 pins as output
  GPIOA_PDDR |= GPIO_PDDR_PDD(11);
  GPIOA_PDDR |= GPIO_PDDR_PDD(28);
  GPIOA_PDDR |= GPIO_PDDR_PDD(29);
  GPIOA_PDDR |= GPIO_PDDR_PDD(10);

  return true;
}

void LED_t::LEDs_On() const
{
  GPIOA_PSOR |= m_color;
}

void LED_t::LEDs_Off() const
{
  GPIOA_PCOR |= m_color;
}

void LED_t::LEDs_Toggle() const
{
  GPIOA_PTOR |= m_color;
}

