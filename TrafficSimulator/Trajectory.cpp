#include "Trajectory.h"

Trajectory::Trajectory(BezierSequence *p_road_curve_)
{
    p_road_curve = p_road_curve_;
    init();
}

void Trajectory::init()
{
    generateTrajectory();

    int size = input_points.size();
    projected_x.resize(size);

    for (int i=0; i<size; ++i)
    {
        projected_x[i] = p_road_curve->getXPositionByPoint(input_points[i]);
//        projected_x[i] = p_road_curve->getXPositionByPointByBisection(input_points[i]);
    }

    // make sure the start point projected to len=0.0
    if (size > 0)
    {
        projected_x[0] = 0.0;
    }

    projected_point.resize(size);
    projected_normal.resize(size);
    for (int i=0; i<size; ++i)
    {
        projected_point[i] = p_road_curve->getPointByXPosition(projected_x[i]);
        projected_normal[i] = p_road_curve->getNormalByXPosition(projected_x[i]);
    }
}

void Trajectory::generateTrajectory()
{
    input_points.clear();
    input_timestamp.clear();
    double time = 10;  // in milliseconds
    double len = 0;
    double len_step = p_road_curve->length / 50;

    while (len < p_road_curve->length-len_step)
    {
        zVec2 pos = p_road_curve->getPointByXPosition(len);

        pos.x += 10.0 * (Randomer::generateRandomFromZeroToOne()-0.5);
        pos.y += 10.0 * (Randomer::generateRandomFromZeroToOne()-0.5);
        input_points.push_back(pos);

        time += 2.5;// * Randomer::generateRandomFromZeroToOne();
        input_timestamp.push_back(time);

        len += len_step;
    }

//    int size = p_road_curve->beziers.size();
//    for (int i=0; i<size; ++i)
//    {
//        Bezier *p_bez = &p_road_curve->beziers[i];

//        for (int j=0; j<4; ++j)
//        {
//            zVec2 pos = p_bez->control_point[0];
//            pos.x += 10.0 * (Randomer::generateRandomFromZeroToOne()-0.5);
//            pos.y += 10.0 * (Randomer::generateRandomFromZeroToOne()-0.5);
//            input_points.push_back(pos);

//            time += 100.0 * Randomer::generateRandomFromZeroToOne();
//            input_timestamp.push_back(time);
//        }
//    }
}
