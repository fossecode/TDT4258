#include <stdio.h>
#include "frequencies.h"
#include "tune.h"
#include "imperial_march.h"

int SAMPLE_RATE = 44000;

void play_tune(struct tune tune)
{
    int frequency = tune.frequency;
    int duration  = tune.duration;
    printf("freq, duration: %d, %d\n", frequency, duration);
}

int main (void)
{
    for (int i = 0; i < sizeof(imperial_march) / sizeof(struct tune); i++) 
    {
        play_tune(imperial_march[i]);
    }
    return 0;
}
