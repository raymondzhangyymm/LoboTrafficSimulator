#include "Cell.h"

Cell::Cell(zVec2 p0, zVec2 p1, zVec2 p2, zVec2 p3)
{
    v[0] = p0;
    v[1] = p1;
    v[2] = p2;
    v[3] = p3;

    isOverlappingCell = false;

//    setCenterToGeometricCenter();
//    init();
}

//Cell::Cell(zVec2 p0, zVec2 p1, zVec2 p2, zVec2 p3, int i, int parent_i)
//{
//    v[0] = p0;
//    v[1] = p1;
//    v[2] = p2;
//    v[3] = p3;

//    isOverlappingCell = false;
//    id = i;
//    road_id = parent_i;

//    setCenterToGeometricCenter();
//    init();
//}

void Cell::setCenterToPoint(zVec2 pos)
{
    center_point = pos;
}

void Cell::setCenterToGeometricCenter()
{
    if (v[0].y == v[2].y)
    {
        // v0v2 parallel to x axis
        double t = (v[0].y - v[1].y) / (v[3].y - v[1].y);
        center_point.x = (v[3].x - v[1].x) * t + v[1].x;
        center_point.y = v[0].y;
    }
    else if (v[0].x == v[2].x)
    {
        // v0v2 parallel to y axis
        double t = (v[0].x - v[1].x) / (v[3].x - v[1].x);
        center_point.x = v[0].x;
        center_point.y = (v[3].y - v[1].y) * t + v[1].y;
    }
    else
    {
        double a0 = v[3].x - v[1].x, b0 = v[1].x;
        double c0 = v[3].y - v[1].y, d0 = v[1].y;
        double a1 = v[2].x - v[0].x, b1 = v[0].x;
        double c1 = v[2].y - v[0].y, d1 = v[0].y;

        // solve t0 and t1 for
        // a0*t0+b0 = a1*t1+b1
        // c0*t0+d0 = c1*t1+d1
        // where both a1 and c1 can not be zero.

        // i.e. c1*a0*t0+c1*b0 = c1*a1*t1+c1*b1
        //      a1*c0*t0+a1*d0 = a1*c1*t1+a1*d1
        // i.e. (c1*a0 - a1*c0)*t0 + c1*b0-a1*d0 = c1*b1 - a1*d1
        double t0 = (c1*b1 - a1*d1 - c1*b0 + a1*d0) / (c1*a0 - a1*c0);
        center_point.x = a0 * t0 + b0;
        center_point.y = c0 * t0 + d0;
    }
}

void Cell::init()
{
    // determine the edge types
    for (int i=0; i<4; ++i)
    {
        int j = (i+1)%4;

        // edge from v[i] to v[j]
        if (v[j].y == v[i].y)
        {
            // parallel to x axis
            edge_type[i] = 0;
            edge[i].x = v[j].x - v[i].x;
            edge[i].y = 0.0;

            if (edge[i].x >= 0.0)
            {
                v_start[i] = v[i];
                v_end[i] = v[j];
                edge_norm[i] = zVec2(0.0, 1.0);
            }
            else
            {
                v_start[i] = v[j];
                v_end[i] = v[i];
                edge_norm[i] = zVec2(0.0, -1.0);
            }
        }
        else if (v[j].x == v[i].x)
        {
            // parallel to y axis
            edge_type[i] = 1;
            edge[i].x = 0.0;
            edge[i].y = v[j].y - v[i].y;

            if (edge[i].y >= 0.0)
            {
                v_start[i] = v[i];
                v_end[i] = v[j];
                edge_norm[i] = zVec2(-1.0, 0.0);
            }
            else
            {
                v_start[i] = v[j];
                v_end[i] = v[i];
                edge_norm[i] = zVec2(1.0, 0.0);
            }
        }
        else
        {
            edge_type[i] = 2;
            edge[i].x = v[j].x - v[i].x;
            edge[i].y = v[j].y - v[i].y;

            if (edge[i].x >= 0.0)
            {
                v_start[i] = v[i];
                v_end[i] = v[j];
            }
            else
            {
                v_start[i] = v[j];
                v_end[i] = v[i];
            }

            edge_norm[i].x = 1.0;
            edge_norm[i].y = - edge[i].x / edge[i].y;

            // make sure satisfy the right hand rule and normalize.
            if (edge[i].x * edge_norm[i].y - edge[i].y * edge_norm[i].x > 0.0)
            {
                edge_norm[i] /= edge_norm[i].length();
            }
            else
            {
                edge_norm[i] /= -edge_norm[i].length();
            }
        }
    }

    // the tangent
    zVec2 vv1, vv2;
    vv1.x = -(v[1].y - v[0].y);
    vv1.y = v[1].x - v[0].x;
    vv1.normalize();
    vv2.x = -(v[2].y - v[3].y);
    vv2.y = v[2].x - v[3].x;
    vv2.normalize();
    center_tangent = vv1 + vv2;
    center_tangent.normalize();

//    double alpha = atan2(center_tangent.y, center_tangent.x);
//    center_normal.x = cos(alpha + PI / 2.0);
//    center_normal.y = sin(alpha + PI / 2.0);
    center_normal.x = -center_tangent.y;
    center_normal.y = center_tangent.x;

//    zVec2 mm = ((v[0]+v[1]) - (v[3]+v[2])) / 2.0;
//    center_length = mm.length();
}

