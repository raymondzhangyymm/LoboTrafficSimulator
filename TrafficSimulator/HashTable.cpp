#include "HashTable.h"

// Constructor
HashTable::HashTable(double _support_radius)
{
    cellsize = _support_radius;
}

// It returns the next prime number after the input "number".
unsigned int HashTable::getNextPrimeNumber(unsigned int number)
{
    // Making the input number be uneven.
    if (number % 2 == 0)
    {
        number++;
    }

    // Set the limit
    unsigned int limit = number + 1000000;

    // Finding the next prime number.
    for (; number <= limit; number += 2 )
    {
        if (isPrime(number))
        {            
            return number;
        }
    }

    // No prime is found, error.
    return 0;
}

// Check if the input "number" is a prime.
bool HashTable::isPrime(unsigned int number)
{
    // less meaning for 1, 2 or 3.
    if (number <= 3)
    {
        return true;
    }

    // Return false if the number is even.
    if (number % 2 == 0)
    {
        return false;
    }

    // Set the max denominator and check if the number is a prime.
    unsigned int max_denominator = sqrt(number);
    for (unsigned int d=3; d <= max_denominator; d += 2)
    {
        if (number % d == 0)
        {
            return false;
        }
    }

    return true;
}

// Compute the hash key for the input "position".
int HashTable::getHashKey(zVec2 pos)
{
    long long x = floor(pos.x / cellsize) * HASH_FUNCTION_PRIME_1;
    long long y = floor(pos.y / cellsize) * HASH_FUNCTION_PRIME_2;
    
    return int((x ^ y) % hashTableSize);
}
int HashTable::getHashKey(double pos_x, double pos_y)
{
    long long x = floor(pos_x / cellsize) * HASH_FUNCTION_PRIME_1;
    long long y = floor(pos_y / cellsize) * HASH_FUNCTION_PRIME_2;

    return int((x ^ y) % hashTableSize);
}

// Build a hash table and insert all particles.
void HashTable::buildHashTable(vector<Particle*> &part_list, double _support)
{
    cellsize = _support;

    int num_part = part_list.size();
    hashTableSize = getNextPrimeNumber(2 * num_part);

    // clear the old table and build a new table.
    hashTable.clear();
    for (int i=0; i<num_part; i++)
    {
        Particle* p_part = part_list[i];

        // Get particle's hash key
        int particleKey = getHashKey(p_part->position);

        // Push to the corresponding dynamic list
        hashTable[particleKey].push_back(p_part);
    }
}

// Find the nearest neighbours of the particle
void HashTable::getNearestNeighbors(zVec2 pos, vector<Particle*> &neigh_list)
{
    neigh_list.clear();

    double pos_x = pos.x, pos_y = pos.y;
    double h2 = cellsize * cellsize;

    // Compute the bounding box
    double min_x = pos_x - cellsize;
    double min_y = pos_y - cellsize;
    double max_x = pos_x + cellsize;
    double max_y = pos_y + cellsize;

    // Only nine lists need to test. Might contain identical keys.
    int key[9];
    key[0] = getHashKey(min_x, min_y);
    key[1] = getHashKey(min_x, pos_y);
    key[2] = getHashKey(min_x, max_y);
    key[3] = getHashKey(pos_x, min_y);
    key[4] = getHashKey(pos_x, pos_y);
    key[5] = getHashKey(pos_x, max_y);
    key[6] = getHashKey(max_x, min_y);
    key[7] = getHashKey(max_x, pos_y);
    key[8] = getHashKey(max_x, max_y);

    for (int k=0; k<9; ++k)
    {
        // Important! Must avoid possible hash collisions with identical keys.
        bool isNewList = true;
        for (int j=0; j<k; ++j)
        {
            if (key[k] == key[j])
            {
                isNewList = false;
                break;
            }
        }

        if (isNewList)
        {
            vector<Particle*> part_list = hashTable[key[k]];

            int num_part = part_list.size();
            for (int i=0; i<num_part; ++i)
            {
                Particle* p_part = part_list[i];

                double lx = pos_x - p_part->position.x;
                double ly = pos_y - p_part->position.y;
                if (lx*lx + ly*ly <= h2)
                {
                    neigh_list.push_back(p_part);
                }
            }
        }
    }
}

