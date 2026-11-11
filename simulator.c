#include"simulator.h"
#include "particle.h"
#include <float.h>
#include <stdbool.h>
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

   s->width = width ; 
   s->height = height ; 
   s->maxParticles = maxNumParticles;


   return s;
}


void simulatorFree(Simulator *s){
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


double simulatorStatParticles(Simulator *s, double (*f)(Particle *))
{
    if (s == NULL || s->nbParticles <= 0) {
        return 0.0; 
    }

    double sum = 0.0;
    int n = s->nbParticles;

    for (int i = 0; i < n; i++) {
        sum += f(&s->particles[i]);     }

    return sum / n;
}