// Compute the normal of the line segment, satisfy right hand rule.
// i.e. p0p1 X norm > 0, where p0p1 = p1 - p0.
zVec2 Cell::getNormal(zVec2 p0, zVec2 p1)
{
    if (p0.y == p1.y)
    {
        // the line segment is aligned to x-axis
        if (p0.x <= p1.x)
        {
            return zVec2(0.0, 1.0);
        }
        else
        {
            return zVec2(0.0, -1.0);
        }
    }
    else
    {
        // Solve (p1.x - p0.x) * nl.x + (p1.y - p0.y) * nl.y = 0
        zVec2 p0p1 = p1 - p0;
        zVec2 nl;
        nl.x = 1;
        nl.y = - p0p1.x / p0p1.y;

        // make sure satisfy the right hand rule.
        if (p0p1.x * nl.y - p0p1.y * nl.x > 0.0)
        {
            return nl / nl.length();
        }
        else
        {
            return nl / (- nl.length());
        }
    }
}

void Cell::scale(double factor)
{
    for (int i=0; i<4; ++i)
    {
        zVec2 vv = v[i] - center_point;
        v[i] = vv * factor + center_point;
    }
}

void Cell::translate(zVec2 pos)
{
    zVec2 vv = pos - center_point;
    for (int i=0; i<4; ++i)
    {
        v[i] += vv;
    }
    center_point = pos;
}

void Cell::rotate(double theta)
{
    double costheta = cos(theta);
    double sintheta = sin(theta);

    for (int i=0; i<4; ++i)
    {
        zVec2 vv = v[i] - center_point;
        v[i].x = costheta * vv[0] - sintheta * vv[1] + center_point.x;
        v[i].y = sintheta * vv[0] + costheta * vv[1] + center_point.y;
    }

    init();
}

void Cell::align(zVec2 direct)
{
    double theta_1 = atan2(center_tangent.y, center_tangent.x);
    double theta_2 = atan2(direct.y, direct.x);

    double costheta = cos(theta_2 - theta_1);
    double sintheta = sin(theta_2 - theta_1);

    for (int i=0; i<4; ++i)
    {
        zVec2 vv = v[i] - center_point;
        v[i].x = costheta * vv[0] - sintheta * vv[1] + center_point.x;
        v[i].y = sintheta * vv[0] + costheta * vv[1] + center_point.y;
    }

    init();
}

bool Cell::isInZone(zVec2 p)
{
    // Position on the edge_1 or edge_3 is considered to be out of the zone.
    if ((p.x-v[0].x)*edge[0].y <= (p.y-v[0].y)*edge[0].x &&
        (p.x-v[1].x)*edge[1].y < (p.y-v[1].y)*edge[1].x &&
        (p.x-v[2].x)*edge[2].y <= (p.y-v[2].y)*edge[2].x &&
        (p.x-v[3].x)*edge[3].y < (p.y-v[3].y)*edge[3].x)
    {
        return true;
    }

    return false;
}

