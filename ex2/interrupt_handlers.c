#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>

#include "efm32gg.h"

#define c 261
#define d 294
#define e 329
#define f 349
#define g 391
#define gS 415
#define a 440
#define aS 455
#define b 466
#define cH 523
#define cSH 554
#define dH 587
#define dSH 622
#define eH 659
#define fH 698
#define fSH 740
#define gH 784
#define gSH 830
#define aH 880

int count = 0;
double noise = 0;

void playSound(double sample){
    *DAC0_CH0DATA = cos((1*sample));
    *DAC0_CH1DATA = cos((1*sample));
}

void increment(int pitch){
    noise += 50;
    /*count += 1;
    if(count > 44000/pitch){
        noise = 0;
        count = 0;
      }*/
}


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

   // 0bxxxxxxxx11111110
   // 0b0000000000000001
   if ((*GPIO_PC_DIN ^ 0b11111111) != 0) {
   	  switch (*GPIO_PC_DIN){
        case 0b11111110:
          playSound(noise);
          increment(a);

        case 0b11111101:
          playSound(noise);
          increment(g);

        case 0b11111011:
          playSound(noise);
          increment(d);

        case 0b11110111:
          playSound(noise);
          increment(c);
        
      }


   }
 
//   *GPIO_PA_DOUT += 4096;

   // LEDS
}


/* GPIO even pin interrupt handler */
void __attribute__ ((interrupt)) GPIO_EVEN_IRQHandler() 
{
    /* TODO handle button pressed event, remember to clear pending interrupt */
    *GPIO_IFC = *GPIO_IF;

    // Determine if rise or fall
    
//    *GPIO_DIN = GPIO_PC_BASE << 8;
//    *GPIO_PA_DOUT = 0b111111111111111;
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
