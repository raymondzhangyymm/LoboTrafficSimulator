#ifndef CARPARTICLE_H
#define CARPARTICLE_H

#include "global.h"
#include "Cell.h"
#include "TriangleMesh.h"
#include "Trajectory.h"

class CarParticle
{
public:
    CarParticle();
    CarParticle(BezierSequence *road_curve_p_, double road_offset_);
    CarParticle(Cell *cell_p_);

    long long id;
    int road_id;
    int lane_id;
    int cell_id;

    BezierSequence *road_curve_p;
    double road_offset;
    void initByXPosition(double x_pos_);

    Cell *cell_p;
    void initByPosition(zVec2 pos_);

    // The current and the previous position of the car.
    double x_position, x_velocity;
    double x_position_pre;

    zVec2 position, velocity;
    double density;

    void getBbox(zVec2 bbox[]);

    TriangleMesh *carmesh;
    zVec4 carcolor;
    int carmesh_id, carcolor_id;
    double length, width, height;
    bool isVisible;
    bool isAnchored;

    zVec2 orientation, normal;

    int tail_length;
    zVec2 tail_left[256], tail_right[256];
    void pushTail(zVec2 lp, zVec2 rp);
    void resetTail();

    int head_length;
    zVec2 head_left[256], head_right[256];
    vector<zVec2> head_visual_trajpoints;

    void step(double delta_t);

    // compute the spring force/acceleration
    zVec2 spring_acceleration;
    zVec2 spring_velocity;
    zVec2 spring_position;
    zVec2 spring_displacement;
    double spring_previous_displacement;
    double spring_angular_frequency, spring_damping_ratio;
    bool isColliding;
    void computeSpringAcceleration(double delta_t);
    void clearSpring();

    // for trajectory following
    vector<TaxiNode*> taxinode_list;
    int current_taxinode;
    Trajectory *trajectory;
    void computeXVelocityByTrajectory();
    int searchTrajectory(double x_pos);

    // try a step
    zVec2 position_try, velocity_try;
    int road_id_try, cell_id_try;
    bool isBad;
    bool isOutOfCell;
    void stepTry(double delta_t);
    void stepTry(double delta_t, zVec2 vel);
    void stepConfirm();

    void stepX(double delta_t);
    void stepX(double delta_t, CarParticle *p_car_pre);

    bool isSafeToPreviousCar(CarParticle *p_car_pre);
    bool isSafeToNextCar(CarParticle *p_car_next);

    CarParticle* duplicate();

    // For handling lane changing
    int lanechanging_status; //0: no changing, 1: changing out, 2: changing in, 3: end changing out
    int lanechanging_direction; // -1: move to the left, 1: move to the right
    double lanechanging_percentage; // the car moves smoothly by controlling the percentage
    double lanechanging_x_position; // for real position of the car in lanechanging.
    bool needLaneChanging;  //true if the density difference is high.
    CarParticle *ghost_neighbor; // the ghost is a coupled car which is the target in the target lane

    void updatePositionByXPos();
    void updateTail();
    void updateHead();

//    // The bounding box of the car
//    vector<zVec2> bbox;
//    vector<zVec2> original_bbox;

//    void initLocation(Road *p_road);

//    void tryStep(Road *p_road, double delta_time);

//    zVec2 elastic_force;
//    zVec2 elastic_acceleration;
//    void tryStepByElasticForce(Road *p_road, double delta_time);

//    void confirmStep(Road *p_road);
//    void relocateBoundingBox();
};

#endif // CARPARTICLE_H
