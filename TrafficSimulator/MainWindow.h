#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <fstream>
#include <sstream>
#include "MainViewer.h"
#include "MinViewer.h"
#include "LocalMonitor.h"
#include "KernelMonitor.h"
#include "ParticleSystemTraffic.h"
#include "ParticleSystemTraffic1D.h"
#include "ParticleSystemTrafficEXP.h"
#include "ParticleSystemTrafficTrj.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    SystemParameter *system_parameter_p;
    ParticleSystemTraffic* traffic_system_p;

private slots:
    void on_pushButtonStartPause_clicked();

    void on_pushButtonLoad_clicked();

    void on_pushButtonReset_clicked();

    void on_pushButtonStep_clicked();

    void on_Pressureforce_clicked(bool checked);

    void on_Viscosityforce_clicked(bool checked);

    void on_Externalforce_clicked(bool checked);

    void on_doubleSpinBox_GasStiffness_editingFinished();

    void on_doubleSpinBox_Viscosity_editingFinished();

    void on_radioButton_kernel_poly6_clicked(bool checked);

    void on_radioButton_kernel_spiky_clicked(bool checked);

    void on_radioButton_kernel_viscosity_clicked(bool checked);

    void on_radioButton_kernel_poly6_half_clicked(bool checked);

    void on_radioButton_kernel_spiky_half_clicked(bool checked);

    void on_radioButton_kernel_viscosity_half_clicked(bool checked);

    void on_checkBox_AddConstrain_clicked(bool checked);

    void on_checkBox_SensorData_clicked(bool checked);

    void on_pushButtonOutput_Configuration_clicked();

    void on_checkBox_EntireDensityVelocity_clicked(bool checked);

    void on_doubleSpinBox_CoeffFieldForce_editingFinished();

    void on_doubleSpinBox_CoeffDampingForce_editingFinished();

    void on_pushButtonSnapshot_clicked();

    void on_checkBox_Screenshot_Image_clicked(bool checked);

    void on_doubleSpinBox_supportRadius_editingFinished();

    void on_pushButton_Blocks_Show_clicked();

    void on_pushButton_Blocks_Add_clicked();

    void on_pushButton_Blocks_Delete_clicked();

    void on_pushButton_Blocks_Cancel_clicked();

    void on_checkBox_ShowReferencePlanes_clicked(bool checked);

    void on_checkBox_ShowCells_clicked(bool checked);

    void on_radioButton_ShowMainDensity_clicked(bool checked);

    void on_radioButton_ShowMainVelocity_clicked(bool checked);

    void on_radioButton_ShowMainDensityVelocity_Nothing_clicked(bool checked);

    void on_radioButton_ShowParticles_clicked(bool checked);

    void on_radioButton_ShowVehicles_clicked(bool checked);

    void on_radioButton_ShowParticlesVehicles_Nothing_clicked(bool checked);

    void on_pushButton_Beziers_Show_clicked();

    void on_pushButton_Beziers_Add_clicked();

    void on_pushButton_Beziers_Delete_clicked();

    void on_pushButton_Beziers_Cancel_clicked();

    void on_pushButton_Observation_Show_clicked();

    void on_pushButton_Observation_Add_clicked();

    void on_pushButton_Observation_Delete_clicked();

    void on_pushButton_Observation_Cancel_clicked();

    void on_checkBox_ShowParticle_NormalTangent_clicked(bool checked);

    void on_checkBox_ShowParticle_Force_clicked(bool checked);

    void on_checkBox_ShowParticle_Velocity_Direction_clicked(bool checked);

    void on_checkBox_ShowVehicle_Force_clicked(bool checked);

    void on_checkBox_ShowVehicle_Velocity_Direction_clicked(bool checked);

    void on_checkBox_RoadNet_Primary_clicked(bool checked);

    void on_checkBox_RoadNet_Secondary_clicked(bool checked);

    void on_checkBox_RoadNet_Residential_clicked(bool checked);

    void on_checkBox_RoadNet_Oneway_clicked(bool checked);

    void on_checkBox_ShowVehicle_Tail_clicked(bool checked);

    void on_checkBox_ShowVehicle_Trajectory_clicked(bool checked);

    void on_checkBox_ShowRoad_Facilities_clicked(bool checked);

    void on_pushButtonSnapshot_SaveCamera_clicked();

    void on_pushButtonSnapshot_LoadCamera_clicked();

    void on_pushButton_Selection_Particle_clicked(bool checked);

    void on_pushButton_Selection_Observation_clicked(bool checked);

    void on_pushButton_Selection_Spline_clicked(bool checked);

    void on_pushButton_Selection_Block_clicked(bool checked);

    void on_pushButton_Selection_Road_clicked(bool checked);

    void stepSimulation();
    void stepSimulationEXP();

    void on_radioButton_ShowVehicles_Bbox_clicked(bool checked);

    void on_checkBox_MapElements_Buildings_clicked(bool checked);

    void on_checkBox_MapElements_Railways_clicked(bool checked);

    void on_checkBox_MapElements_Highways_clicked(bool checked);

    void on_checkBox_MapElements_Others_clicked(bool checked);

    void on_checkBox_MapElements_BoundingBox_clicked(bool checked);

    void on_checkBox_RoadNet_Trunk_clicked(bool checked);

    void on_checkBox_RoadNet_Motorway_clicked(bool checked);

    void on_checkBox_RoadNet_Tertiary_clicked(bool checked);

    void on_checkBox_RoadNet_Service_clicked(bool checked);

    void on_checkBox_RoadNet_Footway_clicked(bool checked);

    void on_checkBox_RoadNet_Unclassified_clicked(bool checked);

    void on_checkBox_RoadNet_Path_clicked(bool checked);

    void on_checkBox_RoadNet_Others_clicked(bool checked);

    void on_tabWidget_tabBarClicked(int index);

public slots:
    void clearBadLocalMonitors();

private:
    Ui::MainWindow *ui;

    QString project_directory, working_directory;
    QIcon icon_start, icon_stop;

    // the timer for calling the simulator, fixed at 1ms
    QTimer simulation_steptimer;
    int simulation_stepcount_0;
    int simulation_stepcount_1;
    int simulation_stepcount_2;
    double EXP_timeinterval_0;
//    int EXP_stepcount_1;
//    int EXP_stepcount_2;
//    int EXP_stepcount_3;

    // the timer for simulating and rendering
    QElapsedTimer fps_timer;
    int fps_count;
    double fps_time;

    MainViewer *p_viewer;
    MinViewer *p_viewer_1, *p_viewer_2;
    KernelMonitor *p_kernel_monitor;

    void initViewers();
    void initOutputFiles();

    void saveUIToSetting();
    void loadSettingToUI();

    void swapCamera(qglviewer::Camera *p_cam_1, qglviewer::Camera *p_cam_2);

    vector<Node*> *p_observation_list;
    vector<LocalMonitor*> local_monitor_list;
    void updateLocalMonitors();

    // file operators
    int filecount_fieldmesh;
    void exportCurrentVelocity(char *filename);
    void exportCurrentDensity(char *filename);

    vector<ofstream> sensorfile_density, sensorfile_velocity;
    void exportSensorFileDensityVelocity();

    int filecount_screenshot;
    void exportScreenshot(char *filename);
    void saveScreenShotPPM(const char *filename);
    void saveScreenShotPNG(const char *filename);

    bool isShowingLocalMoniters;
};

#endif // MAINWINDOW_H
