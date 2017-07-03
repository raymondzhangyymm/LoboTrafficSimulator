#include "Particle.h"

Particle::Particle()
{
    orig_position.clear();
    orig_velocity.clear();
    orig_acceleration.clear();

    x_position = 0;
    x_velocity = 0;
    x_acceleration = 0;

    density	= 0;
    pressure = 0;

    isAnchored          = false;
    isActive            = false;
    isSelected          = false;
    isNeighbor          = false;
    isInRoad            = false;
    isBad               = false;
    isInitialzedVelocityHalfplace = false;

    hasRoadChanged = false;
    destination_id = 0;
}

//=========================================================================
void Particle::initByPositionAndCellEnvolop(zVec2 pos, Cell *cell_)
{
    orig_position = pos;

    position = orig_position;
    velocity = orig_velocity;
    acceleration = orig_acceleration;

    cell_envolop = cell_;

    normal = cell_envolop->center_normal;
    tangent = cell_envolop->center_tangent;
}

Particle* Particle::duplicate()
{
    Particle *p_part = new Particle;

    p_part->id = id;
    p_part->road_id = road_id;
    p_part->lane_id = lane_id;
    p_part->cell_id = cell_id;

    p_part->mass = mass;

    p_part->orig_position = orig_position;
    p_part->orig_velocity = orig_velocity;
    p_part->orig_acceleration = orig_acceleration;

    p_part->position = position;
    p_part->velocity = velocity;
    p_part->acceleration = acceleration;

    p_part->x_position = x_position;
    p_part->x_velocity = x_velocity;
    p_part->x_acceleration = x_acceleration;

    p_part->neighbors.clear();
    p_part->neighborsHalfRegion.clear();

    return p_part;
}

void Particle::step(double deltaTime)
{
    //--------------------------------------------------------------------------
    // Leap frog
    //--------------------------------------------------------------------------
//    // Check if the velocity_halfplace has been initialized.
//    if (!isInitialzedVelocityHalfplace)
//    {
//        velocity_halfplace = velocity - acceleration * (0.5 * deltaTime);
//        isInitialzedVelocityHalfplace = true;
//    }

//    // Update the velocity
//    zVec2 velocity_halfplace_1 = LeapFrogIntegrator(velocity_halfplace, acceleration, deltaTime);

//    velocity = (velocity_halfplace_1 + velocity_halfplace) * 0.5;

//    velocity_halfplace = velocity_halfplace_1;
    
//    // Update the position
//    position = LeapFrogIntegrator(position, velocity_halfplace_1, deltaTime);

    //--------------------------------------------------------------------------
    // Semi-Implicit Euler
    //--------------------------------------------------------------------------
    // Update the position
    velocity += acceleration * deltaTime;

    // correct the direction
//    velocity = tangent * velocity.length();

    position += velocity * deltaTime;
}

void Particle::stepTry(double deltaTime)
{
    velocity_try = velocity + acceleration * deltaTime;
    position_try = position + velocity_try * deltaTime;
}

void Particle::stepTry(double deltaTime, zVec2 vel)
{
    velocity_try = vel;
    position_try = position + velocity_try * deltaTime;
}

void Particle::stepConfirm()
{
    velocity = velocity_try;
    position = position_try;
}

void Particle::stepByConstantSpeed(double deltaTime)
{
    velocity = tangent * 5.0;
    position += velocity * deltaTime;
}

void Particle::stepXPosition(double deltaTime)
{
    //--------------------------------------------------------------------------
    // Semi-Implicit Euler
    //--------------------------------------------------------------------------
    x_velocity += x_acceleration * deltaTime;
    x_position += x_velocity * deltaTime;
}

//============================================================================
// leap frog integrator.
//============================================================================
zVec2 Particle::LeapFrogIntegrator(zVec2 previousVector, zVec2 vector, double deltaTime)
{
    return previousVector + vector * deltaTime;
}