bool Cell::isInExtendedZone(double &penetration_dist, zVec2 &norm, zVec2 p)
{
    // check if it is between edge v0v1 and v2v3
    if ((p.x-v[0].x)*edge[0].y <= (p.y-v[0].y)*edge[0].x &&
        (p.x-v[2].x)*edge[2].y <= (p.y-v[2].y)*edge[2].x)
    {
        // check if it is in the left extended zone
        double a = edge[3].y;
        double b = - edge[3].x;
        double c = - v[3].x*a - v[3].y*b;
        double dist = (a * p.x + b * p.y + c) / sqrt(a*a + b*b);
        if (dist <= 0.0)
        {
            penetration_dist = dist;
            norm = edge_norm[3];
            return true;
        }
        // check if it is in the right extended zone
        a = edge[1].y;
        b = - edge[1].x;
        c = - v[1].x*a - v[1].y*b;
        dist = (a * p.x + b * p.y + c) / sqrt(a*a + b*b);
        if (dist <= 0.0)
        {
            penetration_dist = dist;
            norm = edge_norm[1];
            return true;
        }
    }

    return false;
}

// Justify if p0p1 intersects the boundary of the zone
// return the intersection point p and the normal, if TRUE
// It is assumed that p0 is within the zone.
bool Cell::computeIntersection(zVec2 &cp, zVec2 &norm, zVec2 p0, zVec2 p1)
{
    // determine the type of the input line p0p1
//    zVec2 p0_orig = p0;
//    zVec2 p1_orig = p1;
    int p0p1_type;
    double dx_p0p1, dy_p0p1;
    if (p1.y == p0.y)
    {
        p0p1_type = 0;
        dx_p0p1 = p1.x - p0.x;
        dy_p0p1 = 0.0;

        if (dx_p0p1 < 0.0)
        {
            zVec2 temp = p0;
            p0 = p1;
            p1 = temp;
        }
    }
    else if (p1.x == p0.x)
    {
        p0p1_type = 1;
        dx_p0p1 = 0.0;
        dy_p0p1 = p1.y - p0.y;

        if (dy_p0p1 < 0.0)
        {
            zVec2 temp = p0;
            p0 = p1;
            p1 = temp;
        }
    }
    else
    {
        p0p1_type = 2;
        dx_p0p1 = p1.x - p0.x;
        dy_p0p1 = p1.y - p0.y;

        if (dx_p0p1 < 0.0)
        {
            zVec2 temp = p0;
            p0 = p1;
            p1 = temp;
        }
    }

    // check with the edges
    for (int i=0; i<4; ++i)
    {
        switch (edge_type[i])
        {
        case 0:
            // the edge is parallel to x axis
            if (p0p1_type == 0)
            {
                // p0p1 is parallel to x axis
                if (v_start[i].y == p0.y)
                {
                    // the two lines are colinear
                    if (v_start[i].x <= p0.x && p0.x <= v_end[i].x)
                    {
                        // p0 is within the edge
                        if (p1.x <= v_end[i].x)
                        {
                            cp.x = p1.x;
                        }
                        else
                        {
                            cp.x = v_end[i].x;
                        }
                        cp.y = v_start[i].y;
                        norm = edge_norm[i];
                        return true;
                    }
                    else if (v_start[i].x <= p1.x && p1.x <= v_end[i].x)
                    {
                        // p1 is within the edge
                        if (p0.x >= v_start[i].x)
                        {
                            cp.x = p0.x;
                        }
                        else
                        {
                            cp.x = v_start[i].x;
                        }
                        cp.y = v_start[i].y;
                        norm = edge_norm[i];
                        return true;
                    }
                }
            }
            else if (p0p1_type == 1)
            {
                // p0p1 is parallel to y axis
                if (v_start[i].x <= p0.x && p0.x <= v_end[i].x &&
                    p0.y <= v_start[i].y && v_start[i].y <= p1.y)
                {
                    cp.x = p0.x;
                    cp.y = v_start[i].y;
                    norm = edge_norm[i];
                    return true;
                }
            }
            else if (p0p1_type == 2)
            {
                // p0p1 is arbitry
                double t = (v_start[i].y - p0.y) / (p1.y - p0.y);
                if (0.0 <= t && t <= 1.0)
                {
                    cp.x = (p1.x - p0.x) * t + p0.x;
                    cp.y = v_start[i].y;
                    norm = edge_norm[i];
                    return true;
                }
            }
            break;

        case 1:
            // the edge is parallel to y axis
            if (p0p1_type == 0)
            {
                // p0p1 is parallel to x axis
                if (v_start[i].y <= p0.y && p0.y <= v_end[i].y &&
                    p0.x <= v_start[i].x && v_start[i].x <= p1.x)
                {
                    cp.x = v_start[i].x;
                    cp.y = p0.y;
                    norm = edge_norm[i];
                    return true;
                }
            }
            else if (p0p1_type == 1)
            {
                // p0p1 is parallel to y axis
                if (v_start[i].x == p0.x)
                {
                    // the two lines are colinear
                    if (v_start[i].y <= p0.y && p0.y <= v_end[i].y)
                    {
                        // p0 is within the edge
                        if (p1.y <= v_end[i].y)
                        {
                            cp.y = p1.y;
                        }
                        else
                        {
                            cp.y = v_end[i].y;
                        }
                        cp.x = v_start[i].x;
                        norm = edge_norm[i];
                        return true;
                    }
                    else if (v_start[i].y <= p1.y && p1.y <= v_end[i].y)
                    {
                        // p1 is within the edge
                        if (p0.y >= v_start[i].y)
                        {
                            cp.y = p0.y;
                        }
                        else
                        {
                            cp.y = v_start[i].y;
                        }
                        cp.x = v_start[i].x;
                        norm = edge_norm[i];
                        return true;
                    }
                }
            }
            else if (p0p1_type == 2)
            {
                // p0p1 is arbitry
                double t = (v_start[i].x - p0.x) / (p1.x - p0.x);
                if (0.0 <= t && t <= 1.0)
                {
                    cp.x = v_start[i].x;
                    cp.y = (p1.y - p0.y) * t + p0.y;
                    norm = edge_norm[i];
                    return true;
                }
            }
            break;

        case 2:
            // the edge is arbitry
            if (p0p1_type == 0)
            {
                //the p0p1 is parallel to x axis
                double t = (p0.y - v_start[i].y) / (v_end[i].y - v_start[i].y);
                if (0.0 <= t && t <= 1.0)
                {
                    cp.x = (v_end[i].x - v_start[i].x) * t + v_start[i].x;
                    cp.y = p0.y;
                    norm = edge_norm[i];
                    return true;
                }
            }
            else if (p0p1_type == 1)
            {
                //the p0p1 is parallel to y axis
                double t = (p0.x - v_start[i].x) / (v_end[i].x - v_start[i].x);
                if (0.0 <= t && t <= 1.0)
                {
                    cp.x = p0.x;
                    cp.y = (v_end[i].y - v_start[i].y) * t + v_start[i].y;
                    norm = edge_norm[i];
                    return true;
                }
            }
            else if (p0p1_type == 2)
            {
                //both the edge and p0p1 are arbitry
                double a0 = v_end[i].x - v_start[i].x, b0 = v_start[i].x;
                double c0 = v_end[i].y - v_start[i].y, d0 = v_start[i].y;
                double a1 = p1.x - p0.x, b1 = p0.x;
                double c1 = p1.y - p0.y, d1 = p0.y;

                if (c1*a0 == a1*c0)
                {
                    // they are parallel to each other.
                    if (c1*b0 - a1*d0 == c1*b1 - a1*d1)
                    {
                        // they are colinear
                        if (v_start[i].x <= p0.x && p0.x <= v_end[i].x)
                        {
                            // p0 is within the edge
                            if (p1.x <= v_end[i].x)
                            {
                                cp.x = p1.x;
                            }
                            else
                            {
                                cp.x = v_end[i].x;
                            }
                            cp.y = v_start[i].y;
                            norm = edge_norm[i];
                            return true;
                        }
                        else if (v_start[i].x <= p1.x && p1.x <= v_end[i].x)
                        {
                            // p1 is within the edge
                            if (p0.x >= v_start[i].x)
                            {
                                cp.x = p0.x;
                            }
                            else
                            {
                                cp.x = v_start[i].x;
                            }
                            cp.y = v_start[i].y;
                            norm = edge_norm[i];
                            return true;
                        }
                    }
                }
                else
                {
                    // they are not parallel to each other.
                    // solve t0 and t1 for
                    // a0*t0+b0 = a1*t1+b1
                    // c0*t0+d0 = c1*t1+d1
                    // i.e. c1*a0*t0+c1*b0 = c1*a1*t1+c1*b1
                    //      a1*c0*t0+a1*d0 = a1*c1*t1+a1*d1
                    // i.e. (c1*a0 - a1*c0)*t0 + c1*b0-a1*d0 = c1*b1 - a1*d1
                    double t0 = (c1*b1 - a1*d1 - c1*b0 + a1*d0) / (c1*a0 - a1*c0);
                    double t1 = (a0*t0 + b0 - b1) / a1;
                    if (0.0 <= t0 && t0 <= 1.0 &&
                        0.0 <= t1 && t1 <= 1.0)
                    {
                        cp.x = a1 * t1 + b1;
                        cp.y = c1 * t1 + d1;
                        norm = edge_norm[i];
                        return true;
                    }
                }
                break;
            }
        }
    }

    return false;
}

