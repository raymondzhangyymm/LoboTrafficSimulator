#include "BezierSequence.h"

BezierSequence::BezierSequence(vector<Bezier> &bezier_array_)
{
    beziers = bezier_array_;
}

// This function samples the sequence of beziers by a bunch of pioints
// to approximate the beziers. The tangent/normal/x_map are also recorded.
void BezierSequence::buildBezierSequence()
{
    // Initialize the individual beziers and compute the total length
    length = 0.0;
    num_beziers = beziers.size();
    for (int i=0; i<num_beziers; ++i)
    {
        Bezier *p_bez = &beziers[i];
        p_bez->init();
        length += p_bez->length;
    }
//    cout << "before == num_bezier num_point length step = " << beziers.size() << " " << num_beziers << " "
//         << num_points << " " << length << endl;

    sequ_point.clear();
    sequ_normal.clear();
    sequ_tangent.clear();
    sequ_x.clear();

    // The curve will be divided to (num_points-1) segments (num_points >= 2)
    // such that each cell roughly contains 10 line segments
    int size_points = 100.0 * length / DEFAULT_LANE_CELL_LENGTH + 2;
    double step_arc_length = length / double(size_points-1);
    double arc_acc = 0.0;
    double arc_in_bez = 0.0;

    // generate the sample points except the last one
    int idx_current_bez = 0;
    Bezier *p_current_bez = &beziers[idx_current_bez];
    while (arc_acc < length)
    {
//        cout << "p_current_bez->length arc_in_bez arc_acc = "
//             << p_current_bez->length << " " << arc_in_bez << " " << arc_acc << endl;

        double t = p_current_bez->getParameterByArcLength(arc_in_bez);
        sequ_point.push_back(p_current_bez->getPoint(t));
        sequ_normal.push_back(p_current_bez->getNormal(t));
        sequ_tangent.push_back(p_current_bez->getTangent(t));
        sequ_x.push_back(arc_acc / length);

        arc_in_bez += step_arc_length;
        arc_acc += step_arc_length;

        // move to the next bezier if the point is beyond current bezier.
        while (arc_in_bez > p_current_bez->length)
        {
            arc_in_bez -= p_current_bez->length;
            idx_current_bez++;
            if (idx_current_bez < num_beziers)
            {
                p_current_bez = &beziers[idx_current_bez];
            }
            else
            {
                arc_acc = length + 1.0;
                break;
            }
        }

    }

//    cout << "p_current_bez->length arc_in_bez arc_acc = "
//         << p_current_bez->length << " " << arc_in_bez << " " << arc_acc-1.0 << endl;

    // the last point is aligned to t=1.0 in the last bezier.
//    p_current_bez = &beziers[num_beziers-1];
    sequ_point.push_back(p_current_bez->getPoint(1.0));
    sequ_normal.push_back(p_current_bez->getNormal(1.0));
    sequ_tangent.push_back(p_current_bez->getTangent(1.0));
    sequ_x.push_back(1.0);

    num_points = sequ_point.size();

    hasModifiedBezier = false;
}

zVec2 BezierSequence::getPointByXPosition(double xx)
{
    int idx = num_points * (xx/length);
    if (idx >= num_points)
    {
        idx = num_points - 1;
    }
    else if (idx < 0)
    {
        idx = 0;
    }

    return sequ_point[idx];
}

zVec2 BezierSequence::getNormalByXPosition(double xx)
{
    int idx = num_points * (xx/length);
    if (idx >= num_points)
    {
        idx = num_points - 1;
    }
    else if (idx < 0)
    {
        idx = 0;
    }

    return sequ_normal[idx];
}

zVec2 BezierSequence::getTangentByXPosition(double xx)
{
    int idx = num_points * (xx/length);
    if (idx >= num_points)
    {
        idx = num_points - 1;
    }
    else if (idx < 0)
    {
        idx = 0;
    }

    return sequ_tangent[idx];
}

void BezierSequence::getPointNormalTangentByXPosition(zVec2 &pos, zVec2 &norm, zVec2 &tangent, double xx)
{
    int idx = num_points * (xx/length);
    if (idx >= num_points)
    {
        idx = num_points - 1;
    }
    else if (idx < 0)
    {
        idx = 0;
    }

    pos = sequ_point[idx];
    norm = sequ_normal[idx];
    tangent = sequ_tangent[idx];
    return;
}

