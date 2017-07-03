/********************************************************************************
** Form generated from reading UI file 'particleproperty.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PARTICLEPROPERTY_H
#define UI_PARTICLEPROPERTY_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>

QT_BEGIN_NAMESPACE

class Ui_ParticleProperty
{
public:
    QDialogButtonBox *buttonBox;
    QLabel *label_Particle_ID;
    QLabel *label_Neighbor_Number;
    QLabel *label_Density;
    QLabel *label_Velocity;
    QCheckBox *checkBox_Constrain;
    QLabel *label_Viscosity_Force;
    QLabel *label_Pressure_Force;
    QLabel *label_External_Force;
    QLabel *label_Damping_Force;

    void setupUi(QDialog *ParticleProperty)
    {
        if (ParticleProperty->objectName().isEmpty())
            ParticleProperty->setObjectName(QStringLiteral("ParticleProperty"));
        ParticleProperty->resize(300, 400);
        buttonBox = new QDialogButtonBox(ParticleProperty);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(70, 360, 161, 32));
        QFont font;
        font.setPointSize(12);
        buttonBox->setFont(font);
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        label_Particle_ID = new QLabel(ParticleProperty);
        label_Particle_ID->setObjectName(QStringLiteral("label_Particle_ID"));
        label_Particle_ID->setGeometry(QRect(30, 10, 261, 31));
        QFont font1;
        font1.setPointSize(14);
        label_Particle_ID->setFont(font1);
        label_Neighbor_Number = new QLabel(ParticleProperty);
        label_Neighbor_Number->setObjectName(QStringLiteral("label_Neighbor_Number"));
        label_Neighbor_Number->setGeometry(QRect(30, 40, 261, 31));
        label_Neighbor_Number->setFont(font1);
        label_Density = new QLabel(ParticleProperty);
        label_Density->setObjectName(QStringLiteral("label_Density"));
        label_Density->setGeometry(QRect(30, 70, 261, 31));
        label_Density->setFont(font1);
        label_Velocity = new QLabel(ParticleProperty);
        label_Velocity->setObjectName(QStringLiteral("label_Velocity"));
        label_Velocity->setGeometry(QRect(30, 100, 261, 31));
        label_Velocity->setFont(font1);
        checkBox_Constrain = new QCheckBox(ParticleProperty);
        checkBox_Constrain->setObjectName(QStringLiteral("checkBox_Constrain"));
        checkBox_Constrain->setGeometry(QRect(30, 300, 151, 31));
        checkBox_Constrain->setFont(font1);
        label_Viscosity_Force = new QLabel(ParticleProperty);
        label_Viscosity_Force->setObjectName(QStringLiteral("label_Viscosity_Force"));
        label_Viscosity_Force->setGeometry(QRect(30, 170, 261, 31));
        label_Viscosity_Force->setFont(font1);
        label_Pressure_Force = new QLabel(ParticleProperty);
        label_Pressure_Force->setObjectName(QStringLiteral("label_Pressure_Force"));
        label_Pressure_Force->setGeometry(QRect(30, 140, 261, 31));
        label_Pressure_Force->setFont(font1);
        label_External_Force = new QLabel(ParticleProperty);
        label_External_Force->setObjectName(QStringLiteral("label_External_Force"));
        label_External_Force->setGeometry(QRect(30, 200, 261, 31));
        label_External_Force->setFont(font1);
        label_Damping_Force = new QLabel(ParticleProperty);
        label_Damping_Force->setObjectName(QStringLiteral("label_Damping_Force"));
        label_Damping_Force->setGeometry(QRect(30, 230, 261, 31));
        label_Damping_Force->setFont(font1);

        retranslateUi(ParticleProperty);
        QObject::connect(buttonBox, SIGNAL(accepted()), ParticleProperty, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), ParticleProperty, SLOT(reject()));

        QMetaObject::connectSlotsByName(ParticleProperty);
    } // setupUi

    void retranslateUi(QDialog *ParticleProperty)
    {
        ParticleProperty->setWindowTitle(QApplication::translate("ParticleProperty", "Dialog", 0));
        label_Particle_ID->setText(QApplication::translate("ParticleProperty", "Particle ID", 0));
        label_Neighbor_Number->setText(QApplication::translate("ParticleProperty", "Neighbor Number", 0));
        label_Density->setText(QApplication::translate("ParticleProperty", "Density", 0));
        label_Velocity->setText(QApplication::translate("ParticleProperty", "Velocity", 0));
        checkBox_Constrain->setText(QApplication::translate("ParticleProperty", "Constrain", 0));
        label_Viscosity_Force->setText(QApplication::translate("ParticleProperty", "Viscosity Force", 0));
        label_Pressure_Force->setText(QApplication::translate("ParticleProperty", "Pressure Force", 0));
        label_External_Force->setText(QApplication::translate("ParticleProperty", "External Force", 0));
        label_Damping_Force->setText(QApplication::translate("ParticleProperty", "Damping Force", 0));
    } // retranslateUi

};

namespace Ui {
    class ParticleProperty: public Ui_ParticleProperty {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PARTICLEPROPERTY_H
