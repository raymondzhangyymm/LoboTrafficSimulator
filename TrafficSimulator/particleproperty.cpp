#include "particleproperty.h"
#include "ui_particleproperty.h"

ParticleProperty::ParticleProperty(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ParticleProperty)
{
    ui->setupUi(this);

    particlein = NULL;
}

ParticleProperty::~ParticleProperty()
{
    delete ui;
}

void ParticleProperty::display(Particle *particlein_, int simulation_md)
{
    if (particlein_)
    {
        particlein = particlein_;

        if (simulation_md == 1)
        {
            // 1D particle
            ui->label_Particle_ID->setText("Particle Id: " + QString::number(particlein->id));
            ui->label_Neighbor_Number->setText("Neighbor Number: " + QString::number(particlein->neighbors.size()));
            ui->label_Density->setText("Density: " + QString::number(particlein->density));
            ui->label_Velocity->setText("Velocity: " + QString::number(particlein->x_velocity));

            ui->label_Pressure_Force->setText("Pressure Force: "
                                              + QString::number(particlein->x_pressureForce));
            ui->label_Viscosity_Force->setText("Viscosity Force: "
                                               + QString::number(particlein->x_viscosityForce));
            ui->label_External_Force->setText("External Force: "
                                              + QString::number(particlein->x_externalForce));
            ui->label_Damping_Force->setText("Damping Force: "
                                             + QString::number(particlein->x_dampingForce));
        }
        else if (simulation_md == 2)
        {
            // 2D particle
            ui->label_Particle_ID->setText("Particle Id: " + QString::number(particlein->id));
            ui->label_Neighbor_Number->setText("Neighbor Number: " + QString::number(particlein->neighbors.size()));
            ui->label_Density->setText("Density: " + QString::number(particlein->density));
            ui->label_Velocity->setText("Velocity: " + QString::number(particlein->velocity.length()));
            //        ui->checkBox_Constrain->setChecked(roadin->isConstrained);

            ui->label_Pressure_Force->setText("Pressure Force: "
                                              + QString::number(particlein->pressureForce.length()));
            ui->label_Viscosity_Force->setText("Viscosity Force: "
                                               + QString::number(particlein->viscosityForce.length()));
            ui->label_External_Force->setText("External Force: "
                                              + QString::number(particlein->externalForce.length()));
            ui->label_Damping_Force->setText("Damping Force: "
                                             + QString::number(particlein->dampingForce.length()));
        }

        if (!isVisible())
        {
            setWindowFlags(Qt::WindowStaysOnTopHint);
            show();
        }
    }
}