zVec2 BezierSequence::getClosestPoint(zVec2 pos)
{
    double min_dist2 = 1e20; // initial minimun distance square
    int idx = 0;

    // find the smooth point closest to the given pos.
    int size = sequ_point.size();
    for (int i=0; i<size; ++i)
    {
        zVec2 v = sequ_point[i] - pos;
        double d2 = v.x*v.x + v.y*v.y;
        if (d2 < min_dist2)
        {
            min_dist2 = d2;
            idx = i;
        }
    }

    return sequ_point[idx];
}

zVec2 BezierSequence::getClosestPrecisePoint(zVec2 pos)
{
    double min_dist2 = 1e20; // initial minimun distance square
    int idx_bezier = 0;  // the index to the curve
    int idx_smoothpoint = 0;  // the index to the bezier in the curve

    // find the smooth point closest to the given pos.
    Bezier *p_bez;
    int size = beziers.size();
    for (int i=0; i<size; ++i)
    {
        p_bez = &beziers[i];

        int size_point = p_bez->sequ_point.size();
        for (int j=0; j<size_point; ++j)
        {
            zVec2 v = p_bez->sequ_point[j] - pos;
            double d2 = v.x*v.x + v.y*v.y;
            if (d2 < min_dist2)
            {
                min_dist2 = d2;
                idx_bezier = i;
                idx_smoothpoint = j;
            }
        }
    }

    // determine the precise point by Newton's method
    p_bez = &beziers[idx_bezier];
    double t = p_bez->getParameterClosestToPosition(pos,
                                                    p_bez->sequ_t[idx_smoothpoint]);
    return p_bez->getPoint(t);
}

double BezierSequence::getXPositionByPoint(zVec2 pos)
{
    double min_dist2 = 1e20; // initial minimun distance square
    int idx = 0;

    // find the smooth point closest to the given pos.
    int size = sequ_point.size();
    for (int i=0; i<size; ++i)
    {
        zVec2 v = sequ_point[i] - pos;
        double d2 = v.x*v.x + v.y*v.y;
        if (d2 < min_dist2)
        {
            min_dist2 = d2;
            idx = i;
        }
    }

    return sequ_x[idx] * length;
}

double BezierSequence::getXPositionByPointByBisection(zVec2 pos)
{
    double min_dist2 = 1e20; // initial minimun distance square
    int idx_bezier = -1;
    double tt_bezier;
    int size = beziers.size();
    for (int i=0; i<size; ++i)
    {
        Bezier *p_bez = &beziers[i];
        double tt;
        if (p_bez->getParameterClosestToPositionByBisection(pos, tt))
        {
            zVec2 v = p_bez->getPoint(tt) - pos;
            double d2 = v.x*v.x + v.y*v.y;
            if (d2 < min_dist2)
            {
                min_dist2 = d2;
                idx_bezier = i;
                tt_bezier = tt;
            }
        }
    }

    if (idx_bezier == -1)
    {
//        cout << "Warning! All Bisection methods failed! Use the slow method.\n";
        return getXPositionByPoint(pos);
    }
    else
    {
        Bezier *p_bez;
        double xpos = 0;
        for (int i=0; i<idx_bezier; ++i)
        {
            p_bez = &beziers[i];
            xpos += p_bez->length;
        }
        p_bez = &beziers[idx_bezier];
        xpos += p_bez->getArcLength(tt_bezier);
        return xpos;
    }
}

void BezierSequence::manipulateControlPoint(int idx_cp, zVec2 pos)
{
    int size = beziers.size();
    if (idx_cp < 0 || idx_cp >= size*3 + 1)
    {
        cout << "ERROR! Not a valid bezier id!\n";
        return;
    }

    if (idx_cp == 0)
    {
        // This is the very first control point of the bezier sequence
        Bezier *p_bez = &beziers[0];
        p_bez->control_point[1] += pos - p_bez->control_point[0];
        p_bez->control_point[0] = pos;
        p_bez->init();
    }
    else
    {
        idx_cp--;

        // determine the index of the bezier as well as the control point selected
        int i = idx_cp / 3;
        int j = idx_cp % 3;

        // get the target bezier
        Bezier *p_bez = &beziers[i];

        if (j == 0)
        {
            // the control point is the p1 of the bezier
            p_bez->control_point[1] = pos;
            p_bez->init();

            if (i > 0)
            {
                // it is not the first bezier thus has a previous neighbor
                // adjust the p2 by p3 of the neighbor in order to make the curve smooth
                Bezier *p_bez_previous = &beziers[i-1];
                zVec2 p3 = p_bez_previous->control_point[3];

                zVec2 dirc = p3 - pos;
                double len = (p_bez_previous->control_point[2] - p3).length();
                p_bez_previous->control_point[2] = p3 + dirc * (len / dirc.length());
                p_bez_previous->init();
            }
        }
        else if (j == 1)
        {
            // the control point is the p2 of the bezier
            p_bez->control_point[2] = pos;
            p_bez->init();

            if (i < beziers.size()-1)
            {
                // it is not the last bezier thus has a next neighbor
                // adjust the p1 by p0 of the neighbor in order to make the curve smooth
                Bezier *p_bez_next = &beziers[i+1];
                zVec2 p0 = p_bez_next->control_point[0];

                zVec2 dirc = p0 - pos;
                double len = (p_bez_next->control_point[1] - p0).length();
                p_bez_next->control_point[1] = p0 + dirc * (len / dirc.length());
                p_bez_next->init();
            }
        }
        else if (j == 2)
        {
            // the control point is the p3 of the bezier
            // move both p3 and p2.
            p_bez->control_point[2] += pos - p_bez->control_point[3];
            p_bez->control_point[3] = pos;
            p_bez->init();

            if (i < beziers.size()-1)
            {
                // it is not the last bezier thus has a next neighbor
                // move both p1 and p0 of the neighbor
                Bezier *p_bez_next = &beziers[i+1];
                p_bez_next->control_point[1] += pos - p_bez_next->control_point[0];
                p_bez_next->control_point[0] = pos;
                p_bez_next->init();
            }
        }
    }
}

