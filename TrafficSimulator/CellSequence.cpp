#include "CellSequence.h"

CellSequence::CellSequence()
{

}

void CellSequence::clearCells()
{
    int size = cells.size();
    for (int i=0; i<size; ++i)
    {
        delete cells[i];
    }
    cells.clear();
}

// determine the bounding box and the source and destination cell
void CellSequence::init(int road_id_)
{
    road_id = road_id_;

    // set up the cells and assign ids
    int size = cells.size();
    for (int i=0; i<size; ++i)
    {
        Cell *p_cell = cells[i];
        p_cell->id = i;
        p_cell->road_id = road_id;
        p_cell->setCenterToGeometricCenter();
        p_cell->init();
    }

    // find the bounding box of the area, at least 1 by 1
    max_x = 0.5;
    max_y = 0.5;
    min_x = -0.5;
    min_y = -0.5;
    for (int i=0; i<size; ++i)
    {
        for (int j=0; j<4; ++j)
        {
            zVec2 p = cells[i]->v[j];
            if (min_x > p.x)
            {
                min_x = p.x;
            }
            if (max_x < p.x)
            {
                max_x = p.x;
            }
            if (min_y > p.y)
            {
                min_y = p.y;
            }
            if (max_y < p.y)
            {
                max_y = p.y;
            }
        }
    }



//    // compute the total central length of the cells
//    double len_total = 0.0;
//    for (int i=0; i<size; ++i)
//    {
//        Cell *p_cell = cells[i];
//        len_total += p_cell->center_length;
//    }

//    // determine the relative postion of each cell to the whole bounding area
//    double len_acc = 0.0;
//    for (int i=0; i<size; ++i)
//    {
//        Cell *p_cell = cells[i];
//        p_cell->x0_mapped = len_acc / len_total;

//        len_acc += p_cell->center_length;
//        p_cell->x1_mapped = len_acc / len_total;
//    }

    // set the source and destination which are half of the head and tail cell
    source = *cells[0];
    source.v[3] = (source.v[0] + source.v[3]) / 2.0;
    source.v[2] = (source.v[1] + source.v[2]) / 2.0;
    source.setCenterToGeometricCenter();
    source.init();

    destination = *cells[size-1];
    destination.v[0] = (destination.v[0] + destination.v[3]) / 2.0;
    destination.v[1] = (destination.v[1] + destination.v[2]) / 2.0;
    destination.setCenterToGeometricCenter();
    destination.init();
}

bool CellSequence::isInArea(zVec2 p)
{
    int num_zone = cells.size();
    for (int i=0; i<num_zone; ++i)
    {
        Cell *p_ele = cells[i];
        if (p_ele->isInZone(p))
        {
            return true;
        }
    }

    return false;
}

bool CellSequence::isInArea(zVec2 p, int &idx)
{
    int num_zone = cells.size();
    for (int i=0; i<num_zone; ++i)
    {
        Cell *p_ele = cells[i];
        if (p_ele->isInZone(p))
        {
            idx = i;
            return true;
        }
    }

    idx = -1;
    return false;
}

int CellSequence::getCellId(zVec2 p)
{
    int num_zone = cells.size();
    for (int i=0; i<num_zone; ++i)
    {
        Cell *p_ele = cells[i];
        if (p_ele->isInZone(p))
        {
            return i;
        }
    }

    return -1;
}

//int CellSequence::getCellIdByXPosition(double pos)
//{
//    if (pos < 0 || pos > cells_central_length)
//    {
//        return -1;
//    }
//    else
//    {
//        int num_cells = cells.size();
//        int id = num_cells * (pos / cells_central_length);
//        if (id == num_cells)
//        {
//            return id-1;
//        }
//        else
//        {
//            return id;
//        }
//    }
//}


