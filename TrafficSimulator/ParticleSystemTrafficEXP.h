#ifndef PARTICLESYSTEMTRAFFICEXP_H
#define PARTICLESYSTEMTRAFFICEXP_H

#include "ParticleSystemTraffic.h"

class ParticleSystemTrafficEXP : public ParticleSystemTraffic
{
public:
    ParticleSystemTrafficEXP(SystemParameter *model_parameter_);

    virtual void initSystem();
    virtual void step();
    virtual void updateParticles();

    void setSystemParameters();
};

#endif // PARTICLESYSTEMTRAFFICEXP_H
