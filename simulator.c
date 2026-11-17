#include"simulator.h"
#include "particle.h"
#include <float.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "color.h"
#include <math.h>
#include "vector.h"
#include "Event.h"

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



Simulator *simulatorCreate(double width, double height, int maxNumParticles)
{
    Simulator *s = malloc(sizeof(Simulator));
    if (s == NULL) {
        return NULL;
    }

    s->particles = malloc(maxNumParticles * sizeof(Particle *));
    if (s->particles == NULL) {
        free(s);
        return NULL;
    }

    for (int i = 0; i < maxNumParticles; i++) {
        s->particles[i] = NULL;
    }

    s->width = width;
    s->height = height;
    s->maxParticles = maxNumParticles;
    s->nbParticles = 0;
    s->time = 0.0;
    s->numCollisions = 0;
    s->started = false;
    s->currentEvent = eventCreate(NONE, NULL, NULL, INFINITY_TIME);

    return s;
}


void simulatorFree(Simulator *s)
{
   if (s == NULL) return;

    for (int i = 0; i < s->nbParticles; i++) {
        particleFree(s->particles[i]); // Vérifie le nom exact dans particle.h (particleFree ou free)
    }
    free(s->particles);
    if (s->currentEvent != NULL) {
        eventFree(s->currentEvent);
    }
   free(s);
}


double simulatorGetHeight(Simulator *s)
{
   double Height = s->height;

   return Height ;
}


double simulatorGetWidth(Simulator *s)
{
   double Width = s->width ; 

   return Width;
}

int simulatorGetNumCollisions(Simulator *s)
{
   double numcollisions = s->numCollisions ;
    return numcollisions ;
}


double simulatorGetElapsedTime(Simulator *s)
{
   double time = s->time; 
   
   return time ; 
}


int simulatorGetNumberOfParticles(Simulator *s)
{
   int Numberofparticles = s->nbParticles;
   return Numberofparticles;
}


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



Particle *simulatorGetParticle(Simulator *s, int index)
{
     if (index < 0 || index >= s->nbParticles)
    {
        return NULL;
    }

    return s->particles[index];
    
}

Event *simulatorGetNextEvent(Simulator *s)
{
   eventSet(s->currentEvent, NONE, NULL, NULL, INFINITY_TIME);

   for (int i = 0; i < s->nbParticles; i++) 
   {
      Particle *p = s->particles[i];
      double width = s->width;                  
      double height = s->height;
      
      double tx = particleGetXWallCollisionTime(p, width);
      double ty = particleGetYWallCollisionTime(p, height);

     
      if (tx >= 0 && s->time + tx < eventGetTime(s->currentEvent))
      {
         eventSet(s->currentEvent, XWALL, p, NULL, s->time + tx);
      }
      if (ty >= 0 && s->time + ty < eventGetTime(s->currentEvent))
      {
         eventSet(s->currentEvent, YWALL, p, NULL, s->time + ty);
      }

      for (int j = i + 1; j < s->nbParticles; j++)
      {
          double t = particleGetCollisionTime(p, s->particles[j]);

          if (t >= 0 && s->time + t < eventGetTime(s->currentEvent))
          {
              eventSet(s->currentEvent, COLLISION, p, s->particles[j], s->time + t);
          }
      }
   }

   
   s->started = true; 
   return s->currentEvent;
}


static void collisionHandle (Event *collision, Simulator *s) 
{
   EventType type = eventGetType(collision);
   if (type == XWALL) {
      Particle *p1 = eventGetParticle1(collision);
      particleHandleXWallCollision(p1);
   }
   else if (type == YWALL) {
      Particle *p1 = eventGetParticle1(collision);
      particleHandleYWallCollision(p1);
   }
   else if (type == COLLISION) {
      Particle *p1 = eventGetParticle1(collision);
      Particle *p2 = eventGetParticle2(collision); // CORRIGÉ : p2 au lieu de p1 !
      particleHandleCollision(p1, p2);
      s->numCollisions++; // Incrémente le compteur
   }
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
      nextevent = simulatorGetNextEvent(s);
   }
   allparticlemove(s, t-s->time);
   s->time = t ;
 
}


double simulatorStatParticles(Simulator *s, double (*f)(Particle *))
{
    if (s == NULL || s->nbParticles <= 0) {
        return 0.0; 
    }

    double sum = 0.0;
    int n = s->nbParticles;

    for (int i = 0; i < n; i++) {
        sum += f(s->particles[i]);     }

    return sum / n;
}


