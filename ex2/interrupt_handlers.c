#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include "efm32gg.h"

#include "frequencies.h"
#include "tune.h"
#include "songs/imperial_march.h"
#include "songs/mario.h"
#include "songs/the_final_countdown.h"
#define sampleFrequency 5700.0


int toneNumber = 0;
int toneDuration  = 0;
double noise = 0;
bool increment = true;
int current_tune = 0;
bool off = true;
double amplitude = 1000.0;
int count = 0;

void playSound(double pitch){
  noise += (amplitude/(sampleFrequency/pitch)); 
  /*if (increment){
	   noise += (amplitude/(sampleFrequency/pitch));
  }
  else{
    noise -= (amplitude/(sampleFrequency/pitch));
  }*/

  *DAC0_CH0DATA = (int)floor(noise);
  *DAC0_CH1DATA = (int)floor(noise);
  //if (noise < 0) increment = true;
  if (noise > amplitude) noise = 0;
}

/*Low energy timer */
void __attribute__ ((interrupt)) LETIMER0_IRQHandler()
{
	// Clear interrupt flag 
	*LETIMER0_IFC = 1;

	//playSound(318.0);

  /*if (off && count > sampleFrequency){
		*GPIO_PA_DOUT = 0b11110000 << 8;
        	off = false;
		count = 0;
    }else if (count > sampleFrequency){
		*GPIO_PA_DOUT = 0b00001111 << 8;
                off = true;
		count = 0;
	}
	count = count + 1;*/

	switch(current_tune){
		case 0:
      if (imperial_march[toneNumber].frequency != -1 ){
        if (imperial_march[toneNumber].frequency){
          playSound((double)imperial_march[toneNumber].frequency);
        }
        toneDuration += 6;
        if (toneDuration >= imperial_march[toneNumber].duration){
          //Go to next tone.
          toneNumber+=1;
          toneDuration = 0;
        }
      }
      else{
        //Song is finished;
        toneDuration = 0;
        toneNumber = 0;
        current_tune = -1;
      } 
			break;
		case 1:
      if (pew_pew[toneNumber].frequency != -1 ){
        if (pew_pew[toneNumber].frequency){
          playSound((double)pew_pew[toneNumber].frequency);
        }
        toneDuration += 6;
        if (toneDuration >= pew_pew[toneNumber].duration){
          //Go to next tone.
          toneNumber+=1;
          toneDuration = 0;
        }
      }
    case 2:
      if (the_final_countdown[toneNumber].frequency != -1 ){
        if (the_final_countdown[toneNumber].frequency){
          playSound((double)the_final_countdown[toneNumber].frequency);
        }
        toneDuration += 6;
        if (toneDuration >= the_final_countdown[toneNumber].duration){
          //Go to next tone.
          toneNumber+=1;
          toneDuration = 0;
        }
      }
    case 3:
      if (mario[toneNumber].frequency != -1 ){
        if (mario[toneNumber].frequency){
          playSound((double)mario[toneNumber].frequency);
        }
        toneDuration += 6;
        if (toneDuration >= mario[toneNumber].duration){
          //Go to next tone.
          toneNumber+=1;
          toneDuration = 0;
        }
      }

      else{
        //Song is finished;
        toneDuration = 0;
        toneNumber = 0;
        current_tune = -1;
      } 
      break;

 
   



   // 0bxxxxxxxx11111110
   // 0b0000000000000001

/*   if ((*GPIO_PC_DIN ^ 0b11111111) != 0) {

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
   }*/
}

/* GPIO even pin interrupt handler */
void __attribute__ ((interrupt)) GPIO_EVEN_IRQHandler() 
{
	// Clear interrupt handler
    *GPIO_IFC = *GPIO_IF;
    *GPIO_PA_DOUT = *GPIO_PC_DIN << 8;
    switch(*GPIO_PC_DIN)
      case 0b10111111:
        amplitude -= 100;
        break;
      case 0b11111110:
        current_tune = 0;
        break;
      case 0b11111101:
        current_tune = 1;
        break;
      case 0b11111011:
        current_tune = 2;
        break;
      case 0b11110111:
        current_tune = 3;
        break;

}

/* GPIO odd pin interrupt handler */
void __attribute__ ((interrupt)) GPIO_ODD_IRQHandler() 
{
	// Clear interrupt flag 
    *GPIO_IFC = *GPIO_IF;
    *GPIO_PA_DOUT = *GPIO_PC_DIN << 8;
//    *GPIO_DIN = GPIO_PC_BASE << 8;
//    *GPIO_PA_DOUT = 0xff;
    switch(*GPIO_PC_DIN)
      case 0b01111111:
        amplitude += 100;
        break;
      case 0b11111110:
        current_tune = 0;
        break;
      case 0b11111101:
        current_tune = 1;
        break;
      case 0b11111011:
        current_tune = 2;
        break;
      case 0b11110111:
        current_tune = 3;
        break;
}
