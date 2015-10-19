#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"

/* function to setup the low energy timer */
void setupTimer(uint16_t period)
{

	*CMU_OSCENCMD = (1 << 6); // Enable the low frequency oscillator (LFRCO)
	*CMU_HFCORECLKEN0 |= (1 << 4); // Enable the low energy timer
	*LETIMER0_CTRL |= (1 << 9); // Set COMP0 as TOP register
	*CMU_LFACLKEN0 |= (1 << 2); // Enable LOW energy timer 0
	*LETIMER0_COMP0 = 1; // The counter is cleared in the clock cycle when it matches the value in this register.
	*LETIMER0_IEN = 1; // Enable interrupts
}

void resetTimer(){
	*LETIMER0_CMD = 2; // Stop timer
	*CMU_HFCORECLKEN0 &= ~(1 << 4); // Disable the low energy timer
	*CMU_LFACLKEN0 &= ~(1 << 2); //Disable low energy timer clock

}

void startTimer() {
    *LETIMER0_CMD = 1; // Start timer
}

void stopTimer() {
    *LETIMER0_CMD = 2; // Stop timer
}

