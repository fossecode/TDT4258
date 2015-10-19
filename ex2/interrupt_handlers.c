#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include "efm32gg.h"

#include "frequencies.h"
#include "tune.h"
#include "songs/imperial_march.h"
#include "songs/mario.h"
#include "songs/beep.h"
#include "songs/pew_pew.h"
#define sampleFrequency 5700.0

void setupDAC();
void resetDAC();
void startTimer();
void stopTimer();

int toneNumber = 0;
int toneDuration  = 0;
double sample = 0;
int current_tune = -1;
double amplitude = 1000.0;

void playSound(double pitch){
  sample += (amplitude/(sampleFrequency/pitch)); 
  *DAC0_CH0DATA = (int)floor(sample);
  *DAC0_CH1DATA = (int)floor(sample);
  if (sample > amplitude) sample = 0;
}

void songFinished(){
	//Song is finished;
	toneDuration = 0;
	toneNumber = 0;
	current_tune = -1;
	stopTimer();
        resetDAC();
}

void nextTone(){
	//Go to next tone.
        toneNumber+=1;
        toneDuration = 0;
}

void initSong(){
	// End song if a song is currently running.
	toneDuration = 0;
	toneNumber = 0;
	current_tune = -1;
	// Setup the dac
	setupDAC();
	// Start the timer
	startTimer();
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
          nextTone();
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
          nextTone();
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
          nextTone();
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
          nextTone();
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
	initSong();
        current_tune = 0;
        break;
      case 0b11111011:
	initSong();
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
	initSong();
        current_tune = 1;
        break;
      case 0b11110111:
	initSong();
        current_tune = 3;
        break;
    }
}
