#ifndef ROADPROPERTY_H
#define ROADPROPERTY_H

#include <QDialog>
#include "Road.h"

namespace Ui {
class RoadProperty;
}

class RoadProperty : public QDialog
{
    Q_OBJECT

public:
    explicit RoadProperty(QWidget *parent = 0);
    ~RoadProperty();

    void display(Road *roadin_);

private slots:
    void on_buttonBox_accepted();

private:
    Ui::RoadProperty *ui;

    Road *roadin;
    double road_speed_limit;
    double road_inflow_rate;
    bool hasConstrain;
    bool hasSource;

    void saveRoadData();
};

#endif // ROADPROPERTY_H
