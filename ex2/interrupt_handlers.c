#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include "efm32gg.h"

#include "frequencies.h"
#include "tune.h"
#include "songs/imperial_march.h"
#include "songs/silent.h"

int	   count = 0;
int    timer1  = 0;
double noise = 0;
int current_tune = 0;


void playSound(double pitch){
    *DAC0_CH0DATA = noise +2000;
    *DAC0_CH1DATA = noise +2000;
    noise += 44000/pitch;
    if (noise > 400)
    {
      noise = 0;
    }
}


/* TIMER1 interrupt handler */
void __attribute__ ((interrupt)) TIMER1_IRQHandler() 
{  
   // Clear interrupt flag 
   *TIMER1_IFC   = 1;

   switch(current_tune){
      case 0:
        break;
      case 1:
        if (imperial_march[count].frequency != -1)
        {
           playSound(imperial_march[count].frequency);
           timer1 += 1;
           if (timer1 > imperial_march[count].duration)
        {
          count+=1;
        }

   }
   if (imperial_march[count].frequency == -1)
   {
     count = 0;
     timer1 = 0;
   }
        break;
   }
  
/*
   if (imperial_march[count].frequency != -1)
   {
      playSound(imperial_march[count].frequency);
      timer1 += 1;
      if (timer1 > imperial_march[count].duration)
      {
        count+=1;
      }

   }
   if (imperial_march[count].frequency == -1)
   {
     count = 0;
     timer1 = 0;
   }*/



   // 0bxxxxxxxx11111110
   // 0b0000000000000001
   if ((*GPIO_PC_DIN ^ 0b11111111) != 0) {

   	  switch (*GPIO_PC_DIN) {
        case 0b11111110:
          playSound((double)a);
          break;

        case 0b11111101:
          playSound((double)b);
          break;

        case 0b11111011:
          playSound((double)g);
          break;

        case 0b11110111:
          playSound((double)aH);
          break;
        case 0b11101111:
          playSound((double)f);
          break;
      }
   }
}

/* GPIO even pin interrupt handler */
void __attribute__ ((interrupt)) GPIO_EVEN_IRQHandler() 
{
	// Clear interrupt handler
    *GPIO_IFC = *GPIO_IF;
    *GPIO_PA_DOUT = *GPIO_PC_DIN << 8;
    if (*GPIO_PC_DIN == 0b10111111)
    {
      current_tune = 0;
    }

}

/* GPIO odd pin interrupt handler */
void __attribute__ ((interrupt)) GPIO_ODD_IRQHandler() 
{
	// Clear interrupt flag 
    *GPIO_IFC = *GPIO_IF;
    *GPIO_PA_DOUT = *GPIO_PC_DIN << 8;
//    *GPIO_DIN = GPIO_PC_BASE << 8;
//    *GPIO_PA_DOUT = 0xff;
    if (*GPIO_PC_DIN == 0b01111111)
    {
      current_tune = 1;
    }
}
