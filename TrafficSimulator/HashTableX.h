#ifndef HashTableX_H
#define HashTableX_H

#include "global.h"
#include "Particle.h"

class HashTableX
{
public:
    HashTableX(double length_, double support_);

    void buildHashTableX(vector<Particle*> *part_list, double support_);

    void getNeighborsByXPosition(double x_pos, vector<Particle*> &neigh_list);
    void getNeighborsHalfRegionByXPosition(double x_pos, vector<Particle*> &neigh_list);
    void getTwoNeighborListsByXPosition(double x_pos,
                                        vector<Particle*> &neigh_list,
                                        vector<Particle*> &halfneigh_list);

private:
    double support;
    double x_length;

    // The hash table is where the particles are inserted.
    // Each entry is a dynamic list.
    vector<vector<Particle*>> hash_table;
    int table_size;

    int getHashKeyByXPosition(double x_pos);
};

#endif
