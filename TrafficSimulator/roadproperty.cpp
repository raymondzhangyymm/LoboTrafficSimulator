#include "roadproperty.h"
#include "ui_roadproperty.h"

RoadProperty::RoadProperty(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RoadProperty)
{
    ui->setupUi(this);

    roadin = NULL;
}

RoadProperty::~RoadProperty()
{
    delete ui;
}

void RoadProperty::display(Road *roadin_)
{
    if (roadin_)
    {
        roadin = roadin_;
        ui->label_Road_ID->setText("Road Id: " + QString::number(roadin->id));
        ui->label_Road_Length->setText("Road Length: " + QString::number(roadin->road_length));
        ui->label_Road_Type->setText("Road Type: " + QString::number(roadin->roadsubtype));
        ui->label_Lane_Number->setText("Lane Number: " + QString::number(roadin->num_lanes));
        ui->doubleSpinBox_Speed_Limit->setValue(roadin->speed_limit);

        ui->doubleSpinBox_Inflow_Rate->setValue(roadin->inflow_rate[0]);
        ui->checkBox_Source->setChecked(roadin->isInflow[0]);

        ui->doubleSpinBox_Speed_Constrain->setValue(roadin->speed_constrain);
        ui->checkBox_Constrain->setChecked(roadin->isConstrained);

        if (!isVisible())
        {
            setWindowFlags(Qt::WindowStaysOnTopHint);
            show();
        }
    }
}

void RoadProperty::saveRoadData()
{
    if (roadin)
    {
        roadin->speed_limit = ui->doubleSpinBox_Speed_Limit->value();

        roadin->inflow_rate[0] = ui->doubleSpinBox_Inflow_Rate->value();
        roadin->isInflow[0] = ui->checkBox_Source->isChecked();

        roadin->speed_constrain = ui->doubleSpinBox_Speed_Constrain->value();
        roadin->isConstrained = ui->checkBox_Constrain->isChecked();
    }
}

void RoadProperty::on_buttonBox_accepted()
{
    saveRoadData();
}
