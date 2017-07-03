#include "carproperty.h"
#include "ui_carproperty.h"

CarProperty::CarProperty(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CarProperty)
{
    ui->setupUi(this);

    carin = NULL;
}

CarProperty::~CarProperty()
{
    delete ui;
}

void CarProperty::display(CarParticle *carin_)
{
    if (carin_)
    {
        carin = carin_;
        ui->label_Car_ID->setText("Particle Id: " + QString::number(carin->id));
        ui->label_Density->setText("Density: " + QString::number(carin->density));
        ui->label_Velocity->setText("Velocity: " + QString::number(carin->velocity.length()));
//        ui->checkBox_Constrain->setChecked(roadin->isConstrained);

        if (!isVisible())
        {
            setWindowFlags(Qt::WindowStaysOnTopHint);
            show();
        }
    }
}
