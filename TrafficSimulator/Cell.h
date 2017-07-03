#ifndef BOUNDARYELEMENT_H
#define BOUNDARYELEMENT_H

#include "global.h"
//#include "Particle.h"
//#include "CarParticle.h"

class Cell
{
public:
    Cell(){}
    Cell(zVec2 p0, zVec2 p1, zVec2 p2, zVec2 p3);
//    Cell(zVec2 p0, zVec2 p1, zVec2 p2, zVec2 p3, int i, int ri);

    zVec2 v[4];  // A cell is a quadrilateral determined by four verteices of right hand rule
    int id;
    int road_id; // the road which the cell belongs to.

    // The average center information
    zVec2 center_point;
    zVec2 center_normal;
    zVec2 center_tangent;
//    double center_length;

    // The particles in the cell.
//    vector<Particle*> part_list;
//    vector<CarParticle*> car_list;

    // The other cells overlapping with the cell.
    vector<Cell*> overlapping_cells;
    bool isOverlappingCell;

    // The X position of the start edge of the cell.
    double x_position, x_mapped;

    void setCenterToPoint(zVec2 pos);
    void setCenterToGeometricCenter();

    // Given the vertices and the center, compute edge/tangent/normal etc.
    void init();

    // Justify if a point is in the zone
    bool isInZone(zVec2 p);

    // Justify if a point is in the extended zone
    bool isInExtendedZone(double &penetration_dist, zVec2 &norm, zVec2 p);

    // Justify if p0p1 intersects the boundary of the zone
    // return the intersection point p and the normal, if TRUE
    bool computeIntersection(zVec2 &cp, zVec2 &norm, zVec2 p0, zVec2 p1);

    // Justify if p0p1 intersects the edge v1v2 or v0v3 of the cell
    // return the intersection point cp and the normal if TRUE
    bool isIntersectedWithCellSides(zVec2 &cp, zVec2 &norm, zVec2 p0, zVec2 p1);

    // rigid body movement of the cell
    void scale(double factor);
    void translate(zVec2 pos);
    void rotate(double theta);
    void align(zVec2 direct);

private:
    bool isIntersected(zVec2 &v, zVec2 p0, zVec2 p1, int line_type, int edge_id);
    zVec2 getNormal(zVec2 p0, zVec2 p1);

    zVec2 getCenter();
    zVec2 getCenterNormal();
    zVec2 getCenterTangent();

    // classify the four edges of the cell, i.e. v1v0, v2v1, v3v2, v0v3.
    // in order to speed up the intersection testing
    // type 0: parallel to x axis
    // type 1: parallel to y axis
    // type 2: otherwise
    int edge_type[4];

    // the delta value of the four edges
    zVec2 edge[4];

    // the normals of the four edges by right hand rule, i.e. pointing to cell interior
    zVec2 edge_norm[4];

    // the sorted vertices are corresponding to the four edges, such that
    // type 0: v_start[i].x <= v_end[i].x
    // type 1: v_start[i].y <= v_end[i].y
    // type 2: v_start[i].x <= v_end[i].x
    zVec2 v_start[4];
    zVec2 v_end[4];
};

#endif // BOUNDARYELEMENT_H
