#ifndef Particle_H
#define Particle_H

#include "global.h"
#include "Kernel.h"
#include "TriangleMesh.h"
#include "Cell.h"

// The particle is defined for traffic simulation, i.e. 2D only.
class Particle
{
public:

    Particle();

    int id; // for the position in the particle list that contains it.
    int road_id;
    int lane_id;
    int cell_id;

    double mass;
    double orig_density;
    double density;
    double pressure;
    double sphereRadius;
    double supportRadius;

    // For handling lane changing
    int lanechanging_status; //0: no changing, 1: changing out, 2: changing in, 3: end changing out.
    Particle *lanechanging_target_part; // the mass keeps transferring to the target
    double lanechanging_percentage; // the mass is transferred smoothly by controlling the percentage

    // The road and cell id list stores (road_id, cell_id) pairs for the overlapping cells.
    bool isOnIntersection;
    vector<int> roadcell_id_list;

    // The position, velocity and acceleration of the particle.
    zVec2 orig_position, orig_velocity, orig_acceleration;
    zVec2 position, velocity, acceleration;
    zVec2 tangent, normal;

    // The forces for 2D case.
    zVec2 pressureForce, viscosityForce, dampingForce, externalForce;
    zVec2 trafficForce;

    // The cell where the particle is in.
    Cell *cell_envolop;

    // The position/velocity/accelaration for 1D case.
    double x_position, x_velocity, x_acceleration;
    double x_pressureForce, x_viscosityForce, x_dampingForce, x_externalForce;
    double x_trafficForce;
//    double x_mapped;

    // The halfplace velocity is for Leap-frog integrator.
    zVec2 velocity_halfplace;
    bool isInitialzedVelocityHalfplace;

    bool isAnchored;
    bool isActive;  // for some special particles.
    bool isSelected;
    bool isNeighbor;
    bool isInRoad;
    bool isBad;  // a bad particle should be removed soon

    // The destination where the particle is going
    int destination_id;
    bool hasRoadChanged;

    // The neighbor list and shape functions are associated with the partical
    vector<Particle*> neighbors;
    vector<Particle*> neighborsHalfRegion;

    void initByPositionAndCellEnvolop(zVec2 pos, Cell *cell_);

    Particle* duplicate();

    void step(double deltaTime);

    // try a step
    zVec2 position_try, velocity_try;
    void stepTry(double deltaTime);  // try with default velocity updated by the acceleration.
    void stepTry(double deltaTime, zVec2 vel); // try with some modified velocity
    void stepConfirm();

    // The speed is fixed.
    void stepByConstantSpeed(double deltaTime);

    void stepXPosition(double deltaTime);

    zVec2 LeapFrogIntegrator(zVec2 previousVector, zVec2 vector, double deltaTime);
};

#endif
