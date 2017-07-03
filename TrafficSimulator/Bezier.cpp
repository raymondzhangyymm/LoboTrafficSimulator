#include "Bezier.h"

Bezier::Bezier(zVec2 p0, zVec2 p1, zVec2 p2, zVec2 p3)
{
    // the bezier will be divided by 11 times, i.e. 2^11 = 2048 segments.
    optimal_subdivision = 11;

    control_point[0] = p0;
    control_point[1] = p1;
    control_point[2] = p2;
    control_point[3] = p3;

    control_point_backup[0] = control_point[0];
    control_point_backup[1] = control_point[1];
    control_point_backup[2] = control_point[2];
    control_point_backup[3] = control_point[3];
}

Bezier::Bezier(vector<zVec2> pa_)
{
    if (pa_.size() < 4)
    {
        cout << "ERROR! Instantating bezier class failed!\n";
        return;
    }

    // the bezier will be divided by 11 times, i.e. 2^11 = 2048 segments.
    optimal_subdivision = 11;

    control_point[0] = pa_[0];
    control_point[1] = pa_[1];
    control_point[2] = pa_[2];
    control_point[3] = pa_[3];

    control_point_backup[0] = control_point[0];
    control_point_backup[1] = control_point[1];
    control_point_backup[2] = control_point[2];
    control_point_backup[3] = control_point[3];
}

Bezier::~Bezier()
{
}

void Bezier::init()
{
    // check with the chord lenght and the tangents.
//    double ll1 = (control_point[0]-control_point[3]).length();
//    double ll2 = (control_point[0]-control_point[1]).length();
//    double ll3 = (control_point[2]-control_point[3]).length();
//    if (ll1 < DEFAULT_LANE_CELL_LENGTH || ll2 < 1e-3 || ll3 < 1e-3)
//    {
//        cout << "Warning! SHORT BEZIER ==== " << ll1 << " " << ll2 << " " << ll3 << endl;
//    }

    // compute the coefficientes
    xa =   -control_point[0].x + 3*control_point[1].x - 3*control_point[2].x + control_point[3].x;
    xb =  3*control_point[0].x - 6*control_point[1].x + 3*control_point[2].x;
    xc = -3*control_point[0].x + 3*control_point[1].x;
    xd =    control_point[0].x;

    ya =   -control_point[0].y + 3*control_point[1].y - 3*control_point[2].y + control_point[3].y;
    yb =  3*control_point[0].y - 6*control_point[1].y + 3*control_point[2].y;
    yc = -3*control_point[0].y + 3*control_point[1].y;
    yd =    control_point[0].y;

    // the smooth points are used to draw the approximation and
    // referenced for searching the closest point as well
    int num_smooth = 100;
    sequ_t.resize(num_smooth);
    sequ_point.resize(num_smooth);
    sequ_normal.resize(num_smooth);
    sequ_tangent.resize(num_smooth);

    // generate the points except the last one.
    double t_step = 1.0 / double(num_smooth-1);
    double t = 0.0;
    for (int i=0; i<num_smooth-1; ++i)
    {
        sequ_t[i] = t;
        sequ_point[i] = getPoint(t);
        sequ_normal[i] = getNormal(t);
        sequ_tangent[i] = getTangent(t);

        t += t_step;
    }

    // the last point is t=1.0;
    int last_id = num_smooth-1;
    sequ_t[last_id] = 1.0;
    sequ_point[last_id] = getPoint(1.0);
    sequ_normal[last_id] = getNormal(1.0);
    sequ_tangent[last_id] = getTangent(1.0);

    // build the table and compute the arc length.
    computeArcLengthBySubdivision();

    backupControlPoints();
}

void Bezier::backupControlPoints()
{
    control_point_backup[0] = control_point[0];
    control_point_backup[1] = control_point[1];
    control_point_backup[2] = control_point[2];
    control_point_backup[3] = control_point[3];
}

void Bezier::restoreControlPoints()
{
    control_point[0] = control_point_backup[0];
    control_point[1] = control_point_backup[1];
    control_point[2] = control_point_backup[2];
    control_point[3] = control_point_backup[3];
}

zVec2 Bezier::getPoint(double t)
{
    if (t < 0.0)
    {
        t = 0.0;
    }
    else if (t > 1.0)
    {
        t = 1.0;
    }

    double t3 = t*t*t;
    double t2 = t*t;
    return zVec2(xa*t3 + xb*t2 + xc*t + xd, ya*t3 + yb*t2 + yc*t + yd);
}

