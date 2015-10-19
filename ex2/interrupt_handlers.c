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
#include "songs/pew_pew.h"
#define sampleFrequency 5700.0


int toneNumber = 0;
int toneDuration  = 0;
double noise = 0;
bool increment = true;
int current_tune = -1;
bool off = true;
double amplitude = 1000.0;
int count = 0;

void playSound(double pitch){
  noise += (amplitude/(sampleFrequency/pitch)); 
  *DAC0_CH0DATA = (int)floor(noise);
  *DAC0_CH1DATA = (int)floor(noise);
  if (noise > amplitude) noise = 0;
}

void songFinished(){
	//Song is finished;
	toneDuration = 0;
	toneNumber = 0;
	current_tune = -1;
	stopTimer();
}

/*Low energy timer */
void __attribute__ ((interrupt)) LETIMER0_IRQHandler()
{
	// Clear interrupt flag 
	*LETIMER0_IFC = 1;

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
        songFinished();
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
      else{
        songFinished();
      }
      break;
    case 2:
      if (beep[toneNumber].frequency != -1 ){
        if (beep[toneNumber].frequency){
          playSound((double)beep[toneNumber].frequency);
        }
        toneDuration += 6;
        if (toneDuration >= beep[toneNumber].duration){
          //Go to next tone.
          toneNumber+=1;
          toneDuration = 0;
        }
      }
      else{
        songFinished();
      }
       break;
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
         songFinished();
      } 
      break;
    }
}

/* GPIO even pin interrupt handler */
void __attribute__ ((interrupt)) GPIO_EVEN_IRQHandler() 
{
    // Clear interrupt handler
    *GPIO_IFC = *GPIO_IF;
    *GPIO_PA_DOUT = *GPIO_PC_DIN << 8;
    switch(*GPIO_PC_DIN){
      case 0b10111111:
        amplitude -= 100;
        break;
      case 0b11111110:
	startTimer();
        current_tune = 0;
        break;
      case 0b11111011:
	startTimer();
        current_tune = 2;
        break;
    }

}

/* GPIO odd pin interrupt handler */
void __attribute__ ((interrupt)) GPIO_ODD_IRQHandler() 
{
    // Clear interrupt flag 
    *GPIO_IFC = *GPIO_IF;
    *GPIO_PA_DOUT = *GPIO_PC_DIN << 8;

    switch(*GPIO_PC_DIN){
      case 0b01111111:
        amplitude += 100;
        break;
      case 0b11111101:
	startTimer();
        current_tune = 1;
        break;
      case 0b11110111:
	startTimer();
        current_tune = 3;
        break;
    }
}
