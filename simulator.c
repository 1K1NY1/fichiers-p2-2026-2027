#include"simulator.h"
#include "particle.h"
#include <float.h>
#include <stdbool.h>
#include <stdlib.h>
#include "color.h"
#include <math.h>
#include "vector.h"
#include "Event.h"

/**
 * @brief Opaque type representing an event-driven simulation of particles
 * moving in a box [0, width] x [0, height] and colliding elastically with
 * each other and with the walls of the box.
 */
 struct Simulator_t {
   double width;                  // Largeur de la boîte
   double height;                 // Hauteur de la boîte
   int maxParticles;              // Nombre maximal de particules
   int nbParticles;               // Nombre actuel de particules

   Particle **particles;          // Tableau de pointeurs vers les particules
   double time;                   // Temps écoulé depuis le début
   int numCollisions;             // Nombre total de collisions entre particules

   bool started;                  // La simulation a-t-elle commencé ?
   Event *currentEvent;

}; 


/**
 * @brief Creates a new simulator, with no particle, at time 0.
 * @param width            The width of the box.
 * @param height           The height of the box.
 * @param maxNumParticles  The maximum number of particles of the simulation.
 * @return A pointer to the new simulator, or NULL in case of allocation error.
 */
Simulator *simulatorCreate(double width, double height, int maxNumParticles)
{
   Simulator *s = malloc(sizeof(Simulator));

   s->width = width ; 
   s->height = height ; 
   s->maxParticles = maxNumParticles;


   return s;
}

/**
 * @brief Frees a simulator, as well as all the particles (and events) it
 * contains.
 *
 * @param s  The simulator to free.
 */
void simulatorFree(Simulator *s){
   free(s);
}

/**
 * @brief Returns the height of the box of a simulator.
 *
 * @param s  A simulator.
 * @return The height of the box.
 */
double simulatorGetHeight(Simulator *s)
{
   double Height = s->height;

   return Height ;
}

/**
 * @brief Returns the width of the box of a simulator.
 *
 * @param s  A simulator.
 * @return The width of the box.
 */
double simulatorGetWidth(Simulator *s)
{
   double Width = s->width ; 

   return Width;
}
/**
 * @brief Returns the total number of collisions between particles since the
 * beginning of the simulation (collisions with the walls are not counted).
 *
 * @param s  A simulator.
 * @return The number of collisions between particles.
 */
int simulatorGetNumCollisions(Simulator *s)
{
   double numcollisions = s->numCollisions ;
    return numcollisions ;
}

/**
 * @brief Returns the current time of the simulation (initially 0).
 *
 * @param s  A simulator.
 * @return The current time of the simulation.
 */
double simulatorGetElapsedTime(Simulator *s)
{
   double time = s->time; 
   
   return time ; 
}

/**
 * @brief Returns the number of particles in the simulation.
 *
 * @param s  A simulator.
 * @return The number of particles.
 */
int simulatorGetNumberOfParticles(Simulator *s)
{
   int Numberofparticles = s->nbParticles;
   return Numberofparticles;
}

/**
 * @brief Adds a particle to the simulation. The particle is not added if:
 * - the maximum number of particles is reached;
 * - the particle touches or crosses a wall of the box;
 * - the particle overlaps or touches a particle already in the simulation;
 * - the simulation has already started, i.e. simulatorStep or
 *   simulatorGetNextEvent has already been called.
 * If the particle is added, the simulator becomes responsible for freeing
 * it. Otherwise, the caller remains responsible for freeing it.
 *
 * @param s  A simulator.
 * @param p  The particle to add.
 * @return true if the particle has been added, false otherwise.
 */
bool simulatorAddParticle(Simulator *s, Particle *p)
{
   if (s->nbParticles >= s->maxParticles)
    {
        return false;
    }

    if (s->started)
    {
        return false;
    }

    if (particleOutside(p, s->width, s->height))
    {
        return false;
    }

    for (int i = 0; i < s->nbParticles; i++)
    {
        if (particleOverlap(p, s->particles[i]))
        {
            return false;
        }
    }

    s->particles[s->nbParticles] = p;
    s->nbParticles++;

    return true;
   }


/**
 * @brief Returns a particle of the simulation. Particles are indexed from 0
 * to N-1 (where N is the number of particles), in the order in which they
 * have been added.
 *
 * @param s      A simulator.
 * @param index  The index of the particle.
 * @return The particle of index index, or NULL if index is not valid.
 */