zVec2 Bezier::getTangent(double t)
{
    if (t < 0.0)
    {
        t = 0.0;
    }
    else if (t > 1.0)
    {
        t = 1.0;
    }

    zVec2 v;
    v.x = 3 * xa*t*t + 2 * xb*t + xc;
    v.y = 3 * ya*t*t + 2 * yb*t + yc;
    return v / v.length();
}

zVec2 Bezier::getNormal(double t)
{
    if (t < 0.0)
    {
        t = 0.0;
    }
    else if (t > 1.0)
    {
        t = 1.0;
    }

    // rotate the tangent anti-clockwisely by 90 degree,
    // i.e tangent and normal satisfy right hand rule.
    zVec2 v;
    v.x = - 3 * ya*t*t - 2 * yb*t - yc;
    v.y = 3 * xa*t*t + 2 * xb*t + xc;
    return v / v.length();
}

double Bezier::getArcLength(double t)
{
    int num_points = arclength_array.size();
    int idx = num_points * (t);
    if (idx >= num_points)
    {
        idx = num_points - 1;
    }
    else if (idx < 0)
    {
        idx = 0;
    }

    return arclength_array[idx];
}

double Bezier::getParameterByArcLength(double arc_)
{
    if (arc_ <= 0.0)
    {
        return 0.0;
    }
    else if (arc_ >= length)
    {
        return 1.0;
    }

    int size_array = arclength_array.size();

    // binary searching the table for the closest element
    int low = 0, high = size_array - 1, middle;
    while (low <= high)
    {
        middle = (low + high) / 2;
        if (arc_ == arclength_array[middle])
        {
            return double(middle+1) / double(size_array);
        }
        else if (arc_ < arclength_array[middle])
        {
            high = middle - 1;
        }
        else
        {
            low = middle + 1;
        }
    }

    if (arc_ < arclength_array[middle])
    {
        return (double(middle) + 0.5) / double(size_array);
    }
    else
    {
        return (double(middle) + 1.5) / double(size_array);
    }


//    if (arc_ <= 0.0)
//    {
//        return 0.0;
//    }
//    else if (arc_ >= length)
//    {
//        return 1.0;
//    }

//    double len = 0.0;
//    double t = 0.0;
//    double t_step = 1.0 / 10000.0;
//    zVec2 moving_point_0 = getPoint(0.0);
//    zVec2 moving_point_1;

//    t += t_step;
//    while (t <= 1.0)
//    {
//        moving_point_1 = getPoint(t);
//        len += (moving_point_1 - moving_point_0).length();
//        if (len > arc_)
//        {
//            return t-t_step*0.5;
//        }

//        moving_point_0 = moving_point_1;
//        t += t_step;
//    }

//    return 1.0;
}

double Bezier::getParameterClosestToPosition(zVec2 pos, double t_init)
{
    double tn = t_init;

    zVec2 p_t = getPoint(t_init);
    zVec2 dp_t = getFirstDerivative(t_init);
    zVec2 ddp_t = getSecondDerivative(t_init);

    zVec2 delta_p = p_t - pos;

    double residual = (delta_p.x * dp_t.x + delta_p.y * dp_t.y) /
                      (ddp_t.x*ddp_t.x + ddp_t.y*ddp_t.y + delta_p.x * ddp_t.x + delta_p.y * ddp_t.y);

    int i = 0;
    while (fabs(residual) > 1e-5 && i < 100)
    {
        i++;

        tn = tn - residual;

        p_t = getPoint(tn);
        dp_t = getFirstDerivative(tn);
        ddp_t = getSecondDerivative(tn);

        delta_p = p_t - pos;

        residual = (delta_p.x * dp_t.x + delta_p.y * dp_t.y) /
                   (ddp_t.x*ddp_t.x + ddp_t.y*ddp_t.y + delta_p.x * ddp_t.x + delta_p.y * ddp_t.y);

//        cout << "i, t and residual = " << i << " " << tn << " " << residual << endl;
    }

    if (fabs(residual) > 1e-5)
    {
        cout << "Warning! Newton's method failed. Residual = " << residual << endl;
        return t_init;
    }

    if (tn < 0)
    {
        return 0.0;
    }
    if (tn > 1.0)
    {
        return 1.0;
    }

    return tn;
}

