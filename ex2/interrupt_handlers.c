#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"

int noise = 0;

/* TIMER1 interrupt handler */
void __attribute__ ((interrupt)) TIMER1_IRQHandler() 
{  
  /*
    TODO feed new samples to the DAC
    remember to clear the pending interrupt by writing 1 to TIMER1_IFC
  */  
   *TIMER1_IFC   = 1;
//   *DAC0_CH0DATA = 0x337;
//   *DAC0_CH1DATA = 0x337;
   *DAC0_CH0DATA = noise;
  *DAC0_CH1DATA = noise;
//   *GPIO_PA_DOUT += 4096;
   *TIMER1_TOP = 200;
   *GPIO_PA_DOUT = 0b1010101000000000;
}

/* GPIO even pin interrupt handler */
void __attribute__ ((interrupt)) GPIO_EVEN_IRQHandler() 
{
    /* TODO handle button pressed event, remember to clear pending interrupt */
    *GPIO_IFC = *GPIO_IF;
//    *GPIO_DIN = GPIO_PC_BASE << 8;
//    *GPIO_PA_DOUT = 0b111111111111111;
    noise += 250;
}

/* GPIO odd pin interrupt handler */
void __attribute__ ((interrupt)) GPIO_ODD_IRQHandler() 
{
    /* TODO handle button pressed event, remember to clear pending interrupt */
    /* TODO handle button pressed event, remember to clear pending interrupt */
    *GPIO_IFC = *GPIO_IF;
//    *GPIO_DIN = GPIO_PC_BASE << 8;
//    *GPIO_PA_DOUT = 0xff;
}
