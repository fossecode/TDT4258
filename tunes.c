#include <stdio.h>

int SAMPLE_RATE = 44000;

struct tune
{
    int frequency;
    int duration;
} tune;


int determine_how_many_times_a_tune_should_be_played(int duration)
{
    return SAMPLE_RATE / 1000 * duration;
}

void play_tune(struct tune tune)
{
    int frequency = tune.frequency;
    int duration  = determine_how_many_times_a_tune_should_be_played(tune.duration);

    printf("freq, duration: %d, %d\n", frequency, duration);
}

int main (void)
{

    struct tune imperial_march[3] = {
        {55, 10},
        {25, 10},
        {15, 10}
    };

    for (int i = 0; i < sizeof(imperial_march) / sizeof(struct tune); i++) 
    {
        play_tune(imperial_march[i]);
    }

    return 0;
}
