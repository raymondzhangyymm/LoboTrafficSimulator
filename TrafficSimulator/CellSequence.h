#ifndef CELLSEQUENCE_H
#define CELLSEQUENCE_H

#include "global.h"
#include "Cell.h"

// The polygon consists of a number of cells
// Each cell is a trapezoid which is garanteed to be convex.
class CellSequence
{
public:
    CellSequence();

    vector<Cell*> cells;
    double min_x, max_x, min_y, max_y;
    Cell source;
    Cell destination;

    // information for collision detection
    zVec2 collision_contact_point;
    zVec2 collision_surface_normal;
    double penetration_depth;

//    void init(vector<Cell*> cells_, int road_id_, double length_);
    void init(int road_id_);

    void clearCells();

    bool isInArea(zVec2 p);
    bool isInArea(zVec2 p, int &idx);

    int getCellId(zVec2 p);
//    int getCellIdByXPosition(double pos);

private:
    int road_id;
//    double cells_central_length;
};

#endif // CellSequence_H
