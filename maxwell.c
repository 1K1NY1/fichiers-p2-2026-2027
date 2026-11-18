#include"simulator.h"
#include "particle.h"
#include <float.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "color.h"
#include <math.h>
#include "vector.h"
#include <time.h>



static double sample(double min, double max)
{
    double r = ((double)rand()) / RAND_MAX;

    return r * (max - min) + min;
}

int main(void)
{
    srand(time(NULL));

    Simulator *sim = simulatorCreate(100, 100, 500);
    for(int i = 0 ; i < 500 ; i++) 
    {
        bool added = false ;
        while (!added)
        {
         double theta = sample(0.0, 2.0 * M_PI ) ;
            Particle *p = particleCreate(
                1.0, 1.0,
                (Vector){sample(0.0, 100.0), sample(0.0, 100.0)},
                (Vector){cos(theta), sin(theta)},
                COLOR_BLACK);
            added = simulatorAddParticle(sim, p);
            if (!added)
                particleFree(p);
        }
        
    }

    printf("Energie initiale : %f\n", 
            simulatorStatParticles(sim, particleKineticEnergy));
    
        while (simulatorStatParticles(sim, particleGetNumCollisions) < 50)
    {
        simulatorStep(sim, 0.5);
    }
    printf("Collisions moyennes par particule : %f\n",
           simulatorStatParticles(sim, particleGetNumCollisions));

    FILE *f = fopen("speeds.txt", "w");
    for (int i = 0; i < 500 ; i++)
    {
        Particle *p = simulatorGetParticle(sim, i);
        double v = sqrt(2.0 * particleKineticEnergy(p) / 1.0); // masse = 1
        fprintf(f, "%f\n", v);
    }
    fclose(f);

    printf("Energie moyenne : %f\n", simulatorStatParticles(sim, particleKineticEnergy));



    simulatorFree(sim);
    return 0;
}