bool BezierSequence::splitBezier(zVec2 pos)
{
    double min_dist2 = 1e20; // initial minimun distance square
    int idx_bezier = -1;  // the index to the closest bezier
    int idx_smoothpoint = -1;  // the index to the smooth/sample point on the bezier

    // find the smooth point closest to the given pos.
    int size = beziers.size();
    for (int i=0; i<size; ++i)
    {
        Bezier *p_bez = &beziers[i];

        int size_point = p_bez->sequ_point.size();
        for (int j=0; j<size_point; ++j)
        {
            zVec2 v = p_bez->sequ_point[j] - pos;
            double d2 = v.x*v.x + v.y*v.y;
            if (d2 < min_dist2)
            {
                min_dist2 = d2;
                idx_bezier = i;
                idx_smoothpoint = j;
            }
        }
    }

    // the point should be close enough to the bezier curve, otherwise discards it.
    if (min_dist2 <= 0.1)
    {
        // find the parameter of the actual closest point on the bezier by Newton's method
        Bezier *p_bez = &beziers[idx_bezier];

        cout << "Before Newton: bezierid, pointid, point, pos, t = " << idx_bezier << " "
             << idx_smoothpoint << " " << p_bez->sequ_point[idx_smoothpoint] << " "
             << pos << " " << p_bez->sequ_t[idx_smoothpoint] << endl;

        double t = p_bez->getParameterClosestToPosition(pos,
                                                        p_bez->sequ_t[idx_smoothpoint]);

        cout << "After Newton: point, t = " << p_bez->getPoint(t) << " " << t << endl;

        // add a new space to the end of bezier array
        beziers.push_back(*p_bez);
        size = beziers.size();
        for (int i=size-1; i>idx_bezier; --i)
        {
            beziers[i] = beziers[i-1];
        }

        // split the bezier to two beziers using De Casteljau algorithm
        splitByDeCasteljau(beziers[idx_bezier], beziers[idx_bezier+1], t);

        // re-compute the two new beziers
        beziers[idx_bezier].init();
        beziers[idx_bezier+1].init();

        return true;
    }
    else
    {
        return false;
    }
}

void BezierSequence::splitByDeCasteljau(Bezier &bez1, Bezier &bez2, double t)
{
    zVec2 p0 = bez1.control_point[0];
    zVec2 p1 = bez1.control_point[1];
    zVec2 p2 = bez1.control_point[2];
    zVec2 p3 = bez1.control_point[3];

    // the first split
    zVec2 a0 = p0 + (p1-p0) * t;
    zVec2 a1 = p1 + (p2-p1) * t;
    zVec2 a2 = p2 + (p3-p2) * t;

    // the second split
    zVec2 b0 = a0 + (a1-a0) * t;
    zVec2 b1 = a1 + (a2-a1) * t;

    // the third split
    zVec2 c0 = b0 + (b1-b0) * t;

    // update the corresponding control points
    bez1.control_point[0] = p0;
    bez1.control_point[1] = a0;
    bez1.control_point[2] = b0;
    bez1.control_point[3] = c0;

    bez2.control_point[0] = c0;
    bez2.control_point[1] = b1;
    bez2.control_point[2] = a2;
    bez2.control_point[3] = p3;
}
