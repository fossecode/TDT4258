#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include "efm32gg.h"

#include "frequencies.h"
#include "tune.h"
#include "songs/imperial_march.h"
#define sampleFrequency 32748
//#include "songs/silent.h"




int toneNumber = 0;
int toneDuration  = 0;
double noise = 0;
bool increment = true;
int current_tune = 0;
bool off = true;
double amplitude = 100;
int count = 0;

void playSound(double pitch){
    
    if (increment){
	noise += (1000/((double)sampleFrequency/(double)pitch/4.0));
    }
    else{
       noise -= (1000/((double)sampleFrequency/(double)pitch/4.0));
   }

    *DAC0_CH0DATA = noise;
    *DAC0_CH1DATA = noise;
   if (noise < 0) increment = true;
   else if (noise > amplitude) increment = false;
}

/*Low energy timer */
void __attribute__ ((interrupt)) LETIMER0_IRQHandler()
{
	// Clear interrupt flag 
	*LETIMER0_IFC = 1;

	//playSound(318.0);

    if (off && count < 32768){
		*GPIO_PA_DOUT = 0b11110000 << 8;
        	off = false;
		count = 0;
    }else if (count > 32768){
		*GPIO_PA_DOUT = 0b00001111 << 8;
                off = true;
		count = 0;
	}
	count = count + 1;

	/*switch(current_tune){
		case 0:
			break;
		case 1:*/
	if (imperial_march[toneNumber].frequency != -1)
	{
		playSound(imperial_march[toneNumber].frequency);
		toneDuration += 1;
		if (toneDuration >= imperial_march[toneNumber].duration)
		{
			//Go to next tone.
			toneNumber+=1;
		}
	}
	else
	{
		//Song is finished;
		toneDuration = 0;
	}
        	/*break;
		default:
			break;
	}*/
  
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
          playSound((double)c);
          break;

        case 0b11111101:
          playSound((double)g);
          break;

        case 0b11111011:
          playSound((double)cH);
          break;

        case 0b11110111:
          playSound((double)fH);
          break;
        case 0b11101111:
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
