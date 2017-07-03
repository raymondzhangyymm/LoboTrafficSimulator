#include "HashTableX.h"

HashTableX::HashTableX(double length_, double support_)
{
    x_length = length_;
    support = support_;
}

int HashTableX::getHashKeyByXPosition(double pos)
{
    int id = table_size * (pos / x_length);
    if (id >= table_size)
    {
        return table_size-1;
    }
    else if (id <= 0)
    {
        return 0;
    }
    else
    {
        return id;
    }
}

void HashTableX::buildHashTableX(vector<Particle*> *part_list, double support_)
{
    support = support_;
    table_size = x_length / support + 1;  // at least one entry for the table.

    hash_table.clear();
    hash_table.resize(table_size);

    // insert the particles to the proper dynamic list.
    int num_part = part_list->size();
    for (int i=0; i<num_part; i++)
    {
        Particle* p_part = part_list->at(i);

        // Get the id of the cell where the particle is in
        int key = getHashKeyByXPosition(p_part->x_position);

        // Push to the corresponding dynamic list
        hash_table[key].push_back(p_part);
    }
}

void HashTableX::getNeighborsByXPosition(double x_pos, vector<Particle*> &neigh_list)
{
    neigh_list.clear();

    // The candisate entries should be range of (x_pos-support) to (x_pos+support).
    int key0 = getHashKeyByXPosition(x_pos - support);
    int key1 = getHashKeyByXPosition(x_pos + support);

    for (int key=key0; key<=key1; ++key)
    {
        vector<Particle*> *part_list = &hash_table[key];
        int num_part = part_list->size();
        for (int i=0; i<num_part; ++i)
        {
            Particle* p_part = part_list->at(i);

            double dist = p_part->x_position - x_pos;
            if (dist >= -support && dist <= support)
            {
                neigh_list.push_back(p_part);
            }
        }
    }

//    neigh_list.clear();

//    // Only three lists need to test.
//    // NOTE: the keys might be equal to each other and hence redundant.
//    int key[3];
//    key[0] = getHashKeyByXPosition(x_pos - support);
//    key[1] = getHashKeyByXPosition(x_pos);
//    key[2] = getHashKeyByXPosition(x_pos + support);

//    for (int k=0; k<3; ++k)
//    {
//        // Important! Must avoid the multiple visits to identical lists.
//        bool isNewList = true;
//        for (int j=0; j<k; ++j)
//        {
//            if (key[k] == key[j])
//            {
//                isNewList = false;
//                break;
//            }
//        }

//        if (isNewList)
//        {
//            vector<Particle*> *part_list = &hash_table[key[k]];

//            int num_part = part_list->size();
//            for (int i=0; i<num_part; ++i)
//            {
//                Particle* p_part = part_list->at(i);

//                double dist = p_part->x_position - x_pos;
//                if (dist >= -support && dist <= support)
//                {
//                    neigh_list.push_back(p_part);
//                }
//            }
//        }
//    }
}

void HashTableX::getNeighborsHalfRegionByXPosition(double x_pos, vector<Particle*> &neigh_list)
{
    neigh_list.clear();

    // The candisate entries should be range of x_pos to (x_pos+support).
    int key0 = getHashKeyByXPosition(x_pos);
    int key1 = getHashKeyByXPosition(x_pos + support);

    for (int key=key0; key<=key1; ++key)
    {
        vector<Particle*> *part_list = &hash_table[key];
        int num_part = part_list->size();
        for (int i=0; i<num_part; ++i)
        {
            Particle* p_part = part_list->at(i);

            double dist = p_part->x_position - x_pos;
            if (dist >= 0 && dist <= support)
            {
                neigh_list.push_back(p_part);
            }
        }
    }

//    neigh_list.clear();

//    // Only two lists need to test.
//    // NOTE: the keys might be equal to each other and hence redundant.
//    int key[2];
//    key[0] = getHashKeyByXPosition(x_pos);
//    key[1] = getHashKeyByXPosition(x_pos + support);

//    for (int k=0; k<2; ++k)
//    {
//        // Important! Must avoid multiple visits to identical lists.
//        bool isNewList = true;
//        for (int j=0; j<k; ++j)
//        {
//            if (key[k] == key[j])
//            {
//                isNewList = false;
//                break;
//            }
//        }

//        if (isNewList)
//        {
//            vector<Particle*> *part_list = &hash_table[key[k]];

//            int num_part = part_list->size();
//            for (int i=0; i<num_part; ++i)
//            {
//                Particle* p_part = part_list->at(i);

//                double dist = p_part->x_position - x_pos;
//                if (dist >= 0 && dist <= support)
//                {
//                    neigh_list.push_back(p_part);
//                }
//            }
//        }
//    }
}

void HashTableX::getTwoNeighborListsByXPosition(double x_pos,
                                                vector<Particle*> &neigh_list,
                                                vector<Particle*> &halfneigh_list)
{
    neigh_list.clear();
    halfneigh_list.clear();

    // The candisate entries should be range of (x_pos-support) to (x_pos+support).
    int key0 = getHashKeyByXPosition(x_pos - support);
    int key1 = getHashKeyByXPosition(x_pos + support);

    for (int key=key0; key<=key1; ++key)
    {
        vector<Particle*> *part_list = &hash_table[key];
        int num_part = part_list->size();
        for (int i=0; i<num_part; ++i)
        {
            Particle* p_part = part_list->at(i);

            double dist = p_part->x_position - x_pos;
            if (dist >= -support && dist <= support)
            {
                neigh_list.push_back(p_part);

                if (dist >= 0)
                {
                    halfneigh_list.push_back(p_part);
                }
            }
        }
    }
}