bool Bezier::getParameterClosestToPositionByBisection(zVec2 pos, double &result_t)
{
    double tolerance = 1e-4;
    int max_iteration = 10;

    // divide the bezier to 100 intervals
    int num_interval = 100;
    double tt_step = 1.0 / double(num_interval);
    vector<double> tt(num_interval+1);
    vector<double> ff(num_interval+1);
    vector<double> rr; // the roots
    for (int i=0; i<num_interval; ++i)
    {
        tt[i] = tt_step * double(i);
        ff[i] = computeMinDistanceFunctionValue(pos, tt[i]);
        if (fabs(ff[i]) < tolerance)
        {
            rr.push_back(tt[i]);
        }

    }
    tt[num_interval] = 1.0;
    ff[num_interval] = computeMinDistanceFunctionValue(pos, 1.0);
    if (fabs(ff[num_interval]) < tolerance)
    {
        rr.push_back(1.0);
    }

    // find the roots within intervals
    for (int i=0; i<num_interval; ++i)
    {
        int tta = tt[i], ttb = tt[i+1];
        double ffa = ff[i], ffb = ff[i+1];
        if (ffa*ffb < 0.0)
        {
            double ttc;
            int k;
            for (k=0; k<max_iteration; ++k)
            {
                ttc = (tta + ttb) / 2.0;
                double ffc = computeMinDistanceFunctionValue(pos, ttc);
                if (fabs(ffc) < tolerance)
                {
                    rr.push_back(ttc);
                    break;
                }
                else if (ffa*ffc < 0.0)
                {
                    ttb = ttc;
                }
                else if (ffc*ffb < 0.0)
                {
                    tta = ttc;
                }
            }

            if (k >= max_iteration)
            {
                rr.push_back(ttc);
            }
        }
    }

    // find the closet points by the root candidates.
    int num_root = rr.size();
    if (num_root > 0)
    {
        double min_dist2 = 1e20; // initial minimun distance square
        int idx = 0;
        for (int i=0; i<num_root; ++i)
        {
            zVec2 v = getPoint(rr[i]) - pos;
            double d2 = v.x*v.x + v.y*v.y;
            if (d2 < min_dist2)
            {
                min_dist2 = d2;
                idx = i;
            }
        }

        result_t = rr[idx];
        return true;
    }
    else
    {
        return false;
    }
}

//----------------------------------------------------------------------------------------
zVec2 Bezier::getFirstDerivative(double t)
{
    return zVec2(3*xa*t*t + 2*xb*t + xc, 3*ya*t*t + 2*yb*t + yc);
}

zVec2 Bezier::getSecondDerivative(double t)
{
    return zVec2(6*xa*t + 2*xb, 6*ya*t + 2*yb);
}

