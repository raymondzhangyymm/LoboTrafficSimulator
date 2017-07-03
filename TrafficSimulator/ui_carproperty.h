/********************************************************************************
** Form generated from reading UI file 'carproperty.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CARPROPERTY_H
#define UI_CARPROPERTY_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>

QT_BEGIN_NAMESPACE

class Ui_CarProperty
{
public:
    QDialogButtonBox *buttonBox;
    QLabel *label_Car_ID;
    QLabel *label_Density;
    QLabel *label_Velocity;

    void setupUi(QDialog *CarProperty)
    {
        if (CarProperty->objectName().isEmpty())
            CarProperty->setObjectName(QStringLiteral("CarProperty"));
        CarProperty->resize(300, 400);
        buttonBox = new QDialogButtonBox(CarProperty);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(70, 360, 161, 32));
        QFont font;
        font.setPointSize(12);
        buttonBox->setFont(font);
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        label_Car_ID = new QLabel(CarProperty);
        label_Car_ID->setObjectName(QStringLiteral("label_Car_ID"));
        label_Car_ID->setGeometry(QRect(30, 10, 261, 31));
        QFont font1;
        font1.setPointSize(14);
        label_Car_ID->setFont(font1);
        label_Density = new QLabel(CarProperty);
        label_Density->setObjectName(QStringLiteral("label_Density"));
        label_Density->setGeometry(QRect(30, 70, 261, 31));
        label_Density->setFont(font1);
        label_Velocity = new QLabel(CarProperty);
        label_Velocity->setObjectName(QStringLiteral("label_Velocity"));
        label_Velocity->setGeometry(QRect(30, 110, 261, 31));
        label_Velocity->setFont(font1);

        retranslateUi(CarProperty);
        QObject::connect(buttonBox, SIGNAL(accepted()), CarProperty, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), CarProperty, SLOT(reject()));

        QMetaObject::connectSlotsByName(CarProperty);
    } // setupUi

    void retranslateUi(QDialog *CarProperty)
    {
        CarProperty->setWindowTitle(QApplication::translate("CarProperty", "Dialog", 0));
        label_Car_ID->setText(QApplication::translate("CarProperty", "Car ID", 0));
        label_Density->setText(QApplication::translate("CarProperty", "Density", 0));
        label_Velocity->setText(QApplication::translate("CarProperty", "Velocity", 0));
    } // retranslateUi

};

namespace Ui {
    class CarProperty: public Ui_CarProperty {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CARPROPERTY_H
