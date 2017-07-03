#ifndef PARTICLESYSTEMTRAFFIC1D_H
#define PARTICLESYSTEMTRAFFIC1D_H

#include "ParticleSystemTraffic.h"

class ParticleSystemTraffic1D : public ParticleSystemTraffic
{
public:
    ParticleSystemTraffic1D(SystemParameter *model_parameter_);

    virtual void initSystem();
    virtual void step();

    virtual void createObservationPoint();
    virtual void moveObservationPoint(Node *p_node, zVec2 pos);
    virtual void updateObservation();
};

#endif // PARTICLESYSTEMTRAFFIC1D_H
