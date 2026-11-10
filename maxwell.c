#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

#include "color.h"
#include "vector.h"
#include "particle.h"
#include "event.h"
#include "simulator.h"
#include "simulation_gif.h"

void main(void)
{
    // default parameter values
    double dt = 0.5;
    int nb_steps = 500;
    int num_particles = 20;
    char *filename = NULL;
    char *giffilename = "simulation.gif";
    int gifmaxsize = 1000;

    // Command line argument handling
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-f") == 0 && i + 1 < argc)
        {
            filename = argv[++i];
        }
        else if (strcmp(argv[i], "-n") == 0 && i + 1 < argc)
        {
            num_particles = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "-s") == 0 && i + 1 < argc)
        {
            nb_steps = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "-t") == 0 && i + 1 < argc)
        {
            dt = atof(argv[++i]);
        }
        else if (strcmp(argv[i], "-g") == 0 && i + 1 < argc)
        {
            gifmaxsize = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc)
        {
            giffilename = argv[++i];
        }
        else if (strcmp(argv[i], "-h") == 0)
        {
            print_usage(argv[0]);
            return 0;
        }
    }

    Simulator *sim = NULL;
    if (filename != NULL)
    {
        sim = LoadSimFromFile(filename);
    }
    else
    {
        sim = createRandomSim(num_particles);
    }

    if (sim == NULL)
    {
        fprintf(stderr, "Error: Could not initialize simulation.\n");
        return 1;
    }

    // Simulator *sim = createRandomSim(20);

    fprintf(stderr, "Number of particles that will be simulated: %d\n", simulatorGetNumberOfParticles(sim));

    SimGIF *gif = createGif(sim, giffilename, gifmaxsize);

    addFrame(sim, gif, 5);

    printf("Initial average particule energy: %.2f\n",
           simulatorStatParticles(sim, particleKineticEnergy));

    for (int t = 0; t < nb_steps; t++)
    {
        fprintf(stderr, "t = %d\r", t);

        simulatorStep(sim, dt);

        addFrame(sim, gif, 5);
    }

    closeGif(gif);

    printf("Number of collisions between particles during simulation: %d\n",
           simulatorGetNumCollisions(sim));
    printf("Average number of collisions per particle: %.2f\n",
           simulatorStatParticles(sim, particleGetNumCollisions));
    printf("Average particule energy: %.2f\n",
           simulatorStatParticles(sim, particleKineticEnergy));

    simulatorFree(sim);


}
