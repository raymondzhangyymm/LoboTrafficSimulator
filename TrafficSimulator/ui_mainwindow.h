/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionVisual_Velocity_and_Density;
    QAction *actionTraffic_Config;
    QWidget *centralwidget;
    QGridLayout *gridLayout_9;
    QVBoxLayout *verticalLayout_main;
    QDockWidget *buttons;
    QWidget *dockWidgetContents;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButtonLoad;
    QPushButton *pushButtonOutput_Configuration;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButtonStartPause;
    QPushButton *pushButtonStep;
    QPushButton *pushButtonReset;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *pushButtonSnapshot;
    QPushButton *pushButtonSnapshot_LoadCamera;
    QPushButton *pushButtonSnapshot_SaveCamera;
    QSpacerItem *horizontalSpacer_3;
    QDockWidget *control_panel;
    QWidget *dockWidgetContents_2;
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *verticalLayout_6;
    QTabWidget *tabWidget;
    QWidget *tab;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout_density_velocity;
    QSpacerItem *verticalSpacer_2;
    QSpacerItem *verticalSpacer;
    QWidget *tab_3;
    QWidget *gridLayoutWidget_6;
    QGridLayout *gridLayout_6;
    QPushButton *pushButton_Observation_Cancel;
    QPushButton *pushButton_Observation_Delete;
    QPushButton *pushButton_Observation_Add;
    QPushButton *pushButton_Observation_Show;
    QWidget *verticalLayoutWidget_7;
    QVBoxLayout *verticalLayout_local_monitors;
    QWidget *tab_1;
    QLabel *label_10;
    QWidget *gridLayoutWidget_4;
    QGridLayout *gridLayout_4;
    QCheckBox *Pressureforce;
    QCheckBox *Viscosityforce;
    QCheckBox *Externalforce;
    QWidget *gridLayoutWidget_2;
    QGridLayout *gridLayout_2;
    QLabel *label_2;
    QLabel *label;
    QDoubleSpinBox *doubleSpinBox_Viscosity;
    QLabel *label_6;
    QDoubleSpinBox *doubleSpinBox_GasStiffness;
    QLabel *label_4;
    QDoubleSpinBox *doubleSpinBox_CoeffFieldForce;
    QDoubleSpinBox *doubleSpinBox_CoeffDampingForce;
    QLabel *label_15;
    QLabel *label_13;
    QWidget *gridLayoutWidget_5;
    QGridLayout *gridLayout_5;
    QPushButton *pushButton_Beziers_Show;
    QPushButton *pushButton_Beziers_Cancel;
    QPushButton *pushButton_Beziers_Delete;
    QPushButton *pushButton_Beziers_Add;
    QLabel *label_12;
    QCheckBox *checkBox_AddConstrain;
    QWidget *gridLayoutWidget_3;
    QGridLayout *gridLayout_3;
    QPushButton *pushButton_Blocks_Cancel;
    QPushButton *pushButton_Blocks_Delete;
    QPushButton *pushButton_Blocks_Add;
    QPushButton *pushButton_Blocks_Show;
    QWidget *verticalLayoutWidget_6;
    QVBoxLayout *verticalLayout_kernel;
    QWidget *gridLayoutWidget;
    QGridLayout *gridLayout;
    QRadioButton *radioButton_kernel_poly6;
    QRadioButton *radioButton_kernel_poly6_half;
    QRadioButton *radioButton_kernel_viscosity;
    QRadioButton *radioButton_kernel_spiky_half;
    QRadioButton *radioButton_kernel_spiky;
    QRadioButton *radioButton_kernel_viscosity_half;
    QLabel *label_8;
    QDoubleSpinBox *doubleSpinBox_supportRadius;
    QWidget *tab_2;
    QLabel *label_20;
    QWidget *verticalLayoutWidget_3;
    QVBoxLayout *verticalLayout;
    QRadioButton *radioButton_ShowMainDensity;
    QRadioButton *radioButton_ShowMainVelocity;
    QRadioButton *radioButton_ShowMainDensityVelocity_Nothing;
    QWidget *gridLayoutWidget_7;
    QGridLayout *gridLayout_7;
    QCheckBox *checkBox_ShowCells;
    QCheckBox *checkBox_ShowParticle_Velocity_Direction;
    QCheckBox *checkBox_ShowParticle_Force;
    QCheckBox *checkBox_ShowParticle_NormalTangent;
    QCheckBox *checkBox_ShowReferencePlanes;
    QCheckBox *checkBox_ShowRoad_Facilities;
    QCheckBox *checkBox_ShowVehicle_Force;
    QCheckBox *checkBox_ShowVehicle_Velocity_Direction;
    QCheckBox *checkBox_ShowVehicle_Tail;
    QCheckBox *checkBox_ShowVehicle_Trajectory;
    QWidget *verticalLayoutWidget_4;
    QVBoxLayout *verticalLayout_2;
    QCheckBox *checkBox_SensorData;
    QCheckBox *checkBox_EntireDensityVelocity;
    QCheckBox *checkBox_Screenshot_Image;
    QLabel *label_9;
    QWidget *gridLayoutWidget_8;
    QGridLayout *gridLayout_8;
    QCheckBox *checkBox_RoadNet_Service;
    QCheckBox *checkBox_RoadNet_Motorway;
    QCheckBox *checkBox_RoadNet_Residential;
    QCheckBox *checkBox_RoadNet_Trunk;
    QCheckBox *checkBox_RoadNet_Secondary;
    QCheckBox *checkBox_RoadNet_Primary;
    QCheckBox *checkBox_RoadNet_Tertiary;
    QCheckBox *checkBox_RoadNet_Oneway;
    QCheckBox *checkBox_RoadNet_Others;
    QCheckBox *checkBox_RoadNet_Footway;
    QCheckBox *checkBox_RoadNet_Unclassified;
    QCheckBox *checkBox_RoadNet_Path;
    QLabel *label_21;
    QWidget *gridLayoutWidget_9;
    QGridLayout *gridLayout_10;
    QPushButton *pushButton_Selection_Spline;
    QPushButton *pushButton_Selection_Observation;
    QPushButton *pushButton_Selection_Block;
    QPushButton *pushButton_Selection_Particle;
    QPushButton *pushButton_Selection_Road;
    QWidget *gridLayoutWidget_10;
    QGridLayout *gridLayout_11;
    QRadioButton *radioButton_ShowParticlesVehicles_Nothing;
    QRadioButton *radioButton_ShowParticles;
    QRadioButton *radioButton_ShowVehicles;
    QRadioButton *radioButton_ShowVehicles_Bbox;
    QLabel *label_22;
    QLabel *label_23;
    QWidget *gridLayoutWidget_11;
    QGridLayout *gridLayout_12;
    QCheckBox *checkBox_MapElements_Others;
    QCheckBox *checkBox_MapElements_Buildings;
    QCheckBox *checkBox_MapElements_Railways;
    QCheckBox *checkBox_MapElements_Highways;
    QCheckBox *checkBox_MapElements_BoundingBox;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->setEnabled(true);
        MainWindow->resize(1496, 876);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(200);
        sizePolicy.setVerticalStretch(200);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        MainWindow->setMinimumSize(QSize(100, 100));
        MainWindow->setMaximumSize(QSize(10000, 10000));
        QFont font;
        font.setPointSize(8);
        MainWindow->setFont(font);
        QIcon icon;
        icon.addFile(QStringLiteral("Icon/cemagraphics_minicar.png"), QSize(), QIcon::Normal, QIcon::Off);
        MainWindow->setWindowIcon(icon);
        MainWindow->setIconSize(QSize(32, 32));
        actionVisual_Velocity_and_Density = new QAction(MainWindow);
        actionVisual_Velocity_and_Density->setObjectName(QStringLiteral("actionVisual_Velocity_and_Density"));
        actionTraffic_Config = new QAction(MainWindow);
        actionTraffic_Config->setObjectName(QStringLiteral("actionTraffic_Config"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        gridLayout_9 = new QGridLayout(centralwidget);
        gridLayout_9->setSpacing(0);
        gridLayout_9->setObjectName(QStringLiteral("gridLayout_9"));
        gridLayout_9->setContentsMargins(2, 2, 0, 0);
        verticalLayout_main = new QVBoxLayout();
        verticalLayout_main->setSpacing(0);
        verticalLayout_main->setObjectName(QStringLiteral("verticalLayout_main"));
        verticalLayout_main->setSizeConstraint(QLayout::SetNoConstraint);
        verticalLayout_main->setContentsMargins(0, 0, -1, -1);

        gridLayout_9->addLayout(verticalLayout_main, 0, 0, 1, 1);

        MainWindow->setCentralWidget(centralwidget);
        buttons = new QDockWidget(MainWindow);
        buttons->setObjectName(QStringLiteral("buttons"));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(buttons->sizePolicy().hasHeightForWidth());
        buttons->setSizePolicy(sizePolicy1);
        buttons->setMinimumSize(QSize(878, 70));
        buttons->setMaximumSize(QSize(524287, 85));
        buttons->setFeatures(QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
        buttons->setAllowedAreas(Qt::BottomDockWidgetArea|Qt::TopDockWidgetArea);
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QStringLiteral("dockWidgetContents"));
        horizontalLayout = new QHBoxLayout(dockWidgetContents);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        pushButtonLoad = new QPushButton(dockWidgetContents);
        pushButtonLoad->setObjectName(QStringLiteral("pushButtonLoad"));
        QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(pushButtonLoad->sizePolicy().hasHeightForWidth());
        pushButtonLoad->setSizePolicy(sizePolicy2);
        pushButtonLoad->setMinimumSize(QSize(100, 30));
        QFont font1;
        font1.setPointSize(12);
        pushButtonLoad->setFont(font1);
        QIcon icon1;
        icon1.addFile(QStringLiteral("Icon/folder_drag_accept.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButtonLoad->setIcon(icon1);
        pushButtonLoad->setIconSize(QSize(30, 50));

        horizontalLayout->addWidget(pushButtonLoad);

        pushButtonOutput_Configuration = new QPushButton(dockWidgetContents);
        pushButtonOutput_Configuration->setObjectName(QStringLiteral("pushButtonOutput_Configuration"));
        sizePolicy2.setHeightForWidth(pushButtonOutput_Configuration->sizePolicy().hasHeightForWidth());
        pushButtonOutput_Configuration->setSizePolicy(sizePolicy2);
        pushButtonOutput_Configuration->setMinimumSize(QSize(100, 30));
        pushButtonOutput_Configuration->setFont(font1);
        QIcon icon2;
        icon2.addFile(QStringLiteral("Icon/disks.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButtonOutput_Configuration->setIcon(icon2);
        pushButtonOutput_Configuration->setIconSize(QSize(30, 50));

        horizontalLayout->addWidget(pushButtonOutput_Configuration);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        pushButtonStartPause = new QPushButton(dockWidgetContents);
        pushButtonStartPause->setObjectName(QStringLiteral("pushButtonStartPause"));
        sizePolicy2.setHeightForWidth(pushButtonStartPause->sizePolicy().hasHeightForWidth());
        pushButtonStartPause->setSizePolicy(sizePolicy2);
        pushButtonStartPause->setMinimumSize(QSize(100, 30));
        pushButtonStartPause->setFont(font1);
        QIcon icon3;
        icon3.addFile(QStringLiteral("Icon/media_playback_start.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButtonStartPause->setIcon(icon3);
        pushButtonStartPause->setIconSize(QSize(30, 50));

        horizontalLayout->addWidget(pushButtonStartPause);

        pushButtonStep = new QPushButton(dockWidgetContents);
        pushButtonStep->setObjectName(QStringLiteral("pushButtonStep"));
        pushButtonStep->setMinimumSize(QSize(100, 30));
        pushButtonStep->setFont(font1);
        QIcon icon4;
        icon4.addFile(QStringLiteral("Icon/old_go_next.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButtonStep->setIcon(icon4);
        pushButtonStep->setIconSize(QSize(30, 50));

        horizontalLayout->addWidget(pushButtonStep);

        pushButtonReset = new QPushButton(dockWidgetContents);
        pushButtonReset->setObjectName(QStringLiteral("pushButtonReset"));
        pushButtonReset->setMinimumSize(QSize(100, 30));
        pushButtonReset->setFont(font1);
        QIcon icon5;
        icon5.addFile(QStringLiteral("Icon/old_edit_undo.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButtonReset->setIcon(icon5);
        pushButtonReset->setIconSize(QSize(30, 50));

        horizontalLayout->addWidget(pushButtonReset);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);

        pushButtonSnapshot = new QPushButton(dockWidgetContents);
        pushButtonSnapshot->setObjectName(QStringLiteral("pushButtonSnapshot"));
        pushButtonSnapshot->setMinimumSize(QSize(100, 30));
        pushButtonSnapshot->setFont(font1);
        QIcon icon6;
        icon6.addFile(QStringLiteral("Icon/start_here_gnome_orange.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButtonSnapshot->setIcon(icon6);
        pushButtonSnapshot->setIconSize(QSize(30, 50));

        horizontalLayout->addWidget(pushButtonSnapshot);

        pushButtonSnapshot_LoadCamera = new QPushButton(dockWidgetContents);
        pushButtonSnapshot_LoadCamera->setObjectName(QStringLiteral("pushButtonSnapshot_LoadCamera"));
        pushButtonSnapshot_LoadCamera->setMinimumSize(QSize(100, 30));
        pushButtonSnapshot_LoadCamera->setFont(font1);
        QIcon icon7;
        icon7.addFile(QStringLiteral("Icon/1488352350_Upload.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButtonSnapshot_LoadCamera->setIcon(icon7);
        pushButtonSnapshot_LoadCamera->setIconSize(QSize(50, 50));

        horizontalLayout->addWidget(pushButtonSnapshot_LoadCamera);

        pushButtonSnapshot_SaveCamera = new QPushButton(dockWidgetContents);
        pushButtonSnapshot_SaveCamera->setObjectName(QStringLiteral("pushButtonSnapshot_SaveCamera"));
        pushButtonSnapshot_SaveCamera->setMinimumSize(QSize(100, 30));
        pushButtonSnapshot_SaveCamera->setFont(font1);
        QIcon icon8;
        icon8.addFile(QStringLiteral("Icon/1488352357_Download.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButtonSnapshot_SaveCamera->setIcon(icon8);
        pushButtonSnapshot_SaveCamera->setIconSize(QSize(50, 50));

        horizontalLayout->addWidget(pushButtonSnapshot_SaveCamera);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_3);

        buttons->setWidget(dockWidgetContents);
        MainWindow->addDockWidget(static_cast<Qt::DockWidgetArea>(8), buttons);
        control_panel = new QDockWidget(MainWindow);
        control_panel->setObjectName(QStringLiteral("control_panel"));
        sizePolicy1.setHeightForWidth(control_panel->sizePolicy().hasHeightForWidth());
        control_panel->setSizePolicy(sizePolicy1);
        control_panel->setMinimumSize(QSize(450, 500));
        control_panel->setFeatures(QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
        dockWidgetContents_2 = new QWidget();
        dockWidgetContents_2->setObjectName(QStringLiteral("dockWidgetContents_2"));
        horizontalLayout_2 = new QHBoxLayout(dockWidgetContents_2);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        verticalLayout_6 = new QVBoxLayout();
        verticalLayout_6->setObjectName(QStringLiteral("verticalLayout_6"));
        tabWidget = new QTabWidget(dockWidgetContents_2);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        sizePolicy1.setHeightForWidth(tabWidget->sizePolicy().hasHeightForWidth());
        tabWidget->setSizePolicy(sizePolicy1);
        tabWidget->setMaximumSize(QSize(1000, 1000));
        QFont font2;
        font2.setPointSize(11);
        tabWidget->setFont(font2);
        tabWidget->setAutoFillBackground(true);
        tabWidget->setTabBarAutoHide(true);
        tab = new QWidget();
        tab->setObjectName(QStringLiteral("tab"));
        QFont font3;
        font3.setPointSize(18);
        font3.setBold(true);
        font3.setWeight(75);
        tab->setFont(font3);
        verticalLayoutWidget = new QWidget(tab);
        verticalLayoutWidget->setObjectName(QStringLiteral("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(5, 5, 441, 726));
        verticalLayout_density_velocity = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout_density_velocity->setObjectName(QStringLiteral("verticalLayout_density_velocity"));
        verticalLayout_density_velocity->setContentsMargins(0, 0, 0, 0);
        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_density_velocity->addItem(verticalSpacer_2);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_density_velocity->addItem(verticalSpacer);

        tabWidget->addTab(tab, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QStringLiteral("tab_3"));
        gridLayoutWidget_6 = new QWidget(tab_3);
        gridLayoutWidget_6->setObjectName(QStringLiteral("gridLayoutWidget_6"));
        gridLayoutWidget_6->setGeometry(QRect(15, 10, 416, 36));
        gridLayout_6 = new QGridLayout(gridLayoutWidget_6);
        gridLayout_6->setObjectName(QStringLiteral("gridLayout_6"));
        gridLayout_6->setContentsMargins(0, 0, 0, 0);
        pushButton_Observation_Cancel = new QPushButton(gridLayoutWidget_6);
        pushButton_Observation_Cancel->setObjectName(QStringLiteral("pushButton_Observation_Cancel"));

        gridLayout_6->addWidget(pushButton_Observation_Cancel, 0, 3, 1, 1);

        pushButton_Observation_Delete = new QPushButton(gridLayoutWidget_6);
        pushButton_Observation_Delete->setObjectName(QStringLiteral("pushButton_Observation_Delete"));

        gridLayout_6->addWidget(pushButton_Observation_Delete, 0, 2, 1, 1);

        pushButton_Observation_Add = new QPushButton(gridLayoutWidget_6);
        pushButton_Observation_Add->setObjectName(QStringLiteral("pushButton_Observation_Add"));

        gridLayout_6->addWidget(pushButton_Observation_Add, 0, 1, 1, 1);

        pushButton_Observation_Show = new QPushButton(gridLayoutWidget_6);
        pushButton_Observation_Show->setObjectName(QStringLiteral("pushButton_Observation_Show"));

        gridLayout_6->addWidget(pushButton_Observation_Show, 0, 0, 1, 1);

        verticalLayoutWidget_7 = new QWidget(tab_3);
        verticalLayoutWidget_7->setObjectName(QStringLiteral("verticalLayoutWidget_7"));
        verticalLayoutWidget_7->setGeometry(QRect(5, 50, 436, 681));
        verticalLayout_local_monitors = new QVBoxLayout(verticalLayoutWidget_7);
        verticalLayout_local_monitors->setObjectName(QStringLiteral("verticalLayout_local_monitors"));
        verticalLayout_local_monitors->setContentsMargins(0, 0, 0, 0);
        tabWidget->addTab(tab_3, QString());
        tab_1 = new QWidget();
        tab_1->setObjectName(QStringLiteral("tab_1"));
        label_10 = new QLabel(tab_1);
        label_10->setObjectName(QStringLiteral("label_10"));
        label_10->setGeometry(QRect(10, 0, 191, 26));
        QFont font4;
        font4.setPointSize(14);
        label_10->setFont(font4);
        gridLayoutWidget_4 = new QWidget(tab_1);
        gridLayoutWidget_4->setObjectName(QStringLiteral("gridLayoutWidget_4"));
        gridLayoutWidget_4->setGeometry(QRect(10, 120, 411, 26));
        gridLayout_4 = new QGridLayout(gridLayoutWidget_4);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        gridLayout_4->setContentsMargins(0, 0, 0, 0);
        Pressureforce = new QCheckBox(gridLayoutWidget_4);
        Pressureforce->setObjectName(QStringLiteral("Pressureforce"));
        QFont font5;
        font5.setPointSize(12);
        font5.setBold(false);
        font5.setWeight(50);
        Pressureforce->setFont(font5);

        gridLayout_4->addWidget(Pressureforce, 0, 0, 1, 1);

        Viscosityforce = new QCheckBox(gridLayoutWidget_4);
        Viscosityforce->setObjectName(QStringLiteral("Viscosityforce"));
        Viscosityforce->setFont(font5);

        gridLayout_4->addWidget(Viscosityforce, 0, 1, 1, 1);

        Externalforce = new QCheckBox(gridLayoutWidget_4);
        Externalforce->setObjectName(QStringLiteral("Externalforce"));
        Externalforce->setFont(font5);

        gridLayout_4->addWidget(Externalforce, 0, 2, 1, 1);

        gridLayoutWidget_2 = new QWidget(tab_1);
        gridLayoutWidget_2->setObjectName(QStringLiteral("gridLayoutWidget_2"));
        gridLayoutWidget_2->setGeometry(QRect(10, 25, 410, 61));
        gridLayout_2 = new QGridLayout(gridLayoutWidget_2);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        label_2 = new QLabel(gridLayoutWidget_2);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout_2->addWidget(label_2, 1, 2, 1, 1);

        label = new QLabel(gridLayoutWidget_2);
        label->setObjectName(QStringLiteral("label"));

        gridLayout_2->addWidget(label, 0, 2, 1, 1);

        doubleSpinBox_Viscosity = new QDoubleSpinBox(gridLayoutWidget_2);
        doubleSpinBox_Viscosity->setObjectName(QStringLiteral("doubleSpinBox_Viscosity"));
        doubleSpinBox_Viscosity->setDecimals(2);
        doubleSpinBox_Viscosity->setMaximum(1000);
        doubleSpinBox_Viscosity->setSingleStep(0.01);

        gridLayout_2->addWidget(doubleSpinBox_Viscosity, 1, 1, 1, 1);

        label_6 = new QLabel(gridLayoutWidget_2);
        label_6->setObjectName(QStringLiteral("label_6"));

        gridLayout_2->addWidget(label_6, 1, 0, 1, 1);

        doubleSpinBox_GasStiffness = new QDoubleSpinBox(gridLayoutWidget_2);
        doubleSpinBox_GasStiffness->setObjectName(QStringLiteral("doubleSpinBox_GasStiffness"));
        doubleSpinBox_GasStiffness->setDecimals(2);
        doubleSpinBox_GasStiffness->setMaximum(1000);
        doubleSpinBox_GasStiffness->setSingleStep(0.01);

        gridLayout_2->addWidget(doubleSpinBox_GasStiffness, 0, 1, 1, 1);

        label_4 = new QLabel(gridLayoutWidget_2);
        label_4->setObjectName(QStringLiteral("label_4"));

        gridLayout_2->addWidget(label_4, 0, 0, 1, 1);

        doubleSpinBox_CoeffFieldForce = new QDoubleSpinBox(gridLayoutWidget_2);
        doubleSpinBox_CoeffFieldForce->setObjectName(QStringLiteral("doubleSpinBox_CoeffFieldForce"));
        doubleSpinBox_CoeffFieldForce->setDecimals(2);
        doubleSpinBox_CoeffFieldForce->setMaximum(1000);
        doubleSpinBox_CoeffFieldForce->setSingleStep(0.1);

        gridLayout_2->addWidget(doubleSpinBox_CoeffFieldForce, 0, 3, 1, 1);

        doubleSpinBox_CoeffDampingForce = new QDoubleSpinBox(gridLayoutWidget_2);
        doubleSpinBox_CoeffDampingForce->setObjectName(QStringLiteral("doubleSpinBox_CoeffDampingForce"));
        doubleSpinBox_CoeffDampingForce->setDecimals(2);
        doubleSpinBox_CoeffDampingForce->setMaximum(100);
        doubleSpinBox_CoeffDampingForce->setSingleStep(0.01);

        gridLayout_2->addWidget(doubleSpinBox_CoeffDampingForce, 1, 3, 1, 1);

        label_15 = new QLabel(tab_1);
        label_15->setObjectName(QStringLiteral("label_15"));
        label_15->setGeometry(QRect(10, 95, 191, 26));
        label_15->setFont(font4);
        label_13 = new QLabel(tab_1);
        label_13->setObjectName(QStringLiteral("label_13"));
        label_13->setGeometry(QRect(10, 160, 81, 26));
        label_13->setFont(font4);
        gridLayoutWidget_5 = new QWidget(tab_1);
        gridLayoutWidget_5->setObjectName(QStringLiteral("gridLayoutWidget_5"));
        gridLayoutWidget_5->setGeometry(QRect(10, 185, 411, 36));
        gridLayout_5 = new QGridLayout(gridLayoutWidget_5);
        gridLayout_5->setObjectName(QStringLiteral("gridLayout_5"));
        gridLayout_5->setContentsMargins(0, 0, 0, 0);
        pushButton_Beziers_Show = new QPushButton(gridLayoutWidget_5);
        pushButton_Beziers_Show->setObjectName(QStringLiteral("pushButton_Beziers_Show"));

        gridLayout_5->addWidget(pushButton_Beziers_Show, 0, 0, 1, 1);

        pushButton_Beziers_Cancel = new QPushButton(gridLayoutWidget_5);
        pushButton_Beziers_Cancel->setObjectName(QStringLiteral("pushButton_Beziers_Cancel"));

        gridLayout_5->addWidget(pushButton_Beziers_Cancel, 0, 3, 1, 1);

        pushButton_Beziers_Delete = new QPushButton(gridLayoutWidget_5);
        pushButton_Beziers_Delete->setObjectName(QStringLiteral("pushButton_Beziers_Delete"));

        gridLayout_5->addWidget(pushButton_Beziers_Delete, 0, 2, 1, 1);

        pushButton_Beziers_Add = new QPushButton(gridLayoutWidget_5);
        pushButton_Beziers_Add->setObjectName(QStringLiteral("pushButton_Beziers_Add"));

        gridLayout_5->addWidget(pushButton_Beziers_Add, 0, 1, 1, 1);

        label_12 = new QLabel(tab_1);
        label_12->setObjectName(QStringLiteral("label_12"));
        label_12->setGeometry(QRect(10, 230, 81, 26));
        label_12->setFont(font4);
        checkBox_AddConstrain = new QCheckBox(tab_1);
        checkBox_AddConstrain->setObjectName(QStringLiteral("checkBox_AddConstrain"));
        checkBox_AddConstrain->setGeometry(QRect(255, 230, 126, 23));
        gridLayoutWidget_3 = new QWidget(tab_1);
        gridLayoutWidget_3->setObjectName(QStringLiteral("gridLayoutWidget_3"));
        gridLayoutWidget_3->setGeometry(QRect(10, 255, 411, 36));
        gridLayout_3 = new QGridLayout(gridLayoutWidget_3);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        gridLayout_3->setContentsMargins(0, 0, 0, 0);
        pushButton_Blocks_Cancel = new QPushButton(gridLayoutWidget_3);
        pushButton_Blocks_Cancel->setObjectName(QStringLiteral("pushButton_Blocks_Cancel"));

        gridLayout_3->addWidget(pushButton_Blocks_Cancel, 0, 3, 1, 1);

        pushButton_Blocks_Delete = new QPushButton(gridLayoutWidget_3);
        pushButton_Blocks_Delete->setObjectName(QStringLiteral("pushButton_Blocks_Delete"));

        gridLayout_3->addWidget(pushButton_Blocks_Delete, 0, 2, 1, 1);

        pushButton_Blocks_Add = new QPushButton(gridLayoutWidget_3);
        pushButton_Blocks_Add->setObjectName(QStringLiteral("pushButton_Blocks_Add"));

        gridLayout_3->addWidget(pushButton_Blocks_Add, 0, 1, 1, 1);

        pushButton_Blocks_Show = new QPushButton(gridLayoutWidget_3);
        pushButton_Blocks_Show->setObjectName(QStringLiteral("pushButton_Blocks_Show"));

        gridLayout_3->addWidget(pushButton_Blocks_Show, 0, 0, 1, 1);

        verticalLayoutWidget_6 = new QWidget(tab_1);
        verticalLayoutWidget_6->setObjectName(QStringLiteral("verticalLayoutWidget_6"));
        verticalLayoutWidget_6->setGeometry(QRect(5, 390, 431, 341));
        verticalLayout_kernel = new QVBoxLayout(verticalLayoutWidget_6);
        verticalLayout_kernel->setObjectName(QStringLiteral("verticalLayout_kernel"));
        verticalLayout_kernel->setContentsMargins(0, 0, 0, 0);
        gridLayoutWidget = new QWidget(tab_1);
        gridLayoutWidget->setObjectName(QStringLiteral("gridLayoutWidget"));
        gridLayoutWidget->setGeometry(QRect(10, 335, 411, 54));
        gridLayout = new QGridLayout(gridLayoutWidget);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        radioButton_kernel_poly6 = new QRadioButton(gridLayoutWidget);
        radioButton_kernel_poly6->setObjectName(QStringLiteral("radioButton_kernel_poly6"));
        radioButton_kernel_poly6->setFont(font1);
        radioButton_kernel_poly6->setChecked(true);

        gridLayout->addWidget(radioButton_kernel_poly6, 1, 0, 1, 1);

        radioButton_kernel_poly6_half = new QRadioButton(gridLayoutWidget);
        radioButton_kernel_poly6_half->setObjectName(QStringLiteral("radioButton_kernel_poly6_half"));
        radioButton_kernel_poly6_half->setFont(font1);
        radioButton_kernel_poly6_half->setChecked(false);

        gridLayout->addWidget(radioButton_kernel_poly6_half, 2, 0, 1, 1);

        radioButton_kernel_viscosity = new QRadioButton(gridLayoutWidget);
        radioButton_kernel_viscosity->setObjectName(QStringLiteral("radioButton_kernel_viscosity"));
        radioButton_kernel_viscosity->setFont(font1);

        gridLayout->addWidget(radioButton_kernel_viscosity, 1, 2, 1, 1);

        radioButton_kernel_spiky_half = new QRadioButton(gridLayoutWidget);
        radioButton_kernel_spiky_half->setObjectName(QStringLiteral("radioButton_kernel_spiky_half"));
        radioButton_kernel_spiky_half->setFont(font1);

        gridLayout->addWidget(radioButton_kernel_spiky_half, 2, 1, 1, 1);

        radioButton_kernel_spiky = new QRadioButton(gridLayoutWidget);
        radioButton_kernel_spiky->setObjectName(QStringLiteral("radioButton_kernel_spiky"));
        radioButton_kernel_spiky->setFont(font1);

        gridLayout->addWidget(radioButton_kernel_spiky, 1, 1, 1, 1);

        radioButton_kernel_viscosity_half = new QRadioButton(gridLayoutWidget);
        radioButton_kernel_viscosity_half->setObjectName(QStringLiteral("radioButton_kernel_viscosity_half"));
        radioButton_kernel_viscosity_half->setFont(font1);

        gridLayout->addWidget(radioButton_kernel_viscosity_half, 2, 2, 1, 1);

        label_8 = new QLabel(tab_1);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setGeometry(QRect(10, 300, 141, 36));
        label_8->setFont(font4);
        doubleSpinBox_supportRadius = new QDoubleSpinBox(tab_1);
        doubleSpinBox_supportRadius->setObjectName(QStringLiteral("doubleSpinBox_supportRadius"));
        doubleSpinBox_supportRadius->setGeometry(QRect(300, 310, 121, 20));
        doubleSpinBox_supportRadius->setDecimals(2);
        doubleSpinBox_supportRadius->setMaximum(1000);
        doubleSpinBox_supportRadius->setSingleStep(0.01);
        tabWidget->addTab(tab_1, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QStringLiteral("tab_2"));
        label_20 = new QLabel(tab_2);
        label_20->setObjectName(QStringLiteral("label_20"));
        label_20->setGeometry(QRect(15, 0, 131, 26));
        label_20->setFont(font4);
        verticalLayoutWidget_3 = new QWidget(tab_2);
        verticalLayoutWidget_3->setObjectName(QStringLiteral("verticalLayoutWidget_3"));
        verticalLayoutWidget_3->setGeometry(QRect(15, 27, 96, 82));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget_3);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        radioButton_ShowMainDensity = new QRadioButton(verticalLayoutWidget_3);
        radioButton_ShowMainDensity->setObjectName(QStringLiteral("radioButton_ShowMainDensity"));
        radioButton_ShowMainDensity->setFont(font5);

        verticalLayout->addWidget(radioButton_ShowMainDensity);

        radioButton_ShowMainVelocity = new QRadioButton(verticalLayoutWidget_3);
        radioButton_ShowMainVelocity->setObjectName(QStringLiteral("radioButton_ShowMainVelocity"));
        radioButton_ShowMainVelocity->setFont(font5);

        verticalLayout->addWidget(radioButton_ShowMainVelocity);

        radioButton_ShowMainDensityVelocity_Nothing = new QRadioButton(verticalLayoutWidget_3);
        radioButton_ShowMainDensityVelocity_Nothing->setObjectName(QStringLiteral("radioButton_ShowMainDensityVelocity_Nothing"));
        radioButton_ShowMainDensityVelocity_Nothing->setChecked(true);

        verticalLayout->addWidget(radioButton_ShowMainDensityVelocity_Nothing);

        gridLayoutWidget_7 = new QWidget(tab_2);
        gridLayoutWidget_7->setObjectName(QStringLiteral("gridLayoutWidget_7"));
        gridLayoutWidget_7->setGeometry(QRect(15, 105, 381, 151));
        gridLayout_7 = new QGridLayout(gridLayoutWidget_7);
        gridLayout_7->setObjectName(QStringLiteral("gridLayout_7"));
        gridLayout_7->setContentsMargins(0, 0, 0, 0);
        checkBox_ShowCells = new QCheckBox(gridLayoutWidget_7);
        checkBox_ShowCells->setObjectName(QStringLiteral("checkBox_ShowCells"));

        gridLayout_7->addWidget(checkBox_ShowCells, 1, 1, 1, 1);

        checkBox_ShowParticle_Velocity_Direction = new QCheckBox(gridLayoutWidget_7);
        checkBox_ShowParticle_Velocity_Direction->setObjectName(QStringLiteral("checkBox_ShowParticle_Velocity_Direction"));

        gridLayout_7->addWidget(checkBox_ShowParticle_Velocity_Direction, 5, 1, 1, 1);

        checkBox_ShowParticle_Force = new QCheckBox(gridLayoutWidget_7);
        checkBox_ShowParticle_Force->setObjectName(QStringLiteral("checkBox_ShowParticle_Force"));

        gridLayout_7->addWidget(checkBox_ShowParticle_Force, 4, 1, 1, 1);

        checkBox_ShowParticle_NormalTangent = new QCheckBox(gridLayoutWidget_7);
        checkBox_ShowParticle_NormalTangent->setObjectName(QStringLiteral("checkBox_ShowParticle_NormalTangent"));

        gridLayout_7->addWidget(checkBox_ShowParticle_NormalTangent, 3, 1, 1, 1);

        checkBox_ShowReferencePlanes = new QCheckBox(gridLayoutWidget_7);
        checkBox_ShowReferencePlanes->setObjectName(QStringLiteral("checkBox_ShowReferencePlanes"));
        checkBox_ShowReferencePlanes->setFont(font5);

        gridLayout_7->addWidget(checkBox_ShowReferencePlanes, 2, 1, 1, 1);

        checkBox_ShowRoad_Facilities = new QCheckBox(gridLayoutWidget_7);
        checkBox_ShowRoad_Facilities->setObjectName(QStringLiteral("checkBox_ShowRoad_Facilities"));

        gridLayout_7->addWidget(checkBox_ShowRoad_Facilities, 1, 0, 1, 1);

        checkBox_ShowVehicle_Force = new QCheckBox(gridLayoutWidget_7);
        checkBox_ShowVehicle_Force->setObjectName(QStringLiteral("checkBox_ShowVehicle_Force"));

        gridLayout_7->addWidget(checkBox_ShowVehicle_Force, 2, 0, 1, 1);

        checkBox_ShowVehicle_Velocity_Direction = new QCheckBox(gridLayoutWidget_7);
        checkBox_ShowVehicle_Velocity_Direction->setObjectName(QStringLiteral("checkBox_ShowVehicle_Velocity_Direction"));

        gridLayout_7->addWidget(checkBox_ShowVehicle_Velocity_Direction, 3, 0, 1, 1);

        checkBox_ShowVehicle_Tail = new QCheckBox(gridLayoutWidget_7);
        checkBox_ShowVehicle_Tail->setObjectName(QStringLiteral("checkBox_ShowVehicle_Tail"));

        gridLayout_7->addWidget(checkBox_ShowVehicle_Tail, 4, 0, 1, 1);

        checkBox_ShowVehicle_Trajectory = new QCheckBox(gridLayoutWidget_7);
        checkBox_ShowVehicle_Trajectory->setObjectName(QStringLiteral("checkBox_ShowVehicle_Trajectory"));

        gridLayout_7->addWidget(checkBox_ShowVehicle_Trajectory, 5, 0, 1, 1);

        verticalLayoutWidget_4 = new QWidget(tab_2);
        verticalLayoutWidget_4->setObjectName(QStringLiteral("verticalLayoutWidget_4"));
        verticalLayoutWidget_4->setGeometry(QRect(15, 657, 256, 80));
        verticalLayout_2 = new QVBoxLayout(verticalLayoutWidget_4);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        checkBox_SensorData = new QCheckBox(verticalLayoutWidget_4);
        checkBox_SensorData->setObjectName(QStringLiteral("checkBox_SensorData"));

        verticalLayout_2->addWidget(checkBox_SensorData);

        checkBox_EntireDensityVelocity = new QCheckBox(verticalLayoutWidget_4);
        checkBox_EntireDensityVelocity->setObjectName(QStringLiteral("checkBox_EntireDensityVelocity"));

        verticalLayout_2->addWidget(checkBox_EntireDensityVelocity);

        checkBox_Screenshot_Image = new QCheckBox(verticalLayoutWidget_4);
        checkBox_Screenshot_Image->setObjectName(QStringLiteral("checkBox_Screenshot_Image"));

        verticalLayout_2->addWidget(checkBox_Screenshot_Image);

        label_9 = new QLabel(tab_2);
        label_9->setObjectName(QStringLiteral("label_9"));
        label_9->setGeometry(QRect(15, 620, 206, 41));
        label_9->setFont(font4);
        gridLayoutWidget_8 = new QWidget(tab_2);
        gridLayoutWidget_8->setObjectName(QStringLiteral("gridLayoutWidget_8"));
        gridLayoutWidget_8->setGeometry(QRect(15, 405, 381, 112));
        gridLayout_8 = new QGridLayout(gridLayoutWidget_8);
        gridLayout_8->setObjectName(QStringLiteral("gridLayout_8"));
        gridLayout_8->setContentsMargins(0, 0, 0, 0);
        checkBox_RoadNet_Service = new QCheckBox(gridLayoutWidget_8);
        checkBox_RoadNet_Service->setObjectName(QStringLiteral("checkBox_RoadNet_Service"));
        checkBox_RoadNet_Service->setFont(font5);
        checkBox_RoadNet_Service->setChecked(true);

        gridLayout_8->addWidget(checkBox_RoadNet_Service, 3, 0, 1, 1);

        checkBox_RoadNet_Motorway = new QCheckBox(gridLayoutWidget_8);
        checkBox_RoadNet_Motorway->setObjectName(QStringLiteral("checkBox_RoadNet_Motorway"));
        checkBox_RoadNet_Motorway->setFont(font5);
        checkBox_RoadNet_Motorway->setChecked(true);

        gridLayout_8->addWidget(checkBox_RoadNet_Motorway, 1, 1, 1, 1);

        checkBox_RoadNet_Residential = new QCheckBox(gridLayoutWidget_8);
        checkBox_RoadNet_Residential->setObjectName(QStringLiteral("checkBox_RoadNet_Residential"));
        checkBox_RoadNet_Residential->setFont(font1);
        checkBox_RoadNet_Residential->setChecked(true);

        gridLayout_8->addWidget(checkBox_RoadNet_Residential, 1, 2, 1, 1);

        checkBox_RoadNet_Trunk = new QCheckBox(gridLayoutWidget_8);
        checkBox_RoadNet_Trunk->setObjectName(QStringLiteral("checkBox_RoadNet_Trunk"));
        checkBox_RoadNet_Trunk->setChecked(true);

        gridLayout_8->addWidget(checkBox_RoadNet_Trunk, 1, 0, 1, 1);

        checkBox_RoadNet_Secondary = new QCheckBox(gridLayoutWidget_8);
        checkBox_RoadNet_Secondary->setObjectName(QStringLiteral("checkBox_RoadNet_Secondary"));
        checkBox_RoadNet_Secondary->setFont(font5);
        checkBox_RoadNet_Secondary->setChecked(true);

        gridLayout_8->addWidget(checkBox_RoadNet_Secondary, 2, 1, 1, 1);

        checkBox_RoadNet_Primary = new QCheckBox(gridLayoutWidget_8);
        checkBox_RoadNet_Primary->setObjectName(QStringLiteral("checkBox_RoadNet_Primary"));
        checkBox_RoadNet_Primary->setChecked(true);

        gridLayout_8->addWidget(checkBox_RoadNet_Primary, 2, 0, 1, 1);

        checkBox_RoadNet_Tertiary = new QCheckBox(gridLayoutWidget_8);
        checkBox_RoadNet_Tertiary->setObjectName(QStringLiteral("checkBox_RoadNet_Tertiary"));
        checkBox_RoadNet_Tertiary->setFont(font5);
        checkBox_RoadNet_Tertiary->setChecked(true);

        gridLayout_8->addWidget(checkBox_RoadNet_Tertiary, 2, 2, 1, 1);

        checkBox_RoadNet_Oneway = new QCheckBox(gridLayoutWidget_8);
        checkBox_RoadNet_Oneway->setObjectName(QStringLiteral("checkBox_RoadNet_Oneway"));
        checkBox_RoadNet_Oneway->setChecked(true);

        gridLayout_8->addWidget(checkBox_RoadNet_Oneway, 4, 1, 1, 1);

        checkBox_RoadNet_Others = new QCheckBox(gridLayoutWidget_8);
        checkBox_RoadNet_Others->setObjectName(QStringLiteral("checkBox_RoadNet_Others"));
        checkBox_RoadNet_Others->setFont(font1);
        checkBox_RoadNet_Others->setChecked(true);

        gridLayout_8->addWidget(checkBox_RoadNet_Others, 4, 2, 1, 1);

        checkBox_RoadNet_Footway = new QCheckBox(gridLayoutWidget_8);
        checkBox_RoadNet_Footway->setObjectName(QStringLiteral("checkBox_RoadNet_Footway"));
        checkBox_RoadNet_Footway->setFont(font5);
        checkBox_RoadNet_Footway->setChecked(true);

        gridLayout_8->addWidget(checkBox_RoadNet_Footway, 3, 1, 1, 1);

        checkBox_RoadNet_Unclassified = new QCheckBox(gridLayoutWidget_8);
        checkBox_RoadNet_Unclassified->setObjectName(QStringLiteral("checkBox_RoadNet_Unclassified"));
        checkBox_RoadNet_Unclassified->setFont(font5);
        checkBox_RoadNet_Unclassified->setChecked(true);

        gridLayout_8->addWidget(checkBox_RoadNet_Unclassified, 3, 2, 1, 1);

        checkBox_RoadNet_Path = new QCheckBox(gridLayoutWidget_8);
        checkBox_RoadNet_Path->setObjectName(QStringLiteral("checkBox_RoadNet_Path"));
        checkBox_RoadNet_Path->setFont(font5);
        checkBox_RoadNet_Path->setChecked(true);

        gridLayout_8->addWidget(checkBox_RoadNet_Path, 4, 0, 1, 1);

        label_21 = new QLabel(tab_2);
        label_21->setObjectName(QStringLiteral("label_21"));
        label_21->setGeometry(QRect(15, 545, 151, 26));
        label_21->setFont(font4);
        gridLayoutWidget_9 = new QWidget(tab_2);
        gridLayoutWidget_9->setObjectName(QStringLiteral("gridLayoutWidget_9"));
        gridLayoutWidget_9->setGeometry(QRect(15, 575, 412, 36));
        gridLayout_10 = new QGridLayout(gridLayoutWidget_9);
        gridLayout_10->setObjectName(QStringLiteral("gridLayout_10"));
        gridLayout_10->setContentsMargins(0, 0, 0, 0);
        pushButton_Selection_Spline = new QPushButton(gridLayoutWidget_9);
        pushButton_Selection_Spline->setObjectName(QStringLiteral("pushButton_Selection_Spline"));
        pushButton_Selection_Spline->setCheckable(true);
        pushButton_Selection_Spline->setAutoExclusive(true);

        gridLayout_10->addWidget(pushButton_Selection_Spline, 0, 2, 1, 1);

        pushButton_Selection_Observation = new QPushButton(gridLayoutWidget_9);
        pushButton_Selection_Observation->setObjectName(QStringLiteral("pushButton_Selection_Observation"));
        pushButton_Selection_Observation->setCheckable(true);
        pushButton_Selection_Observation->setAutoExclusive(true);

        gridLayout_10->addWidget(pushButton_Selection_Observation, 0, 1, 1, 1);

        pushButton_Selection_Block = new QPushButton(gridLayoutWidget_9);
        pushButton_Selection_Block->setObjectName(QStringLiteral("pushButton_Selection_Block"));
        pushButton_Selection_Block->setCheckable(true);
        pushButton_Selection_Block->setAutoExclusive(true);

        gridLayout_10->addWidget(pushButton_Selection_Block, 0, 3, 1, 1);

        pushButton_Selection_Particle = new QPushButton(gridLayoutWidget_9);
        pushButton_Selection_Particle->setObjectName(QStringLiteral("pushButton_Selection_Particle"));
        pushButton_Selection_Particle->setCheckable(true);
        pushButton_Selection_Particle->setAutoExclusive(true);
        pushButton_Selection_Particle->setFlat(false);

        gridLayout_10->addWidget(pushButton_Selection_Particle, 0, 0, 1, 1);

        pushButton_Selection_Road = new QPushButton(gridLayoutWidget_9);
        pushButton_Selection_Road->setObjectName(QStringLiteral("pushButton_Selection_Road"));
        pushButton_Selection_Road->setCheckable(true);
        pushButton_Selection_Road->setAutoExclusive(true);

        gridLayout_10->addWidget(pushButton_Selection_Road, 0, 4, 1, 1);

        gridLayoutWidget_10 = new QWidget(tab_2);
        gridLayoutWidget_10->setObjectName(QStringLiteral("gridLayoutWidget_10"));
        gridLayoutWidget_10->setGeometry(QRect(155, 25, 160, 80));
        gridLayout_11 = new QGridLayout(gridLayoutWidget_10);
        gridLayout_11->setObjectName(QStringLiteral("gridLayout_11"));
        gridLayout_11->setContentsMargins(0, 0, 0, 0);
        radioButton_ShowParticlesVehicles_Nothing = new QRadioButton(gridLayoutWidget_10);
        radioButton_ShowParticlesVehicles_Nothing->setObjectName(QStringLiteral("radioButton_ShowParticlesVehicles_Nothing"));
        radioButton_ShowParticlesVehicles_Nothing->setChecked(true);

        gridLayout_11->addWidget(radioButton_ShowParticlesVehicles_Nothing, 2, 0, 1, 1);

        radioButton_ShowParticles = new QRadioButton(gridLayoutWidget_10);
        radioButton_ShowParticles->setObjectName(QStringLiteral("radioButton_ShowParticles"));

        gridLayout_11->addWidget(radioButton_ShowParticles, 0, 0, 1, 1);

        radioButton_ShowVehicles = new QRadioButton(gridLayoutWidget_10);
        radioButton_ShowVehicles->setObjectName(QStringLiteral("radioButton_ShowVehicles"));

        gridLayout_11->addWidget(radioButton_ShowVehicles, 1, 0, 1, 1);

        radioButton_ShowVehicles_Bbox = new QRadioButton(gridLayoutWidget_10);
        radioButton_ShowVehicles_Bbox->setObjectName(QStringLiteral("radioButton_ShowVehicles_Bbox"));

        gridLayout_11->addWidget(radioButton_ShowVehicles_Bbox, 1, 1, 1, 1);

        label_22 = new QLabel(tab_2);
        label_22->setObjectName(QStringLiteral("label_22"));
        label_22->setGeometry(QRect(15, 280, 131, 26));
        label_22->setFont(font4);
        label_23 = new QLabel(tab_2);
        label_23->setObjectName(QStringLiteral("label_23"));
        label_23->setGeometry(QRect(15, 375, 131, 26));
        label_23->setFont(font4);
        gridLayoutWidget_11 = new QWidget(tab_2);
        gridLayoutWidget_11->setObjectName(QStringLiteral("gridLayoutWidget_11"));
        gridLayoutWidget_11->setGeometry(QRect(15, 310, 381, 56));
        gridLayout_12 = new QGridLayout(gridLayoutWidget_11);
        gridLayout_12->setObjectName(QStringLiteral("gridLayout_12"));
        gridLayout_12->setContentsMargins(0, 0, 0, 0);
        checkBox_MapElements_Others = new QCheckBox(gridLayoutWidget_11);
        checkBox_MapElements_Others->setObjectName(QStringLiteral("checkBox_MapElements_Others"));
        checkBox_MapElements_Others->setFont(font1);
        checkBox_MapElements_Others->setChecked(true);

        gridLayout_12->addWidget(checkBox_MapElements_Others, 2, 1, 1, 1);

        checkBox_MapElements_Buildings = new QCheckBox(gridLayoutWidget_11);
        checkBox_MapElements_Buildings->setObjectName(QStringLiteral("checkBox_MapElements_Buildings"));
        checkBox_MapElements_Buildings->setChecked(true);

        gridLayout_12->addWidget(checkBox_MapElements_Buildings, 1, 0, 1, 1);

        checkBox_MapElements_Railways = new QCheckBox(gridLayoutWidget_11);
        checkBox_MapElements_Railways->setObjectName(QStringLiteral("checkBox_MapElements_Railways"));
        checkBox_MapElements_Railways->setFont(font5);
        checkBox_MapElements_Railways->setChecked(true);

        gridLayout_12->addWidget(checkBox_MapElements_Railways, 1, 2, 1, 1);

        checkBox_MapElements_Highways = new QCheckBox(gridLayoutWidget_11);
        checkBox_MapElements_Highways->setObjectName(QStringLiteral("checkBox_MapElements_Highways"));
        checkBox_MapElements_Highways->setChecked(true);

        gridLayout_12->addWidget(checkBox_MapElements_Highways, 1, 1, 1, 1);

        checkBox_MapElements_BoundingBox = new QCheckBox(gridLayoutWidget_11);
        checkBox_MapElements_BoundingBox->setObjectName(QStringLiteral("checkBox_MapElements_BoundingBox"));
        checkBox_MapElements_BoundingBox->setChecked(false);

        gridLayout_12->addWidget(checkBox_MapElements_BoundingBox, 2, 0, 1, 1);

        tabWidget->addTab(tab_2, QString());

        verticalLayout_6->addWidget(tabWidget);


        horizontalLayout_2->addLayout(verticalLayout_6);

        control_panel->setWidget(dockWidgetContents_2);
        MainWindow->addDockWidget(static_cast<Qt::DockWidgetArea>(2), control_panel);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Traffic Flow Simulator", 0));
        actionVisual_Velocity_and_Density->setText(QApplication::translate("MainWindow", "Visual Velocity and Density", 0));
        actionTraffic_Config->setText(QApplication::translate("MainWindow", "Traffic Config", 0));
        pushButtonLoad->setText(QString());
        pushButtonOutput_Configuration->setText(QString());
        pushButtonStartPause->setText(QString());
        pushButtonStep->setText(QString());
        pushButtonReset->setText(QString());
        pushButtonSnapshot->setText(QString());
        pushButtonSnapshot_LoadCamera->setText(QString());
        pushButtonSnapshot_SaveCamera->setText(QString());
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("MainWindow", "Density / Velocity", 0));
        pushButton_Observation_Cancel->setText(QApplication::translate("MainWindow", "Cancel", 0));
        pushButton_Observation_Delete->setText(QApplication::translate("MainWindow", "Delete", 0));
        pushButton_Observation_Add->setText(QApplication::translate("MainWindow", "Add", 0));
        pushButton_Observation_Show->setText(QApplication::translate("MainWindow", "Show", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QApplication::translate("MainWindow", "Obsevations", 0));
        label_10->setText(QApplication::translate("MainWindow", "Coefficients", 0));
        Pressureforce->setText(QApplication::translate("MainWindow", "Pressure Force", 0));
        Viscosityforce->setText(QApplication::translate("MainWindow", "Viscosity Force", 0));
        Externalforce->setText(QApplication::translate("MainWindow", "External Force", 0));
        label_2->setText(QApplication::translate("MainWindow", "Damping Force", 0));
        label->setText(QApplication::translate("MainWindow", "Field Force", 0));
        label_6->setText(QApplication::translate("MainWindow", "Viscosity", 0));
        label_4->setText(QApplication::translate("MainWindow", "Gas Stiffness", 0));
        label_15->setText(QApplication::translate("MainWindow", "Forces", 0));
        label_13->setText(QApplication::translate("MainWindow", "Beziers", 0));
        pushButton_Beziers_Show->setText(QApplication::translate("MainWindow", "Show", 0));
        pushButton_Beziers_Cancel->setText(QApplication::translate("MainWindow", "Cancel", 0));
        pushButton_Beziers_Delete->setText(QApplication::translate("MainWindow", "Delete", 0));
        pushButton_Beziers_Add->setText(QApplication::translate("MainWindow", "Add", 0));
        label_12->setText(QApplication::translate("MainWindow", "Blocks", 0));
        checkBox_AddConstrain->setText(QApplication::translate("MainWindow", "Add Constrain", 0));
        pushButton_Blocks_Cancel->setText(QApplication::translate("MainWindow", "Cancel", 0));
        pushButton_Blocks_Delete->setText(QApplication::translate("MainWindow", "Delete", 0));
        pushButton_Blocks_Add->setText(QApplication::translate("MainWindow", "Add", 0));
        pushButton_Blocks_Show->setText(QApplication::translate("MainWindow", "Show", 0));
        radioButton_kernel_poly6->setText(QApplication::translate("MainWindow", "Poly 6", 0));
        radioButton_kernel_poly6_half->setText(QApplication::translate("MainWindow", "Poly 6 Half", 0));
        radioButton_kernel_viscosity->setText(QApplication::translate("MainWindow", "Viscosity", 0));
        radioButton_kernel_spiky_half->setText(QApplication::translate("MainWindow", "Spiky Half", 0));
        radioButton_kernel_spiky->setText(QApplication::translate("MainWindow", "Spiky", 0));
        radioButton_kernel_viscosity_half->setText(QApplication::translate("MainWindow", "Viscosity Half", 0));
        label_8->setText(QApplication::translate("MainWindow", "Kernel Support", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_1), QApplication::translate("MainWindow", "Parameters", 0));
        label_20->setText(QApplication::translate("MainWindow", "MainViewer", 0));
        radioButton_ShowMainDensity->setText(QApplication::translate("MainWindow", "Density", 0));
        radioButton_ShowMainVelocity->setText(QApplication::translate("MainWindow", "Velocity", 0));
        radioButton_ShowMainDensityVelocity_Nothing->setText(QApplication::translate("MainWindow", "None", 0));
        checkBox_ShowCells->setText(QApplication::translate("MainWindow", "Boundary Cells", 0));
        checkBox_ShowParticle_Velocity_Direction->setText(QApplication::translate("MainWindow", "Particle Velocity", 0));
        checkBox_ShowParticle_Force->setText(QApplication::translate("MainWindow", "Particle Force", 0));
        checkBox_ShowParticle_NormalTangent->setText(QApplication::translate("MainWindow", "Particle Norm/Tangent", 0));
        checkBox_ShowReferencePlanes->setText(QApplication::translate("MainWindow", "Reference Planes", 0));
        checkBox_ShowRoad_Facilities->setText(QApplication::translate("MainWindow", "Road Sensors", 0));
        checkBox_ShowVehicle_Force->setText(QApplication::translate("MainWindow", "Vehicle Force", 0));
        checkBox_ShowVehicle_Velocity_Direction->setText(QApplication::translate("MainWindow", "Vehicle Velocity", 0));
        checkBox_ShowVehicle_Tail->setText(QApplication::translate("MainWindow", "Vehicle Tail", 0));
        checkBox_ShowVehicle_Trajectory->setText(QApplication::translate("MainWindow", "Vehicle Trajectory", 0));
        checkBox_SensorData->setText(QApplication::translate("MainWindow", "Cell Sensor Density/Velocity", 0));
        checkBox_EntireDensityVelocity->setText(QApplication::translate("MainWindow", "Entire Density/Velocity", 0));
        checkBox_Screenshot_Image->setText(QApplication::translate("MainWindow", "Screenshot Image", 0));
        label_9->setText(QApplication::translate("MainWindow", "Output Simulation Data", 0));
        checkBox_RoadNet_Service->setText(QApplication::translate("MainWindow", "Service", 0));
        checkBox_RoadNet_Motorway->setText(QApplication::translate("MainWindow", "Motorway", 0));
        checkBox_RoadNet_Residential->setText(QApplication::translate("MainWindow", "Residential", 0));
        checkBox_RoadNet_Trunk->setText(QApplication::translate("MainWindow", "Trunk", 0));
        checkBox_RoadNet_Secondary->setText(QApplication::translate("MainWindow", "Secondary", 0));
        checkBox_RoadNet_Primary->setText(QApplication::translate("MainWindow", "Primary", 0));
        checkBox_RoadNet_Tertiary->setText(QApplication::translate("MainWindow", "Tertiary", 0));
        checkBox_RoadNet_Oneway->setText(QApplication::translate("MainWindow", "Oneway", 0));
        checkBox_RoadNet_Others->setText(QApplication::translate("MainWindow", "Others", 0));
        checkBox_RoadNet_Footway->setText(QApplication::translate("MainWindow", "Footway", 0));
        checkBox_RoadNet_Unclassified->setText(QApplication::translate("MainWindow", "Unclassified", 0));
        checkBox_RoadNet_Path->setText(QApplication::translate("MainWindow", "Path", 0));
        label_21->setText(QApplication::translate("MainWindow", "Selection Choice", 0));
        pushButton_Selection_Spline->setText(QApplication::translate("MainWindow", "Spline", 0));
        pushButton_Selection_Observation->setText(QApplication::translate("MainWindow", "Observation", 0));
        pushButton_Selection_Block->setText(QApplication::translate("MainWindow", "Block", 0));
        pushButton_Selection_Particle->setText(QApplication::translate("MainWindow", "Particle", 0));
        pushButton_Selection_Road->setText(QApplication::translate("MainWindow", "Road", 0));
        radioButton_ShowParticlesVehicles_Nothing->setText(QApplication::translate("MainWindow", "None", 0));
        radioButton_ShowParticles->setText(QApplication::translate("MainWindow", "Particles", 0));
        radioButton_ShowVehicles->setText(QApplication::translate("MainWindow", "Vehicles", 0));
        radioButton_ShowVehicles_Bbox->setText(QApplication::translate("MainWindow", "Bbox", 0));
        label_22->setText(QApplication::translate("MainWindow", "Map Elements", 0));
        label_23->setText(QApplication::translate("MainWindow", "Highways", 0));
        checkBox_MapElements_Others->setText(QApplication::translate("MainWindow", "Others", 0));
        checkBox_MapElements_Buildings->setText(QApplication::translate("MainWindow", "Buildings", 0));
        checkBox_MapElements_Railways->setText(QApplication::translate("MainWindow", "Railways", 0));
        checkBox_MapElements_Highways->setText(QApplication::translate("MainWindow", "Highways", 0));
        checkBox_MapElements_BoundingBox->setText(QApplication::translate("MainWindow", "Bounding Box", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("MainWindow", "Interaction", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