void Bezier::computeArcLengthBySubdivision()
{
    arclength_array.clear();
    arclength_array.resize(pow(2, optimal_subdivision));

//    zVec2 cp[4];
//    cp[0] = control_point[0];
//    cp[1] = control_point[1];
//    cp[2] = control_point[2];
//    cp[3] = control_point[3];

//    double len_lower = (cp[0]-cp[3]).length();
//    double len_upper = (cp[0]-cp[1]).length() + (cp[1]-cp[2]).length() + (cp[2]-cp[3]).length();
//    length = (len_upper + len_lower) * 0.5;
//    return;


    // the original bezier
    vector<zVec2> cp_array;
    cp_array.resize(4);
    cp_array[0] = control_point[0];
    cp_array[1] = control_point[1];
    cp_array[2] = control_point[2];
    cp_array[3] = control_point[3];

    // divide the bezier by optimal_subdivision times
    for (int k=1; k<=optimal_subdivision; ++k)
    {
        int size = cp_array.size();
        vector<zVec2> cp_array_1;
        cp_array_1.resize(size*2);
        for (int i=0; i<size; i+=4)
        {
            zVec2 cp[4], cp_1[4], cp_2[4];
            cp[0] = cp_array[i+0];
            cp[1] = cp_array[i+1];
            cp[2] = cp_array[i+2];
            cp[3] = cp_array[i+3];

            subdivideInHalf(cp, cp_1, cp_2);

            cp_array_1[2*i+0] = cp_1[0];
            cp_array_1[2*i+1] = cp_1[1];
            cp_array_1[2*i+2] = cp_1[2];
            cp_array_1[2*i+3] = cp_1[3];
            cp_array_1[2*i+4] = cp_2[0];
            cp_array_1[2*i+5] = cp_2[1];
            cp_array_1[2*i+6] = cp_2[2];
            cp_array_1[2*i+7] = cp_2[3];
        }
        cp_array = cp_array_1;
    }

    // compute the length of individual segments.
    int size_array = arclength_array.size();
    for (int i=0; i<size_array; ++i)
    {
        zVec2 cp[4];
        cp[0] = cp_array[4*i+0];
        cp[1] = cp_array[4*i+1];
        cp[2] = cp_array[4*i+2];
        cp[3] = cp_array[4*i+3];

        double len_lower = (cp[0]-cp[3]).length();
        double len_upper = (cp[0]-cp[1]).length() + (cp[1]-cp[2]).length() + (cp[2]-cp[3]).length();
        arclength_array[i] = (len_upper + len_lower) * 0.5;

        if (len_upper - len_lower >= 1e-5)
        {
            cout << "Warning! len_upper - len_lower = " << len_upper - len_lower << endl;
        }
    }

    // accumulate the length.
    for (int i=1; i<size_array; ++i)
    {
        arclength_array[i] += arclength_array[i-1];
//        cout << "arclength_array " << i << " = " << arclength_array[i] << endl;
    }

    // the total length is the last element.
    length = arclength_array[size_array-1];

    return;

//    // set the tolerance to 0.00001.
//    double length_tolerance = 1e-5;

//    int times_subdivision = 0;
//    vector<zVec2> cp_array(4);
//    cp_array[0] = control_point[0];
//    cp_array[1] = control_point[1];
//    cp_array[2] = control_point[2];
//    cp_array[3] = control_point[3];

//    // subdivision times should not be greater than 12.
//    while (times_subdivision <= 12)
//    {
//        double len_lower = getLengthOfChord(cp_array);
//        double len_upper = getLengthOfControlPolygon(cp_array);
//        length = (len_upper + len_lower) * 0.5;
////        cout << "len_upper len_lower = " << len_upper << " " << len_lower << endl;

//        if (len_upper - len_lower <= length_tolerance)
//        {
//            optimal_subdivision = pow(2, times_subdivision);
//            return;
//        }
//        else
//        {
//            times_subdivision++;
//            int size = cp_array.size();

//            vector<zVec2> cp_array_1(size*2);
//            for (int i=0; i<size-3; i+=4)
//            {
//                zVec2 cp[4], cp_1[4], cp_2[4];
//                cp[0] = cp_array[i+0];
//                cp[1] = cp_array[i+1];
//                cp[2] = cp_array[i+2];
//                cp[3] = cp_array[i+3];

//                subdivideInHalf(cp, cp_1, cp_2);

//                cp_array_1[2*i+0] = cp_1[0];
//                cp_array_1[2*i+1] = cp_1[1];
//                cp_array_1[2*i+2] = cp_1[2];
//                cp_array_1[2*i+3] = cp_1[3];
//                cp_array_1[2*i+4] = cp_2[0];
//                cp_array_1[2*i+5] = cp_2[1];
//                cp_array_1[2*i+6] = cp_2[2];
//                cp_array_1[2*i+7] = cp_2[3];
//            }
//            cp_array = cp_array_1;
//        }
//    }

//    cout << "Dividing Bezier more than 12 times!\n";
//    optimal_subdivision = pow(2, times_subdivision);
//    return;
}

double Bezier::getLengthOfChord(vector<zVec2> &cps)
{
    double len = 0;

    int size = cps.size();
    for (int i=0; i<size; i+=4)
    {
        len += (cps[i] - cps[i+3]).length();
    }

    return len;
}

double Bezier::getLengthOfControlPolygon(vector<zVec2> &cps)
{
    double len = 0;

    int size = cps.size();
    for (int i=0; i<size; i+=4)
    {
        len += (cps[i] - cps[i+1]).length();
        len += (cps[i+1] - cps[i+2]).length();
        len += (cps[i+2] - cps[i+3]).length();
    }

    return len;
}

void Bezier::subdivideInHalf(const zVec2 cp[], zVec2 cp_1[], zVec2 cp_2[])
{
    zVec2 p0 = cp[0];
    zVec2 p1 = cp[1];
    zVec2 p2 = cp[2];
    zVec2 p3 = cp[3];

    // the first split
    zVec2 a0 = p0 + (p1-p0) * 0.5;
    zVec2 a1 = p1 + (p2-p1) * 0.5;
    zVec2 a2 = p2 + (p3-p2) * 0.5;

    // the second split
    zVec2 b0 = a0 + (a1-a0) * 0.5;
    zVec2 b1 = a1 + (a2-a1) * 0.5;

    // the third split
    zVec2 c0 = b0 + (b1-b0) * 0.5;

    // update the corresponding control points
    cp_1[0] = p0;
    cp_1[1] = a0;
    cp_1[2] = b0;
    cp_1[3] = c0;

    cp_2[0] = c0;
    cp_2[1] = b1;
    cp_2[2] = a2;
    cp_2[3] = p3;
}

double Bezier::computeMinDistanceFunctionValue(zVec2 pos, double tt)
{
    zVec2 va = getPoint(tt);
    zVec2 dva = getFirstDerivative(tt);

    return (va.x-pos.x) * dva.x + (va.y-pos.y) * dva.y;
}
