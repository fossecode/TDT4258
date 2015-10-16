#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include "efm32gg.h"

#include "frequencies.h"
#include "tune.h"
#include "songs/imperial_march.h"

#define M_PI 3.14159265358979323846

int	   count = 0;
double noise = 0;

void playSound(double pitch){
    *DAC0_CH0DATA = 0b011111010000;
    *DAC0_CH1DATA = 0b011111010000;
    noise += 2* 3.14 /( 44000/pitch);;
}

void increment(int pitch){
    noise += pitch;
    /*count += 1;
    if(count > 44000/pitch){
        noise = 0;
        count = 0;
      }*/
}


/* TIMER1 interrupt handler */
void __attribute__ ((interrupt)) TIMER1_IRQHandler() 
{  
   // Clear interrupt flag 
   *TIMER1_IFC   = 1;

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
      }
   }
}

/* GPIO even pin interrupt handler */
void __attribute__ ((interrupt)) GPIO_EVEN_IRQHandler() 
{
	// Clear interrupt handler
    *GPIO_IFC = *GPIO_IF;
}

/* GPIO odd pin interrupt handler */
void __attribute__ ((interrupt)) GPIO_ODD_IRQHandler() 
{
	// Clear interrupt flag 
    *GPIO_IFC = *GPIO_IF;
//    *GPIO_DIN = GPIO_PC_BASE << 8;
//    *GPIO_PA_DOUT = 0xff;
}
