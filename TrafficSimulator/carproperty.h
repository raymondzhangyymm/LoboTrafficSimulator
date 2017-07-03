#ifndef CARPROPERTY_H
#define CARPROPERTY_H

#include <QDialog>
#include "CarParticle.h"

namespace Ui {
class CarProperty;
}

class CarProperty : public QDialog
{
    Q_OBJECT

public:
    explicit CarProperty(QWidget *parent = 0);
    ~CarProperty();

    void display(CarParticle *carin_);

private:
    Ui::CarProperty *ui;

    CarParticle *carin;
};

#endif // CARPROPERTY_H
