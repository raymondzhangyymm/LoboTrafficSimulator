/********************************************************************************
** Form generated from reading UI file 'roadproperty.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ROADPROPERTY_H
#define UI_ROADPROPERTY_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>

QT_BEGIN_NAMESPACE

class Ui_RoadProperty
{
public:
    QDialogButtonBox *buttonBox;
    QLabel *label_Road_ID;
    QLabel *label_Lane_Number;
    QLabel *label_Speed_Limit;
    QLabel *label_Inflow_Rate;
    QLabel *label_Road_Type;
    QDoubleSpinBox *doubleSpinBox_Speed_Limit;
    QDoubleSpinBox *doubleSpinBox_Inflow_Rate;
    QCheckBox *checkBox_Constrain;
    QCheckBox *checkBox_Source;
    QLabel *label_Road_Length;
    QLabel *label_speed_constrain;
    QDoubleSpinBox *doubleSpinBox_Speed_Constrain;

    void setupUi(QDialog *RoadProperty)
    {
        if (RoadProperty->objectName().isEmpty())
            RoadProperty->setObjectName(QStringLiteral("RoadProperty"));
        RoadProperty->resize(260, 400);
        buttonBox = new QDialogButtonBox(RoadProperty);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(50, 360, 161, 32));
        QFont font;
        font.setPointSize(12);
        buttonBox->setFont(font);
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        label_Road_ID = new QLabel(RoadProperty);
        label_Road_ID->setObjectName(QStringLiteral("label_Road_ID"));
        label_Road_ID->setGeometry(QRect(20, 10, 221, 31));
        QFont font1;
        font1.setPointSize(14);
        label_Road_ID->setFont(font1);
        label_Lane_Number = new QLabel(RoadProperty);
        label_Lane_Number->setObjectName(QStringLiteral("label_Lane_Number"));
        label_Lane_Number->setGeometry(QRect(20, 100, 221, 31));
        label_Lane_Number->setFont(font1);
        label_Speed_Limit = new QLabel(RoadProperty);
        label_Speed_Limit->setObjectName(QStringLiteral("label_Speed_Limit"));
        label_Speed_Limit->setGeometry(QRect(20, 140, 111, 31));
        label_Speed_Limit->setFont(font1);
        label_Inflow_Rate = new QLabel(RoadProperty);
        label_Inflow_Rate->setObjectName(QStringLiteral("label_Inflow_Rate"));
        label_Inflow_Rate->setGeometry(QRect(20, 220, 111, 31));
        label_Inflow_Rate->setFont(font1);
        label_Road_Type = new QLabel(RoadProperty);
        label_Road_Type->setObjectName(QStringLiteral("label_Road_Type"));
        label_Road_Type->setGeometry(QRect(20, 70, 221, 31));
        label_Road_Type->setFont(font1);
        doubleSpinBox_Speed_Limit = new QDoubleSpinBox(RoadProperty);
        doubleSpinBox_Speed_Limit->setObjectName(QStringLiteral("doubleSpinBox_Speed_Limit"));
        doubleSpinBox_Speed_Limit->setGeometry(QRect(160, 141, 81, 31));
        QFont font2;
        font2.setPointSize(12);
        font2.setBold(true);
        font2.setWeight(75);
        doubleSpinBox_Speed_Limit->setFont(font2);
        doubleSpinBox_Inflow_Rate = new QDoubleSpinBox(RoadProperty);
        doubleSpinBox_Inflow_Rate->setObjectName(QStringLiteral("doubleSpinBox_Inflow_Rate"));
        doubleSpinBox_Inflow_Rate->setGeometry(QRect(160, 220, 81, 31));
        doubleSpinBox_Inflow_Rate->setFont(font2);
        checkBox_Constrain = new QCheckBox(RoadProperty);
        checkBox_Constrain->setObjectName(QStringLiteral("checkBox_Constrain"));
        checkBox_Constrain->setGeometry(QRect(130, 260, 16, 31));
        checkBox_Constrain->setFont(font1);
        checkBox_Source = new QCheckBox(RoadProperty);
        checkBox_Source->setObjectName(QStringLiteral("checkBox_Source"));
        checkBox_Source->setGeometry(QRect(130, 220, 16, 31));
        checkBox_Source->setFont(font1);
        label_Road_Length = new QLabel(RoadProperty);
        label_Road_Length->setObjectName(QStringLiteral("label_Road_Length"));
        label_Road_Length->setGeometry(QRect(20, 40, 221, 31));
        label_Road_Length->setFont(font1);
        label_speed_constrain = new QLabel(RoadProperty);
        label_speed_constrain->setObjectName(QStringLiteral("label_speed_constrain"));
        label_speed_constrain->setGeometry(QRect(20, 260, 81, 31));
        label_speed_constrain->setFont(font1);
        doubleSpinBox_Speed_Constrain = new QDoubleSpinBox(RoadProperty);
        doubleSpinBox_Speed_Constrain->setObjectName(QStringLiteral("doubleSpinBox_Speed_Constrain"));
        doubleSpinBox_Speed_Constrain->setGeometry(QRect(160, 260, 81, 31));
        doubleSpinBox_Speed_Constrain->setFont(font2);

        retranslateUi(RoadProperty);
        QObject::connect(buttonBox, SIGNAL(accepted()), RoadProperty, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), RoadProperty, SLOT(reject()));

        QMetaObject::connectSlotsByName(RoadProperty);
    } // setupUi

    void retranslateUi(QDialog *RoadProperty)
    {
        RoadProperty->setWindowTitle(QApplication::translate("RoadProperty", "Dialog", 0));
        label_Road_ID->setText(QApplication::translate("RoadProperty", "Road ID", 0));
        label_Lane_Number->setText(QApplication::translate("RoadProperty", "Lane Number", 0));
        label_Speed_Limit->setText(QApplication::translate("RoadProperty", "Speed Limit", 0));
        label_Inflow_Rate->setText(QApplication::translate("RoadProperty", "Inflow", 0));
        label_Road_Type->setText(QApplication::translate("RoadProperty", "Road Type", 0));
        checkBox_Constrain->setText(QApplication::translate("RoadProperty", "Constrain", 0));
        checkBox_Source->setText(QApplication::translate("RoadProperty", "Source", 0));
        label_Road_Length->setText(QApplication::translate("RoadProperty", "Road Length", 0));
        label_speed_constrain->setText(QApplication::translate("RoadProperty", "Constrain", 0));
    } // retranslateUi

};

namespace Ui {
    class RoadProperty: public Ui_RoadProperty {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ROADPROPERTY_H
