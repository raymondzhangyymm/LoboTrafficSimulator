#ifndef TRAJECTORY_H
#define TRAJECTORY_H

#include "global.h"
#include "BezierSequence.h"
#include "randomer.h"

class Trajectory
{
public:
    Trajectory(BezierSequence *p_road_curve_);

    BezierSequence *p_road_curve;

    // the inpute data
    vector<zVec2> input_points;
    vector<double> input_timestamp;

    // the input points projected to the road curve
    vector<double> projected_x;
    vector<zVec2> projected_point;
    vector<zVec2> projected_normal;

    void init();
    void updateVisualTrajectory();

private:
    void generateTrajectory();
};

#endif // TRAJECTORY_H
