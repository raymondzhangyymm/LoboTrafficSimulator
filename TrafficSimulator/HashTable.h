#ifndef HashTable_H
#define HashTable_H

#include "global.h"
#include "Particle.h"

class HashTable
{
public:
    HashTable(double _cellsize);

    void buildHashTable(vector<Particle*> &part_list, double _cellsize);

    void getNearestNeighbors(zVec2 pos, vector<Particle*> &neigh_list);
    void getNearestNeighborsHalfRegion(zVec2 pos, zVec2 tg, vector<Particle*> &halfneigh_list);
    void getTwoNeighborListsByXPosition(zVec2 pos, zVec2 tg,
                                        vector<Particle*> &neigh_list,
                                        vector<Particle*> &halfneigh_list);

private:

    // The size of each cell.
    double cellsize;

    // The number of entries in the hash table.
    int hashTableSize;

    // The hash table where the particles are saved.
    // Each entry is a dynamic list.
    map<int, vector<Particle*>> hashTable;

    int getHashKey(zVec2 pos);
    int getHashKey(double pos_x, double pos_y);

    unsigned int getNextPrimeNumber(unsigned int number);
    bool isPrime(unsigned int number);
};

#endif
