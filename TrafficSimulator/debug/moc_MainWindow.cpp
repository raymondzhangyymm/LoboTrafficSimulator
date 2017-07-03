/****************************************************************************
** Meta object code from reading C++ file 'MainWindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../MainWindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MainWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[85];
    char stringdata0[3070];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 31), // "on_pushButtonStartPause_clicked"
QT_MOC_LITERAL(2, 43, 0), // ""
QT_MOC_LITERAL(3, 44, 25), // "on_pushButtonLoad_clicked"
QT_MOC_LITERAL(4, 70, 26), // "on_pushButtonReset_clicked"
QT_MOC_LITERAL(5, 97, 25), // "on_pushButtonStep_clicked"
QT_MOC_LITERAL(6, 123, 24), // "on_Pressureforce_clicked"
QT_MOC_LITERAL(7, 148, 7), // "checked"
QT_MOC_LITERAL(8, 156, 25), // "on_Viscosityforce_clicked"
QT_MOC_LITERAL(9, 182, 24), // "on_Externalforce_clicked"
QT_MOC_LITERAL(10, 207, 45), // "on_doubleSpinBox_GasStiffness..."
QT_MOC_LITERAL(11, 253, 42), // "on_doubleSpinBox_Viscosity_ed..."
QT_MOC_LITERAL(12, 296, 35), // "on_radioButton_kernel_poly6_c..."
QT_MOC_LITERAL(13, 332, 35), // "on_radioButton_kernel_spiky_c..."
QT_MOC_LITERAL(14, 368, 39), // "on_radioButton_kernel_viscosi..."
QT_MOC_LITERAL(15, 408, 40), // "on_radioButton_kernel_poly6_h..."
QT_MOC_LITERAL(16, 449, 40), // "on_radioButton_kernel_spiky_h..."
QT_MOC_LITERAL(17, 490, 44), // "on_radioButton_kernel_viscosi..."
QT_MOC_LITERAL(18, 535, 32), // "on_checkBox_AddConstrain_clicked"
QT_MOC_LITERAL(19, 568, 30), // "on_checkBox_SensorData_clicked"
QT_MOC_LITERAL(20, 599, 41), // "on_pushButtonOutput_Configura..."
QT_MOC_LITERAL(21, 641, 41), // "on_checkBox_EntireDensityVelo..."
QT_MOC_LITERAL(22, 683, 48), // "on_doubleSpinBox_CoeffFieldFo..."
QT_MOC_LITERAL(23, 732, 50), // "on_doubleSpinBox_CoeffDamping..."
QT_MOC_LITERAL(24, 783, 29), // "on_pushButtonSnapshot_clicked"
QT_MOC_LITERAL(25, 813, 36), // "on_checkBox_Screenshot_Image_..."
QT_MOC_LITERAL(26, 850, 46), // "on_doubleSpinBox_supportRadiu..."
QT_MOC_LITERAL(27, 897, 33), // "on_pushButton_Blocks_Show_cli..."
QT_MOC_LITERAL(28, 931, 32), // "on_pushButton_Blocks_Add_clicked"
QT_MOC_LITERAL(29, 964, 35), // "on_pushButton_Blocks_Delete_c..."
QT_MOC_LITERAL(30, 1000, 35), // "on_pushButton_Blocks_Cancel_c..."
QT_MOC_LITERAL(31, 1036, 39), // "on_checkBox_ShowReferencePlan..."
QT_MOC_LITERAL(32, 1076, 29), // "on_checkBox_ShowCells_clicked"
QT_MOC_LITERAL(33, 1106, 38), // "on_radioButton_ShowMainDensit..."
QT_MOC_LITERAL(34, 1145, 39), // "on_radioButton_ShowMainVeloci..."
QT_MOC_LITERAL(35, 1185, 54), // "on_radioButton_ShowMainDensit..."
QT_MOC_LITERAL(36, 1240, 36), // "on_radioButton_ShowParticles_..."
QT_MOC_LITERAL(37, 1277, 35), // "on_radioButton_ShowVehicles_c..."
QT_MOC_LITERAL(38, 1313, 52), // "on_radioButton_ShowParticlesV..."
QT_MOC_LITERAL(39, 1366, 34), // "on_pushButton_Beziers_Show_cl..."
QT_MOC_LITERAL(40, 1401, 33), // "on_pushButton_Beziers_Add_cli..."
QT_MOC_LITERAL(41, 1435, 36), // "on_pushButton_Beziers_Delete_..."
QT_MOC_LITERAL(42, 1472, 36), // "on_pushButton_Beziers_Cancel_..."
QT_MOC_LITERAL(43, 1509, 38), // "on_pushButton_Observation_Sho..."
QT_MOC_LITERAL(44, 1548, 37), // "on_pushButton_Observation_Add..."
QT_MOC_LITERAL(45, 1586, 40), // "on_pushButton_Observation_Del..."
QT_MOC_LITERAL(46, 1627, 40), // "on_pushButton_Observation_Can..."
QT_MOC_LITERAL(47, 1668, 46), // "on_checkBox_ShowParticle_Norm..."
QT_MOC_LITERAL(48, 1715, 38), // "on_checkBox_ShowParticle_Forc..."
QT_MOC_LITERAL(49, 1754, 51), // "on_checkBox_ShowParticle_Velo..."
QT_MOC_LITERAL(50, 1806, 37), // "on_checkBox_ShowVehicle_Force..."
QT_MOC_LITERAL(51, 1844, 50), // "on_checkBox_ShowVehicle_Veloc..."
QT_MOC_LITERAL(52, 1895, 35), // "on_checkBox_RoadNet_Primary_c..."
QT_MOC_LITERAL(53, 1931, 37), // "on_checkBox_RoadNet_Secondary..."
QT_MOC_LITERAL(54, 1969, 39), // "on_checkBox_RoadNet_Residenti..."
QT_MOC_LITERAL(55, 2009, 34), // "on_checkBox_RoadNet_Oneway_cl..."
QT_MOC_LITERAL(56, 2044, 36), // "on_checkBox_ShowVehicle_Tail_..."
QT_MOC_LITERAL(57, 2081, 42), // "on_checkBox_ShowVehicle_Traje..."
QT_MOC_LITERAL(58, 2124, 39), // "on_checkBox_ShowRoad_Faciliti..."
QT_MOC_LITERAL(59, 2164, 40), // "on_pushButtonSnapshot_SaveCam..."
QT_MOC_LITERAL(60, 2205, 40), // "on_pushButtonSnapshot_LoadCam..."
QT_MOC_LITERAL(61, 2246, 40), // "on_pushButton_Selection_Parti..."
QT_MOC_LITERAL(62, 2287, 43), // "on_pushButton_Selection_Obser..."
QT_MOC_LITERAL(63, 2331, 38), // "on_pushButton_Selection_Splin..."
QT_MOC_LITERAL(64, 2370, 37), // "on_pushButton_Selection_Block..."
QT_MOC_LITERAL(65, 2408, 36), // "on_pushButton_Selection_Road_..."
QT_MOC_LITERAL(66, 2445, 14), // "stepSimulation"
QT_MOC_LITERAL(67, 2460, 17), // "stepSimulationEXP"
QT_MOC_LITERAL(68, 2478, 40), // "on_radioButton_ShowVehicles_B..."
QT_MOC_LITERAL(69, 2519, 41), // "on_checkBox_MapElements_Build..."
QT_MOC_LITERAL(70, 2561, 40), // "on_checkBox_MapElements_Railw..."
QT_MOC_LITERAL(71, 2602, 40), // "on_checkBox_MapElements_Highw..."
QT_MOC_LITERAL(72, 2643, 38), // "on_checkBox_MapElements_Other..."
QT_MOC_LITERAL(73, 2682, 43), // "on_checkBox_MapElements_Bound..."
QT_MOC_LITERAL(74, 2726, 33), // "on_checkBox_RoadNet_Trunk_cli..."
QT_MOC_LITERAL(75, 2760, 36), // "on_checkBox_RoadNet_Motorway_..."
QT_MOC_LITERAL(76, 2797, 36), // "on_checkBox_RoadNet_Tertiary_..."
QT_MOC_LITERAL(77, 2834, 35), // "on_checkBox_RoadNet_Service_c..."
QT_MOC_LITERAL(78, 2870, 35), // "on_checkBox_RoadNet_Footway_c..."
QT_MOC_LITERAL(79, 2906, 40), // "on_checkBox_RoadNet_Unclassif..."
QT_MOC_LITERAL(80, 2947, 32), // "on_checkBox_RoadNet_Path_clicked"
QT_MOC_LITERAL(81, 2980, 34), // "on_checkBox_RoadNet_Others_cl..."
QT_MOC_LITERAL(82, 3015, 26), // "on_tabWidget_tabBarClicked"
QT_MOC_LITERAL(83, 3042, 5), // "index"
QT_MOC_LITERAL(84, 3048, 21) // "clearBadLocalMonitors"

    },
    "MainWindow\0on_pushButtonStartPause_clicked\0"
    "\0on_pushButtonLoad_clicked\0"
    "on_pushButtonReset_clicked\0"
    "on_pushButtonStep_clicked\0"
    "on_Pressureforce_clicked\0checked\0"
    "on_Viscosityforce_clicked\0"
    "on_Externalforce_clicked\0"
    "on_doubleSpinBox_GasStiffness_editingFinished\0"
    "on_doubleSpinBox_Viscosity_editingFinished\0"
    "on_radioButton_kernel_poly6_clicked\0"
    "on_radioButton_kernel_spiky_clicked\0"
    "on_radioButton_kernel_viscosity_clicked\0"
    "on_radioButton_kernel_poly6_half_clicked\0"
    "on_radioButton_kernel_spiky_half_clicked\0"
    "on_radioButton_kernel_viscosity_half_clicked\0"
    "on_checkBox_AddConstrain_clicked\0"
    "on_checkBox_SensorData_clicked\0"
    "on_pushButtonOutput_Configuration_clicked\0"
    "on_checkBox_EntireDensityVelocity_clicked\0"
    "on_doubleSpinBox_CoeffFieldForce_editingFinished\0"
    "on_doubleSpinBox_CoeffDampingForce_editingFinished\0"
    "on_pushButtonSnapshot_clicked\0"
    "on_checkBox_Screenshot_Image_clicked\0"
    "on_doubleSpinBox_supportRadius_editingFinished\0"
    "on_pushButton_Blocks_Show_clicked\0"
    "on_pushButton_Blocks_Add_clicked\0"
    "on_pushButton_Blocks_Delete_clicked\0"
    "on_pushButton_Blocks_Cancel_clicked\0"
    "on_checkBox_ShowReferencePlanes_clicked\0"
    "on_checkBox_ShowCells_clicked\0"
    "on_radioButton_ShowMainDensity_clicked\0"
    "on_radioButton_ShowMainVelocity_clicked\0"
    "on_radioButton_ShowMainDensityVelocity_Nothing_clicked\0"
    "on_radioButton_ShowParticles_clicked\0"
    "on_radioButton_ShowVehicles_clicked\0"
    "on_radioButton_ShowParticlesVehicles_Nothing_clicked\0"
    "on_pushButton_Beziers_Show_clicked\0"
    "on_pushButton_Beziers_Add_clicked\0"
    "on_pushButton_Beziers_Delete_clicked\0"
    "on_pushButton_Beziers_Cancel_clicked\0"
    "on_pushButton_Observation_Show_clicked\0"
    "on_pushButton_Observation_Add_clicked\0"
    "on_pushButton_Observation_Delete_clicked\0"
    "on_pushButton_Observation_Cancel_clicked\0"
    "on_checkBox_ShowParticle_NormalTangent_clicked\0"
    "on_checkBox_ShowParticle_Force_clicked\0"
    "on_checkBox_ShowParticle_Velocity_Direction_clicked\0"
    "on_checkBox_ShowVehicle_Force_clicked\0"
    "on_checkBox_ShowVehicle_Velocity_Direction_clicked\0"
    "on_checkBox_RoadNet_Primary_clicked\0"
    "on_checkBox_RoadNet_Secondary_clicked\0"
    "on_checkBox_RoadNet_Residential_clicked\0"
    "on_checkBox_RoadNet_Oneway_clicked\0"
    "on_checkBox_ShowVehicle_Tail_clicked\0"
    "on_checkBox_ShowVehicle_Trajectory_clicked\0"
    "on_checkBox_ShowRoad_Facilities_clicked\0"
    "on_pushButtonSnapshot_SaveCamera_clicked\0"
    "on_pushButtonSnapshot_LoadCamera_clicked\0"
    "on_pushButton_Selection_Particle_clicked\0"
    "on_pushButton_Selection_Observation_clicked\0"
    "on_pushButton_Selection_Spline_clicked\0"
    "on_pushButton_Selection_Block_clicked\0"
    "on_pushButton_Selection_Road_clicked\0"
    "stepSimulation\0stepSimulationEXP\0"
    "on_radioButton_ShowVehicles_Bbox_clicked\0"
    "on_checkBox_MapElements_Buildings_clicked\0"
    "on_checkBox_MapElements_Railways_clicked\0"
    "on_checkBox_MapElements_Highways_clicked\0"
    "on_checkBox_MapElements_Others_clicked\0"
    "on_checkBox_MapElements_BoundingBox_clicked\0"
    "on_checkBox_RoadNet_Trunk_clicked\0"
    "on_checkBox_RoadNet_Motorway_clicked\0"
    "on_checkBox_RoadNet_Tertiary_clicked\0"
    "on_checkBox_RoadNet_Service_clicked\0"
    "on_checkBox_RoadNet_Footway_clicked\0"
    "on_checkBox_RoadNet_Unclassified_clicked\0"
    "on_checkBox_RoadNet_Path_clicked\0"
    "on_checkBox_RoadNet_Others_clicked\0"
    "on_tabWidget_tabBarClicked\0index\0"
    "clearBadLocalMonitors"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      81,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  419,    2, 0x08 /* Private */,
       3,    0,  420,    2, 0x08 /* Private */,
       4,    0,  421,    2, 0x08 /* Private */,
       5,    0,  422,    2, 0x08 /* Private */,
       6,    1,  423,    2, 0x08 /* Private */,
       8,    1,  426,    2, 0x08 /* Private */,
       9,    1,  429,    2, 0x08 /* Private */,
      10,    0,  432,    2, 0x08 /* Private */,
      11,    0,  433,    2, 0x08 /* Private */,
      12,    1,  434,    2, 0x08 /* Private */,
      13,    1,  437,    2, 0x08 /* Private */,
      14,    1,  440,    2, 0x08 /* Private */,
      15,    1,  443,    2, 0x08 /* Private */,
      16,    1,  446,    2, 0x08 /* Private */,
      17,    1,  449,    2, 0x08 /* Private */,
      18,    1,  452,    2, 0x08 /* Private */,
      19,    1,  455,    2, 0x08 /* Private */,
      20,    0,  458,    2, 0x08 /* Private */,
      21,    1,  459,    2, 0x08 /* Private */,
      22,    0,  462,    2, 0x08 /* Private */,
      23,    0,  463,    2, 0x08 /* Private */,
      24,    0,  464,    2, 0x08 /* Private */,
      25,    1,  465,    2, 0x08 /* Private */,
      26,    0,  468,    2, 0x08 /* Private */,
      27,    0,  469,    2, 0x08 /* Private */,
      28,    0,  470,    2, 0x08 /* Private */,
      29,    0,  471,    2, 0x08 /* Private */,
      30,    0,  472,    2, 0x08 /* Private */,
      31,    1,  473,    2, 0x08 /* Private */,
      32,    1,  476,    2, 0x08 /* Private */,
      33,    1,  479,    2, 0x08 /* Private */,
      34,    1,  482,    2, 0x08 /* Private */,
      35,    1,  485,    2, 0x08 /* Private */,
      36,    1,  488,    2, 0x08 /* Private */,
      37,    1,  491,    2, 0x08 /* Private */,
      38,    1,  494,    2, 0x08 /* Private */,
      39,    0,  497,    2, 0x08 /* Private */,
      40,    0,  498,    2, 0x08 /* Private */,
      41,    0,  499,    2, 0x08 /* Private */,
      42,    0,  500,    2, 0x08 /* Private */,
      43,    0,  501,    2, 0x08 /* Private */,
      44,    0,  502,    2, 0x08 /* Private */,
      45,    0,  503,    2, 0x08 /* Private */,
      46,    0,  504,    2, 0x08 /* Private */,
      47,    1,  505,    2, 0x08 /* Private */,
      48,    1,  508,    2, 0x08 /* Private */,
      49,    1,  511,    2, 0x08 /* Private */,
      50,    1,  514,    2, 0x08 /* Private */,
      51,    1,  517,    2, 0x08 /* Private */,
      52,    1,  520,    2, 0x08 /* Private */,
      53,    1,  523,    2, 0x08 /* Private */,
      54,    1,  526,    2, 0x08 /* Private */,
      55,    1,  529,    2, 0x08 /* Private */,
      56,    1,  532,    2, 0x08 /* Private */,
      57,    1,  535,    2, 0x08 /* Private */,
      58,    1,  538,    2, 0x08 /* Private */,
      59,    0,  541,    2, 0x08 /* Private */,
      60,    0,  542,    2, 0x08 /* Private */,
      61,    1,  543,    2, 0x08 /* Private */,
      62,    1,  546,    2, 0x08 /* Private */,
      63,    1,  549,    2, 0x08 /* Private */,
      64,    1,  552,    2, 0x08 /* Private */,
      65,    1,  555,    2, 0x08 /* Private */,
      66,    0,  558,    2, 0x08 /* Private */,
      67,    0,  559,    2, 0x08 /* Private */,
      68,    1,  560,    2, 0x08 /* Private */,
      69,    1,  563,    2, 0x08 /* Private */,
      70,    1,  566,    2, 0x08 /* Private */,
      71,    1,  569,    2, 0x08 /* Private */,
      72,    1,  572,    2, 0x08 /* Private */,
      73,    1,  575,    2, 0x08 /* Private */,
      74,    1,  578,    2, 0x08 /* Private */,
      75,    1,  581,    2, 0x08 /* Private */,
      76,    1,  584,    2, 0x08 /* Private */,
      77,    1,  587,    2, 0x08 /* Private */,
      78,    1,  590,    2, 0x08 /* Private */,
      79,    1,  593,    2, 0x08 /* Private */,
      80,    1,  596,    2, 0x08 /* Private */,
      81,    1,  599,    2, 0x08 /* Private */,
      82,    1,  602,    2, 0x08 /* Private */,
      84,    0,  605,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::Int,   83,
    QMetaType::Void,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MainWindow *_t = static_cast<MainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_pushButtonStartPause_clicked(); break;
        case 1: _t->on_pushButtonLoad_clicked(); break;
        case 2: _t->on_pushButtonReset_clicked(); break;
        case 3: _t->on_pushButtonStep_clicked(); break;
        case 4: _t->on_Pressureforce_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->on_Viscosityforce_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: _t->on_Externalforce_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: _t->on_doubleSpinBox_GasStiffness_editingFinished(); break;
        case 8: _t->on_doubleSpinBox_Viscosity_editingFinished(); break;
        case 9: _t->on_radioButton_kernel_poly6_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 10: _t->on_radioButton_kernel_spiky_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 11: _t->on_radioButton_kernel_viscosity_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 12: _t->on_radioButton_kernel_poly6_half_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 13: _t->on_radioButton_kernel_spiky_half_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 14: _t->on_radioButton_kernel_viscosity_half_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 15: _t->on_checkBox_AddConstrain_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 16: _t->on_checkBox_SensorData_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 17: _t->on_pushButtonOutput_Configuration_clicked(); break;
        case 18: _t->on_checkBox_EntireDensityVelocity_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 19: _t->on_doubleSpinBox_CoeffFieldForce_editingFinished(); break;
        case 20: _t->on_doubleSpinBox_CoeffDampingForce_editingFinished(); break;
        case 21: _t->on_pushButtonSnapshot_clicked(); break;
        case 22: _t->on_checkBox_Screenshot_Image_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 23: _t->on_doubleSpinBox_supportRadius_editingFinished(); break;
        case 24: _t->on_pushButton_Blocks_Show_clicked(); break;
        case 25: _t->on_pushButton_Blocks_Add_clicked(); break;
        case 26: _t->on_pushButton_Blocks_Delete_clicked(); break;
        case 27: _t->on_pushButton_Blocks_Cancel_clicked(); break;
        case 28: _t->on_checkBox_ShowReferencePlanes_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 29: _t->on_checkBox_ShowCells_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 30: _t->on_radioButton_ShowMainDensity_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 31: _t->on_radioButton_ShowMainVelocity_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 32: _t->on_radioButton_ShowMainDensityVelocity_Nothing_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 33: _t->on_radioButton_ShowParticles_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 34: _t->on_radioButton_ShowVehicles_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 35: _t->on_radioButton_ShowParticlesVehicles_Nothing_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 36: _t->on_pushButton_Beziers_Show_clicked(); break;
        case 37: _t->on_pushButton_Beziers_Add_clicked(); break;
        case 38: _t->on_pushButton_Beziers_Delete_clicked(); break;
        case 39: _t->on_pushButton_Beziers_Cancel_clicked(); break;
        case 40: _t->on_pushButton_Observation_Show_clicked(); break;
        case 41: _t->on_pushButton_Observation_Add_clicked(); break;
        case 42: _t->on_pushButton_Observation_Delete_clicked(); break;
        case 43: _t->on_pushButton_Observation_Cancel_clicked(); break;
        case 44: _t->on_checkBox_ShowParticle_NormalTangent_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 45: _t->on_checkBox_ShowParticle_Force_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 46: _t->on_checkBox_ShowParticle_Velocity_Direction_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 47: _t->on_checkBox_ShowVehicle_Force_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 48: _t->on_checkBox_ShowVehicle_Velocity_Direction_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 49: _t->on_checkBox_RoadNet_Primary_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 50: _t->on_checkBox_RoadNet_Secondary_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 51: _t->on_checkBox_RoadNet_Residential_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 52: _t->on_checkBox_RoadNet_Oneway_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 53: _t->on_checkBox_ShowVehicle_Tail_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 54: _t->on_checkBox_ShowVehicle_Trajectory_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 55: _t->on_checkBox_ShowRoad_Facilities_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 56: _t->on_pushButtonSnapshot_SaveCamera_clicked(); break;
        case 57: _t->on_pushButtonSnapshot_LoadCamera_clicked(); break;
        case 58: _t->on_pushButton_Selection_Particle_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 59: _t->on_pushButton_Selection_Observation_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 60: _t->on_pushButton_Selection_Spline_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 61: _t->on_pushButton_Selection_Block_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 62: _t->on_pushButton_Selection_Road_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 63: _t->stepSimulation(); break;
        case 64: _t->stepSimulationEXP(); break;
        case 65: _t->on_radioButton_ShowVehicles_Bbox_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 66: _t->on_checkBox_MapElements_Buildings_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 67: _t->on_checkBox_MapElements_Railways_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 68: _t->on_checkBox_MapElements_Highways_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 69: _t->on_checkBox_MapElements_Others_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 70: _t->on_checkBox_MapElements_BoundingBox_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 71: _t->on_checkBox_RoadNet_Trunk_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 72: _t->on_checkBox_RoadNet_Motorway_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 73: _t->on_checkBox_RoadNet_Tertiary_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 74: _t->on_checkBox_RoadNet_Service_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 75: _t->on_checkBox_RoadNet_Footway_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 76: _t->on_checkBox_RoadNet_Unclassified_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 77: _t->on_checkBox_RoadNet_Path_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 78: _t->on_checkBox_RoadNet_Others_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 79: _t->on_tabWidget_tabBarClicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 80: _t->clearBadLocalMonitors(); break;
        default: ;
        }
    }
}

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow.data,
      qt_meta_data_MainWindow,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 81)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 81;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 81)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 81;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