bool Cell::isIntersectedWithCellSides(zVec2 &cp, zVec2 &norm, zVec2 p0, zVec2 p1)
{
    // Check with the edge v1v2
    // Use parametric forms of the line segments
    double a0 = v[2].x - v[1].x, b0 = v[1].x;
    double c0 = v[2].y - v[1].y, d0 = v[1].y;
    double a1 = p1.x - p0.x, b1 = p0.x;
    double c1 = p1.y - p0.y, d1 = p0.y;

    double det = c0*a1 - a0*c1;
    if (det != 0.0)
    {
        // They are not parallel to each other.
        // solve t0 and t1 for
        // a0*t0+b0 = a1*t1+b1
        // c0*t0+d0 = c1*t1+d1
        // i.e. c1*a0*t0+c1*b0 = c1*a1*t1+c1*b1
        //      a1*c0*t0+a1*d0 = a1*c1*t1+a1*d1
        // i.e. (c1*a0 - a1*c0)*t0 + c1*b0-a1*d0 = c1*b1 - a1*d1
        double t0 = (a1*(d1-d0) - c1*(b1-b0)) / det;
        double t1 = (a0*(d1-d0) - c0*(b1-b0)) / det;

        // check if it is a valid intersection
        if (0.0 <= t0 && t0 <= 1.0 && 0.0 <= t1 && t1 <= 1.0)
        {
            cp.x = a1 * t1 + b1;
            cp.y = c1 * t1 + d1;
            norm = edge_norm[1];
            return true;
        }
    }

    // Check with the edge v1v2
    a0 = v[3].x - v[0].x; b0 = v[0].x;
    c0 = v[3].y - v[0].y; d0 = v[0].y;
    det = c0*a1 - a0*c1;
    if (det != 0.0)
    {
        // They are not parallel to each other.
        // solve t0 and t1 for
        // a0*t0+b0 = a1*t1+b1
        // c0*t0+d0 = c1*t1+d1
        // i.e. c1*a0*t0+c1*b0 = c1*a1*t1+c1*b1
        //      a1*c0*t0+a1*d0 = a1*c1*t1+a1*d1
        // i.e. (c1*a0 - a1*c0)*t0 + c1*b0-a1*d0 = c1*b1 - a1*d1
        double t0 = (a1*(d1-d0) - c1*(b1-b0)) / det;
        double t1 = (a0*(d1-d0) - c0*(b1-b0)) / det;

        // check if it is a valid intersection
        if (0.0 <= t0 && t0 <= 1.0 && 0.0 <= t1 && t1 <= 1.0)
        {
            cp.x = a1 * t1 + b1;
            cp.y = c1 * t1 + d1;
            norm = edge_norm[3];
            return true;
        }
    }

    return false;
}

