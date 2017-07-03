#ifndef PARTICLESYSTEMTRAFFICTRJ_H
#define PARTICLESYSTEMTRAFFICTRJ_H

#include "ParticleSystemTraffic1D.h"

class ParticleSystemTrafficTrj : public ParticleSystemTraffic1D
{
public:
    ParticleSystemTrafficTrj(SystemParameter *model_parameter_);

    virtual void initSystem();
    virtual void step();
    virtual void reset();

private:
    double mapping_scaling;
    zVec2 mapping_translation;
    void buildTrajectoriesByTaxiFiles();

    void addTaxiNodeToACar(TaxiNode *p_taxinode);
    CarParticle* findCarByTaxiNode(TaxiNode* p_taxinode);
    void insertTaxiNode(CarParticle *p_car, TaxiNode *p_taxinode);
    void insertCar(CarParticle *p_car);

    zVec2 start_position;
    string start_timestamp;
    time_t reference_time;

    // convert timestamp to seconds from the reference time
    double convertTimestamp(const string &str);
};

#endif // PARTICLESYSTEMTRAFFICTRJ_H