Particle *simulatorGetParticle(Simulator *s, int index)
{
     if (index < 0 || index >= s->nbParticles)
    {
        return NULL;
    }

    return s->particles[index];
    
}

/**
 * @brief Returns the next event of the simulation, i.e. the earliest
 * collision (between two particles or between a particle and a wall) that
 * will occur after the current time if all particles keep moving in a
 * straight line. The time of the event is absolute (not relative to the
 * current time). If no event can occur, an event of type NONE and of time
 * INFINITY_TIME is returned. The returned event belongs to the simulator and
 * must not be freed (nor modified) by the caller. It remains valid until the
 * next call to simulatorStep or simulatorFree.
 *
 * @param s  A simulator.
 * @return The next event of the simulation.
 */
Event *simulatorGetNextEvent(Simulator *s)
{
   eventSet(s->currentEvent, NONE,NULL ,NULL ,INFINITY_TIME); // current event crée

   for (int i = 0 ; i <s->nbParticles; i++ ) // parcours des particules et on cherche les collisions entre particules.
   {
      Particle *p = s->particles[i];
      double width = s->width;                  
      double height = s->height;
      double tx = particleGetXWallCollisionTime (p,width);
      double ty = particleGetYWallCollisionTime (p, height);

       if (s->time + tx < eventGetTime(s->currentEvent))
       {
         eventSet(s->currentEvent,XWALL, p, NULL, s->time + tx);
       }
       if (s->time + ty < eventGetTime(s->currentEvent))
       {
         eventSet(s->currentEvent,YWALL, p, NULL, s->time + ty);
       }
      

        for (int j = i + 1; j < s->nbParticles; j++)
        {
            double t = particleGetCollisionTime(p, s->particles[j]);

            if (s->time + t < eventGetTime(s->currentEvent))
            {
                eventSet(s->currentEvent, COLLISION , p, s->particles[j], s->time + t);
            }
   }
}
   s->started = true ; 
   return s->currentEvent;
}

/**
 * @brief Advances the simulation from the current time t to time t + dt.
 * All the events occurring before t + dt are processed in chronological
 * order: particles are moved until the time of the event, the velocities of
 * the particles involved in the event are updated, and the next event is
 * computed. Particles are finally moved until time t + dt, which becomes the
 * new current time of the simulation.
 *
 * @param s   A simulator.
 * @param dt  The duration of the step (>= 0).
 */

static void collisionHandle (Event *collision , Simulator *s) 
{
   if (eventGetType(collision)== XWALL){
      Particle *p1 = eventGetParticle1(collision);
      particleHandleXWallCollision(p1);
   }
   if (eventGetType(collision)== YWALL) {
      Particle *p1 = eventGetParticle1(collision);
      particleHandleYWallCollision(p1);}
   if (eventGetType(collision)== COLLISION) {
      Particle *p1 = eventGetParticle1(collision);
      Particle *p2 = eventGetParticle1(collision);
      particleHandleWallCollision(p1,p2);}
 }
static void allparticlemove(Simulator *s, double dt)
{ 
   for (int i = 0 ; i < s->nbParticles ; i++ )
   {
      particleMove(s->particles[i], dt);
   }
}
void simulatorStep(Simulator *s, double dt)
{
   
   Event * nextevent = simulatorGetNextEvent(s);
   double t = s->time + dt;

   
   while (eventGetTime(nextevent)< t) 
   {
      double t_vat = eventGetTime(nextevent) ; 
      allparticlemove(s , t_vat - s->time );
      s->time = eventGetTime(nextevent);
      collisionHandle(nextevent , s ); 
      eventFree(nextevent); 
      nextevent = simulatorGetNextEvent(s);
   }
   allparticlemove(s, t-s->time);
   s->time = t ;
 
}

/**
 * @brief Computes the average, over all particles of the simulation, of a
 * function of a particle. For example,
 * simulatorStatParticles(s, particleKineticEnergy) returns the average
 * kinetic energy of the particles.
 *
 * @param s  A simulator (with at least one particle).
 * @param f  A function computing a value from a particle.
 * @return The average of f over all particles.
 */
double simulatorStatParticles(Simulator *s, double (*f)(Particle *))
{
   
}


