#ifndef RANDOMER_H
#define RANDOMER_H

#include <random>

static std::default_random_engine generator_0;
static std::default_random_engine generator_1;
static std::uniform_real_distribution<double> distribution_0(0.0, 1.0);
static std::uniform_real_distribution<double> distribution_1(0.0, 1.0);

class Randomer
{
public:
    Randomer()
    {
        // Init the random number (0~1) generater;
//        p_distribution_0 = new std::uniform_real_distribution<double>(0.0, 1.0);
//        p_distribution_1 = new std::uniform_real_distribution<double>(0.0, 1.0);
    }

    // The function generates a random float number range (0.0 ~ 1.0),
    static double generateRandomFromZeroToOne()
    {
        return distribution_1(generator_1);
    }

    // The function generates a random index range (0 ~ largest-1),
    static int generateRandomId(int largest)
    {
        return distribution_1(generator_1) * largest;
    }

    // The function generates a random index,
    // Could return -1 in the probability of '1-probability'
    static int generateRandomId(int largest, double probability)
    {
        double random_rate = distribution_0(generator_0);
        if (random_rate <= probability)
        {
            return distribution_1(generator_1) * largest;
        }
        else
        {
            return -1;
        }
    }
};

#endif // RANDOMER_H
