#ifndef Bezier_H
#define Bezier_H

#include "global.h"

// The class is a basic cubic bezier curve detemined by four control points p0~p3,
// where p0 and p3 are in the curve but p1 and p2 are not.
// It provides basic geometry information and computation for the road class.
class Bezier
{
public:
    // Instantiate by four control points or a point array.
    Bezier(zVec2 p0, zVec2 p1, zVec2 p2, zVec2 p3);
    Bezier(vector<zVec2> pa_);
    ~Bezier();

    // The bezier must be initialized when the control points have changed.
    zVec2 control_point[4];
    void init();

    // Restore the bezier from backup.
    zVec2 control_point_backup[4];
    void backupControlPoints();
    void restoreControlPoints();

    // Segmentize the bezier smoothly for visualization.
    vector<double> sequ_t;
    vector<zVec2> sequ_point;
    vector<zVec2> sequ_normal;
    vector<zVec2> sequ_tangent;

    // The length of the bezier.
    double length;

    // Compute the point determined by the parameter t
    zVec2 getPoint(double t);

    // Compute tangent at the point determined by the parameter t
    zVec2 getTangent(double t);

    // Compute normal at the point determined by the parameter t
    zVec2 getNormal(double t);

    // Compute the arc length at parameter t
    double getArcLength(double t);

    // Compute the parameter t given the arc length from 0 to t
    double getParameterByArcLength(double arc_);

    // Compute the parameter t of the point closest to the given point p,
    // using Newton's method with initial guess of t_init,
    double getParameterClosestToPosition(zVec2 pos, double t_init);

    // or, using robust Bisection method.
    bool getParameterClosestToPositionByBisection(zVec2 pos, double &result_t);

private:
    // The coefficients of the bezier curve derived from the control points
    double xa,xb,xc,xd;
    double ya,yb,yc,yd;

    // Compute the first/second derivative at the point t
    zVec2 getFirstDerivative(double t);
    zVec2 getSecondDerivative(double t);

    // Determine the total arc length of the bezier and the optimal number of subdivision.
    // Using subdivision method, see Gravesen's paper.
    int optimal_subdivision;
    vector<double> arclength_array;
    void computeArcLengthBySubdivision();
    void subdivideInHalf(const zVec2 cp[], zVec2 cp_1[], zVec2 cp_2[]);

    // Get the lower- and upper-bound length of a bezier
    double getLengthOfChord(vector<zVec2> &cps);
    double getLengthOfControlPolygon(vector<zVec2> &cps);

    // For Bisection method
//    bool searchInterval(zVec2 pos, double &ffa, double &ffb, int depth);
    double computeMinDistanceFunctionValue(zVec2 pos, double tt);
};

#endif // Bezier_H