// Find the nearest neighbours of the particle only in the half region along tangent
void HashTable::getNearestNeighborsHalfRegion(zVec2 pos, zVec2 tg, vector<Particle*> &halfneigh_list)
{
    halfneigh_list.clear();

    double pos_x = pos.x, pos_y = pos.y;
    double h2 = cellsize * cellsize;

    // Compute the bounding box
    double min_x = pos_x - cellsize;
    double min_y = pos_y - cellsize;
    double max_x = pos_x + cellsize;
    double max_y = pos_y + cellsize;

    // Only nine lists need to test. Might contain identical keys.
    int key[9];
    key[0] = getHashKey(min_x, min_y);
    key[1] = getHashKey(min_x, pos_y);
    key[2] = getHashKey(min_x, max_y);
    key[3] = getHashKey(pos_x, min_y);
    key[4] = getHashKey(pos_x, pos_y);
    key[5] = getHashKey(pos_x, max_y);
    key[6] = getHashKey(max_x, min_y);
    key[7] = getHashKey(max_x, pos_y);
    key[8] = getHashKey(max_x, max_y);

    for (int k=0; k<9; ++k)
    {
        // Important! Must avoid possible hash collisions with identical keys.
        bool isNewList = true;
        for (int j=0; j<k; ++j)
        {
            if (key[k] == key[j])
            {
                isNewList = false;
                break;
            }
        }

        if (isNewList)
        {
            vector<Particle*> part_list = hashTable[key[k]];

            int num_part = part_list.size();
            for (int i=0; i<num_part; ++i)
            {
                Particle* p_part = part_list[i];

                double lx = p_part->position.x - pos_x;
                double ly = p_part->position.y - pos_y;
                if ((lx*lx + ly*ly <= h2) && (lx*tg.x + ly*tg.y >= 0.0))
                {
                    halfneigh_list.push_back(p_part);
                }
            }
        }
    }
}

void HashTable::getTwoNeighborListsByXPosition(zVec2 pos, zVec2 tg,
                                               vector<Particle*> &neigh_list,
                                               vector<Particle*> &halfneigh_list)
{
    neigh_list.clear();
    halfneigh_list.clear();

    double pos_x = pos.x, pos_y = pos.y;
    double h2 = cellsize * cellsize;

    // Compute the bounding box
    double min_x = pos_x - cellsize;
    double min_y = pos_y - cellsize;
    double max_x = pos_x + cellsize;
    double max_y = pos_y + cellsize;

    // Only nine lists need to test. Might contain identical keys.
    int key[9];
    key[0] = getHashKey(min_x, min_y);
    key[1] = getHashKey(min_x, pos_y);
    key[2] = getHashKey(min_x, max_y);
    key[3] = getHashKey(pos_x, min_y);
    key[4] = getHashKey(pos_x, pos_y);
    key[5] = getHashKey(pos_x, max_y);
    key[6] = getHashKey(max_x, min_y);
    key[7] = getHashKey(max_x, pos_y);
    key[8] = getHashKey(max_x, max_y);

    for (int k=0; k<9; ++k)
    {
        // Important! Must avoid possible hash collisions with identical keys.
        bool isNewList = true;
        for (int j=0; j<k; ++j)
        {
            if (key[k] == key[j])
            {
                isNewList = false;
                break;
            }
        }

        if (isNewList)
        {
            vector<Particle*> part_list = hashTable[key[k]];

            int num_part = part_list.size();
            for (int i=0; i<num_part; ++i)
            {
                Particle* p_part = part_list[i];

                double lx = p_part->position.x - pos_x;
                double ly = p_part->position.y - pos_y;
                if (lx*lx + ly*ly <= h2)
                {
                    neigh_list.push_back(p_part);
                    if (lx*tg.x + ly*tg.y >= 0.0)
                    {
                        halfneigh_list.push_back(p_part);
                    }
                }
            }
        }
    }
}
