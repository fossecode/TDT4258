#include <stdio.h>

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

    struct tune imperial_march[70] = {
        {a,  500},
        {a,  500},    
        {a,  500},
        {f,  350},
        {cH, 150},
        {a,  500},
        {f,  350},
        {cH, 150},
        {a, 1000},
        {eH, 500},
        {eH, 500},
        {eH, 500},    
        {fH, 350},
        {cH, 150},
        {gS, 500},
        {f,  350},
        {cH, 150},
        {a, 1000},
        {aH, 500},
        {a,  350},
        {a,  150},
        {aH, 500},
        {gSH, 250},
        {gH , 250},
        {fSH, 125},
        {fH, 125},    
        {fSH, 250},
        {0, 250},
        {aS, 250},    
        {dSH, 500},  
        {dH, 250},  
        {cSH, 250},  
        {cH, 125},  
        {b, 125},  
        {cH, 250},      
        {0, 250},
        {f, 125},  
        {gS, 500},  
        {f, 375},  
        {a, 125},
        {cH, 500},
        {a, 375},  
        {cH, 125},
        {eH, 1000},
        {aH, 500},
        {a, 350},
        {a, 150},
        {aH, 500},
        {gSH, 250},
        {gH, 250},
        {fSH, 125},
        {fH, 125},    
        {fSH, 250},
        {0, 250},
        {aS, 250},    
        {dSH, 500},  
        {dH, 250},  
        {cSH, 250},  
        {cH, 125},  
        {b, 125},  
        {cH, 250},      
        {0, 250},
        {f, 250},  
        {gS, 500},  
        {f, 375},  
        {cH, 125},
        {a, 500},            
        {f, 375},            
        {c, 125},            
        {a, 1000}
    };

    for (int i = 0; i < sizeof(imperial_march) / sizeof(struct tune); i++) 
    {
        play_tune(imperial_march[i]);
    }

    return 0;
}
