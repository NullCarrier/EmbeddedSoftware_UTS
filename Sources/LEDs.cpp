#include "LEDs.h"
#include "MK70F12.h"


static bool LEDsInit(void)
{
  // Enable the clock gating for PortA	
  SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;
  
  //pin multiplexing	
  PORTA_PCR11 |= PORT_PCR_MUX(1);
  PORTB_PCR28 |= PORT_PCR_MUX(1);
  PORTB_PCR29 |= PORT_PCR_MUX(1);
  PORTB_PCR10 |= PORT_PCR_MUX(1);
  
  // configure 4 pins as out
  GPIOA_PDDR |= GPIO_PDDR_PDD(11);  
  GPIOA_PDDR |= GPIO_PDDR_PDD(28); 
  GPIOA_PDDR |= GPIO_PDDR_PDD(29); 
  GPIOA_PDDR |= GPIO_PDDR_PDD(10); 
  
  return true;
}

void LEDs_On()
{
  switch (m_color)
  { 
    case LED_ORANGE: GPIO_PSOR_PTSO_MASK |= GPIO_PSOR_PTSO(11);
            break;
    case LED_YELLOW: GPIO_PSOR_PTSO_MASK |= GPIO_PSOR_PTSO(28);
	        break;
	case LED_GREEN: GPIO_PSOR_PTSO_MASK |= GPIO_PSOR_PTSO(29);
	        break;
	case LED_BLUE: GPIO_PSOR_PTSO_MASK |= GPIO_PSOR_PTSO(10);
	        break;
  }	
  
}

void LEDs_off()
{
  switch (m_color)
  { 
    case LED_ORANGE: GPIO_PCOR_PTCO_MASK |= GPIO_PCOR_PTCO(11);
            break;
    case LED_YELLOW: GPIO_PCOR_PTCO_MASK |= GPIO_PCOR_PTCO(28);
	        break;
	case LED_GREEN: GPIO_PCOR_PTCO_MASK |= GPIO_PCOR_PTCO(29);
	        break;
	case LED_BLUE: GPIO_PCOR_PTCO_MASK |= GPIO_PCOR_PTCO(10);
	        break;
  }	
  
}

void LEDs_Toggle()
{
  switch (m_color)
  { 
    case LED_ORANGE: GPIO_PTOR_PTTO_MASK |= GPIO_PTOR_PTTO(11);
            break;
    case LED_YELLOW: GPIO_PTOR_PTTO_MASK |= GPIO_PTOR_PTTO(28);
	        break;
	case LED_GREEN: GPIO_PTOR_PTTO_MASK |= GPIO_PTOR_PTTO(29);
	        break;
	case LED_BLUE: GPIO_PTOR_PTTO_MASK |= GPIO_PTOR_PTTO(10);
	        break;
  }	
  
}




