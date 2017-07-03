#ifndef BEZIERSEQUENCE_H
#define BEZIERSEQUENCE_H

#include "global.h"
#include "Bezier.h"

// The class contains a bezier sequence.
// Two consecutive beziers are connected smoothly.
class BezierSequence
{
public:
    BezierSequence(){}
    BezierSequence(vector<Bezier> &bezier_array_);

    // the curve consists of a couple of bezier curves connected smoothly
    vector<Bezier> beziers;
    bool hasModifiedBezier;

    // The length of the curve
    double length;

    int num_points;

    // build bezier sequence.
    void buildBezierSequence();

    // get the position/normal/tangent at the point determined by the x position(range 0.0~1.0)
    zVec2 getPointByXPosition(double xx);
    zVec2 getTangentByXPosition(double xx);
    zVec2 getNormalByXPosition(double xx);
    void getPointNormalTangentByXPosition(zVec2 &pos, zVec2 &norm, zVec2 &tangent, double xx);

    // move a control point to a new position.
    // the related control points will be also moved automatically
    void manipulateControlPoint(int idx_cp, zVec2 pos);

    // split a bezier to two sub beziers at the closest point to pos.
    // return false if failed.
    bool splitBezier(zVec2 pos);

    // find the closest point to pos
    zVec2 getClosestPoint(zVec2 pos);

    // find the closest point to pos using Newton's method.
    zVec2 getClosestPrecisePoint(zVec2 pos);

    // find the x positon by the point closest to given pos
    double getXPositionByPoint(zVec2 pos);
    double getXPositionByPointByBisection(zVec2 pos);

private:
    // use a sequence of dense points to approximate the curve
    int num_beziers;
    PointArray sequ_point;
    PointArray sequ_tangent;
    PointArray sequ_normal;
    vector<double> sequ_x;

    // split bez1 to bez1 and bez2 at the point t. Use De Casteljau algorithm.
    void splitByDeCasteljau(Bezier &bez1, Bezier &bez2, double t);
};

#endif // BEZIERCURVE_H
