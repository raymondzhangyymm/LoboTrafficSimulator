#ifndef PARTICLEPROPERTY_H
#define PARTICLEPROPERTY_H

#include <QDialog>
#include "Particle.h"

namespace Ui {
class ParticleProperty;
}

class ParticleProperty : public QDialog
{
    Q_OBJECT

public:
    explicit ParticleProperty(QWidget *parent = 0);
    ~ParticleProperty();

    void display(Particle *particlein_, int simulation_md);

private:
    Ui::ParticleProperty *ui;

    Particle *particlein;
};

#endif // PARTICLEPROPERTY_H
