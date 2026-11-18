#include "particle.h"
#include <float.h>
#include <stdbool.h>
#include <stdlib.h>
#include "color.h"
#include <math.h>
#include "vector.h"


struct Particle_t  {
    double mass, radius;    //   The mass of the particle and radius (> 0).
    Vector position, velocity ; // The velocity and position  of the particle.
    Color color;  //   The color of the particle (used only for display).
    double numcollision; 
};


Particle *particleCreate(double radius, double mass, Vector position, Vector velocity, Color color){
    Particle *p = malloc(sizeof(Particle));
    if (p == NULL) {
        return NULL; 
    
    }
    p-> radius = radius;
    p-> mass = mass;
    p-> position = position;
    p-> velocity = velocity;
    p-> color = color;
    return p;

}

void particleFree(Particle *p){
    free (p);
}


double particleGetRadius(Particle *p)
{
    double radius = p->radius ; 
    return radius ; 
}


double particleGetMass(Particle *p)
{
    double mass = p->mass ; 
    return mass ;
}


Vector particleGetPosition(Particle *p)
{
    Vector position = p->position ;
    return position ;
}

Vector particleGetVelocity(Particle *p){
    Vector velocity = p->velocity ; 
    return velocity ;
}


Color particleGetColor(Particle *p)
{
    Color color = p->color ; 
    return color ; 
}


double particleGetNumCollisions(Particle *p)
{
    double numcollisions = p->numcollision ;
    return numcollisions ; 
}



double particleKineticEnergy(Particle *p)
{
   double KineticEnergy = 0 ; 
   double mass  = particleGetMass(p); 
   Vector velocity = particleGetVelocity(p);
   double v_squared = vectorDotProduct(velocity, velocity);;

    KineticEnergy = 0.5 * mass * v_squared ; 

    return  KineticEnergy ; 
}

void particleMove(Particle *p, double dt)
{
    Vector position = particleGetPosition(p);
    Vector velocity = particleGetVelocity(p);

    Vector meters = vectorMultByScalar(velocity, dt);

    position = vectorSum(position, meters);

    p->position = position;          // AJOUT
}

void particleHandleCollision(Particle *p1, Particle *p2)
{
    if (p1 == p2) {
        return;
    }
    Vector pos1 = particleGetPosition(p1);
    Vector pos2 = particleGetPosition(p2);
    Vector vel1 = particleGetVelocity(p1);
    Vector vel2 = particleGetVelocity(p2);
    double m1 = particleGetMass(p1);
    double m2 = particleGetMass(p2);
    Vector dp = vectorDiff(pos2, pos1);
    Vector dv = vectorDiff(vel2, vel1);

    double dot_dv_dp = vectorDotProduct(dv, dp);
    double norm_dp_sq = vectorDotProduct(dp, dp);
    double mass_sum = m1 + m2;

    if (norm_dp_sq == 0.0) {
        return;
    }

    double common_factor = (dot_dv_dp / norm_dp_sq);

    double factor1 = (2.0 * m2 / mass_sum) * common_factor;
    double factor2 = (2.0 * m1 / mass_sum) * common_factor;

    Vector facV1 = vectorMultByScalar(dp, factor1);
    Vector facV2 = vectorMultByScalar(dp, factor2);

    vel1 = vectorSum(vel1, facV1);
    vel2 = vectorDiff(vel2, facV2);

    p1->velocity = vel1;             
    p2->velocity = vel2;             

    p1->numcollision++;              
    p2->numcollision++;              
}




void particleHandleXWallCollision(Particle *p)
{
    Vector velocity = particleGetVelocity(p);

    velocity.x = -velocity.x;

    p->velocity = velocity;
}

void particleHandleYWallCollision(Particle *p)
{
    Vector velocity = particleGetVelocity(p);

    velocity.y = -velocity.y;

    p->velocity = velocity;
}


double particleGetCollisionTime(Particle *p1, Particle *p2)
{
    if (p1 == p2)
    {
        return INFINITY_TIME;
    }

    Vector pos1 = particleGetPosition(p1);
    Vector pos2 = particleGetPosition(p2);

    Vector vel1 = particleGetVelocity(p1);
    Vector vel2 = particleGetVelocity(p2);

    Vector dp = vectorDiff(pos2, pos1);
    Vector dv = vectorDiff(vel2, vel1);

    double sigma = particleGetRadius(p1) + particleGetRadius(p2);

    double dv_dp = vectorDotProduct(dv, dp);
    double dv_dv = vectorDotProduct(dv, dv);
    double dp_dp = vectorDotProduct(dp, dp);

    if (dv_dp >= 0)
    {
        return INFINITY_TIME;
    }

    double d = dv_dp * dv_dp
             - dv_dv * (dp_dp - sigma * sigma);

    if (d < 0)
    {
        return INFINITY_TIME;
    }

    return (-dv_dp - sqrt(d)) / dv_dv;
}


double particleGetXWallCollisionTime(Particle *p, double width)
{
    Vector position = particleGetPosition(p);
    Vector velocity = particleGetVelocity(p);

    double radius = particleGetRadius(p);

    if (velocity.x > 0)
    {
        return (width - radius - position.x) / velocity.x;
    }

    if (velocity.x < 0)
    {
        return (radius - position.x) / velocity.x;
    }

    return INFINITY_TIME;
}


double particleGetYWallCollisionTime(Particle *p, double height)
{
    Vector position = particleGetPosition(p);
    Vector velocity = particleGetVelocity(p);

    double radius = particleGetRadius(p);

    if (velocity.y > 0)
    {
        return (height - radius - position.y) / velocity.y;
    }

    if (velocity.y < 0)
    {
        return (radius - position.y) / velocity.y;
    }

    return INFINITY_TIME;
}


bool particleOverlap(Particle *p1, Particle *p2)
{
    Vector pos1 = particleGetPosition(p1);
    Vector pos2 = particleGetPosition(p2);

    Vector dp = vectorDiff(pos2, pos1);

    double distance = vectorNorm(dp);

    double rayon = particleGetRadius(p1) + particleGetRadius(p2);

    return distance <= rayon;
}


bool particleOutside(Particle *p, double width, double height)
{
    Vector position = particleGetPosition(p);

    double radius = particleGetRadius(p);

    if (position.x - radius <= 0)
    {
        return true;
    }

    if (position.x + radius >= width)
    {
        return true;
    }

    if (position.y - radius <= 0)
    {
        return true;
    }

    if (position.y + radius >= height)
    {
        return true;
    }

    return false;
}


