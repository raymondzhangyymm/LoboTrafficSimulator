#include "MainWindow.h"
#include "ui_mainwindow.h"

//======================================================================
// Constructor/destructor of the main window of the simulator.
//======================================================================
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Create and set the working directory
    project_directory = QDir::currentPath();
    working_directory = project_directory + "/workdata";
    if (!QDir(working_directory).exists())
    {
        QDir().mkdir(working_directory);
    }

    icon_start.addFile(QDir::currentPath() + "/icon/media_playback_start.png");
    icon_stop.addFile(QDir::currentPath() + "/icon/media_playback_stop.png");

    simulation_stepcount_0 = 0;
    simulation_stepcount_1 = 0;
    simulation_stepcount_2 = 0;
    EXP_timeinterval_0 = 0;
//    EXP_stepcount_1 = 0;
//    EXP_stepcount_2 = 0;
//    EXP_stepcount_3 = 0;

    fps_count = 0;
    fps_time = 0;

    filecount_fieldmesh = 0;
    filecount_screenshot = 0;

    system_parameter_p = NULL;
    traffic_system_p = NULL;

    QGLFormat format;
    format.setDoubleBuffer(true);
    format.setDepth(false);
    format.setAlpha(false);
    format.setSampleBuffers(true);
    format.setSamples(16);

    p_viewer = new MainViewer(this);
    p_viewer->setFormat(format);

    p_viewer_1 = new MinViewer(this);
    p_viewer_1->setFormat(format);

    p_viewer_2 = new MinViewer(this);
    p_viewer_2->setFormat(format);

    ui->verticalLayout_main->addWidget(p_viewer);
    ui->verticalLayout_density_velocity->addWidget(p_viewer_1);
    ui->verticalLayout_density_velocity->addWidget(p_viewer_2);

    p_kernel_monitor = new KernelMonitor(this);
    ui->verticalLayout_kernel->addWidget(p_kernel_monitor);

    if (ui->tabWidget->currentIndex() == 1)
    {
        isShowingLocalMoniters = true;
    }
    else
    {
        isShowingLocalMoniters = false;
    }
}

MainWindow::~MainWindow()
{
    int num_sensorfiles = traffic_system_p->sensor_lists.size();
    for (int i=0; i<num_sensorfiles; ++i)
    {
        sensorfile_density[i].close();
        sensorfile_velocity[i].close();
    }
    delete ui;
}

void MainWindow::initViewers()
{
    if (!traffic_system_p)
    {
        return;
    }

    p_viewer->initDisplayObject(traffic_system_p, 0);
    p_viewer_1->initDisplayObject(traffic_system_p, 1);
    p_viewer_2->initDisplayObject(traffic_system_p, 2);

    p_viewer->showReferencePlanes = ui->checkBox_ShowReferencePlanes->isChecked();
    p_viewer->showCarTail = ui->checkBox_ShowVehicle_Tail->isChecked();

    p_kernel_monitor->init(system_parameter_p->SupportRadius,
                           system_parameter_p->ParticleRadius,
                           p_viewer->particle_color,
                           system_parameter_p->sphere_mesh);
}

void MainWindow::initOutputFiles()
{
    if (!traffic_system_p)
    {
        return;
    }

    QString subworkingdir(working_directory + "/sensordata");
    if (!QDir(subworkingdir).exists())
    {
        QDir().mkdir(subworkingdir);
    }
    int num_sensorfiles = traffic_system_p->sensor_lists.size();
    sensorfile_density.resize(num_sensorfiles);
    sensorfile_velocity.resize(num_sensorfiles);
    for (int i=0; i<num_sensorfiles; ++i)
    {
        QString filename = subworkingdir + "/sensor_density_lane" + QString::number(i) + ".txt";
        sensorfile_density[i].open(filename.toLatin1());
        filename = subworkingdir + "/sensor_velocity_lane" + QString::number(i) + ".txt";
        sensorfile_velocity[i].open(filename.toLatin1());
    }
}

void MainWindow::stepSimulation()
{
    // increment the fps counters
    fps_count++;

    // compute the time for simulation
    fps_timer.start();
    traffic_system_p->step();
    fps_time += fps_timer.nsecsElapsed();

    // compute the time for rendering
    fps_timer.start();
    p_viewer->updateGL();
    fps_time += fps_timer.nsecsElapsed();

    // update the fps averaged by 10
    if (fps_count == 10)
    {
        p_viewer->fps = 1e9 * 10.0 / double(fps_time);
        fps_count = 0;
        fps_time = 0;
    }

    // update the local moniters only if the tab is activated.
    if (isShowingLocalMoniters)
    {
        updateLocalMonitors();
    }

    // increment the simulation counters
    simulation_stepcount_0++;
    simulation_stepcount_1++;
    simulation_stepcount_2++;

    // every one simulation step do, e.g. every 25ms
    if (simulation_stepcount_0 >= 1)
    {
        simulation_stepcount_0 = 0;

        // output the screen shots of density and velocity.
        if (system_parameter_p->OutputScreenshot)
        {
            char outputfilename[256];
            sprintf(outputfilename, "screenshot_density_%04d.png", filecount_screenshot);
            QString filename = working_directory + "/screenshot/" + outputfilename;

            p_viewer->display_mode_D = 1; // show density
            p_viewer->updateGL();
            saveScreenShotPNG(filename.toLatin1().data());

            sprintf(outputfilename, "screenshot_velocity_%04d.png", filecount_screenshot);
            filename = working_directory + "/screenshot/" + outputfilename;
            p_viewer->display_mode_D = 2; // show velocity
            p_viewer->updateGL();
            saveScreenShotPNG(filename.toLatin1().data());

            filecount_screenshot++;
        }
    }

    // every one simulation step do, e.g. every 25ms
    if (simulation_stepcount_1 >= 1)
    {
        simulation_stepcount_1 = 0;

//        if (system_parameter_p->OutputEntireDensityVelocity)
//        {
//            char outputfilename[256];
//            sprintf(outputfilename, "density_frame_%04d.obj", filecount_fieldmesh);
//            QString filename = working_directory + "/velocitydensity" + outputfilename;
//            exportCurrentDensity(filename.toLatin1().data());

//            sprintf(outputfilename, "velocity_frame_%04d.obj", filecount_fieldmesh);
//            filename = working_directory + "/velocitydensity" + outputfilename;
//            exportCurrentVelocity(filename.toLatin1().data());

//            filecount_fieldmesh++;
//        }
    }

    // every 40 simulation step do, e.g. every 1s
    if (simulation_stepcount_2 >= 40)
    {
        simulation_stepcount_2 = 0;

        if (system_parameter_p->OutputCellSensorDensityVelocity)
        {
            exportSensorFileDensityVelocity();
            cout << "Output to SensorDensityVelocity files of time "
                 << traffic_system_p->simulation_time << endl;
        }
    }
}

void MainWindow::stepSimulationEXP()
{
    // increment the fps counters
    fps_count++;

    // compute the time for simulation
    fps_timer.start();
    traffic_system_p->step();
    fps_time += fps_timer.nsecsElapsed();

    // compute the time for rendering
    fps_timer.start();
    p_viewer->updateGL();
    fps_time += fps_timer.nsecsElapsed();

    // update the fps averaged by 10
    if (fps_count == 10)
    {
        p_viewer->fps = 1e9 * 10.0 / double(fps_time);
        fps_count = 0;
        fps_time = 0;
    }

    // update the local moniters only if the tab is activated.
    if (isShowingLocalMoniters)
    {
        updateLocalMonitors();
    }

    //------------------------------------------------------------------------------------
    // various experiments
    // output the screen shots of density and velocity.
    if (system_parameter_p->ExperimentType == 21 ||
        system_parameter_p->ExperimentType == 22)
    {
        char outputfilename[256];
        sprintf(outputfilename, "screenshot_density_%04d.png", filecount_screenshot);
        QString filename = working_directory + "/screenshot/" + outputfilename;

        p_viewer->display_mode_D = 1; // show density
        p_viewer->updateGL();
        saveScreenShotPNG(filename.toLatin1().data());

        sprintf(outputfilename, "screenshot_velocity_%04d.png", filecount_screenshot);
        filename = working_directory + "/screenshot/" + outputfilename;
        p_viewer->display_mode_D = 2; // show velocity
        p_viewer->updateGL();
        saveScreenShotPNG(filename.toLatin1().data());

        filecount_screenshot++;
    }

    else if (system_parameter_p->ExperimentType == 23)
    {
        if (traffic_system_p->simulation_time >= 30 &&
                traffic_system_p->simulation_time <= 60.0)
        {
            exportSensorFileDensityVelocity();
            cout << "EXP 23 : output to SensorDensityVelocity files of time "
                 << traffic_system_p->simulation_time << endl;

            ui->checkBox_Screenshot_Image->setChecked(true);
            system_parameter_p->OutputScreenshot = true;

            traffic_system_p->isUpdatingFieldMeshes = true;
        }
        else
        {
            ui->checkBox_Screenshot_Image->setChecked(false);
            system_parameter_p->OutputScreenshot = false;

            traffic_system_p->isUpdatingFieldMeshes = false;
        }

        if (traffic_system_p->simulation_time >= 35 &&
                traffic_system_p->simulation_time <= 50)
        {
            ui->checkBox_AddConstrain->setChecked(true);
            system_parameter_p->AddConstrain = true;
        }
        else
        {
            ui->checkBox_AddConstrain->setChecked(false);
            system_parameter_p->AddConstrain = false;
        }
    }

    else if (system_parameter_p->ExperimentType == 13)
    {
        if (traffic_system_p->simulation_time >= 20 &&
                traffic_system_p->simulation_time <= 90.0)
        {
            exportSensorFileDensityVelocity();
            cout << "EXP 13 : output to SensorDensityVelocity files of time "
                 << traffic_system_p->simulation_time << endl;
        }

        if (traffic_system_p->simulation_time >= 30)
        {
            if (traffic_system_p->simulation_time <= 60)
            {
                ui->checkBox_AddConstrain->setChecked(true);
                system_parameter_p->AddConstrain = true;
            }
            else
            {
                ui->checkBox_AddConstrain->setChecked(false);
                system_parameter_p->AddConstrain = false;
            }
        }
    }

    else if (system_parameter_p->ExperimentType == 15)
    {
        if (traffic_system_p->simulation_time >= 20 &&
                traffic_system_p->simulation_time <= 160.0)
        {
            exportSensorFileDensityVelocity();
            cout << "EXP1D 5 : output to SensorDensityVelocity files of time "
                 << traffic_system_p->simulation_time << endl;

            char outputfilename[256];
            sprintf(outputfilename, "density_frame_%04d.obj", filecount_fieldmesh);
            QString filename = working_directory + "/velocitydensity" + outputfilename;
            exportCurrentDensity(filename.toLatin1().data());

            sprintf(outputfilename, "velocity_frame_%04d.obj", filecount_fieldmesh);
            filename = working_directory + "/velocitydensity" + outputfilename;
            exportCurrentVelocity(filename.toLatin1().data());

            filecount_fieldmesh++;
        }

        if (traffic_system_p->simulation_time >= 30)
        {
            if (traffic_system_p->simulation_time <= 60)
            {
                ui->checkBox_AddConstrain->setChecked(true);
                system_parameter_p->AddConstrain = true;
            }
            else
            {
                ui->checkBox_AddConstrain->setChecked(false);
                system_parameter_p->AddConstrain = false;
            }
        }
    }

    else if (system_parameter_p->ExperimentType == 24)
    {
        if (traffic_system_p->simulation_time >= 30)
        {
            exportSensorFileDensityVelocity();
            cout << "EXP 24 : output to SensorDensityVelocity files of time "
                 << traffic_system_p->simulation_time
                 << " , road inflow rate = "
                 << traffic_system_p->road_list[0]->inflow_rate << endl;

            // increase the inflow
            traffic_system_p->road_list[0]->inflow_rate[0] += 0.2;
        }
    }

    // 1D fds stable
    else if (system_parameter_p->ExperimentType == 14)
    {
        EXP_timeinterval_0 += traffic_system_p->model_parameter->DeltaTime;
        if (EXP_timeinterval_0 >= 30)
        {
            EXP_timeinterval_0 = 0;
            exportSensorFileDensityVelocity();
            cout << "EXP 14 : output to SensorDensityVelocity files of time "
                 << traffic_system_p->simulation_time
                 << " , road inflow rate = "
                 << traffic_system_p->road_list[0]->inflow_rate[0] << endl;

            // increase the inflow
            traffic_system_p->road_list[0]->inflow_rate[0] += 0.04;
        }
    }
}

//======================================================================
// Private slots.
//======================================================================
void MainWindow::on_pushButtonLoad_clicked()
{
    // Load the traffic config file
    QDir::setCurrent(project_directory + "/configuration");
    QString file_name = QFileDialog::getOpenFileName(this, tr("Open Traffic Config File"),
                                                     ".", tr("*.txt"));
    if (!file_name.length())
    {
        return;
    }
    QDir::setCurrent(project_directory);

    system_parameter_p = new SystemParameter(file_name.toLatin1());
    if (system_parameter_p->SuccessfulInit)
    {
        // init the Particle System
        if (system_parameter_p->SimulationMode == 0)
        {
            traffic_system_p = new ParticleSystemTrafficTrj(system_parameter_p);
            connect(&simulation_steptimer, SIGNAL(timeout()), this, SLOT(stepSimulation()));
        }
        else if (system_parameter_p->SimulationMode == 1)
        {
            traffic_system_p = new ParticleSystemTraffic1D(system_parameter_p);
            connect(&simulation_steptimer, SIGNAL(timeout()), this, SLOT(stepSimulation()));
        }
        else if (system_parameter_p->SimulationMode == 2)
        {
            traffic_system_p = new ParticleSystemTraffic(system_parameter_p);
            connect(&simulation_steptimer, SIGNAL(timeout()), this, SLOT(stepSimulation()));
        }
        else if (system_parameter_p->SimulationMode == 9)
        {
            traffic_system_p = new ParticleSystemTrafficEXP(system_parameter_p);
            connect(&simulation_steptimer, SIGNAL(timeout()), this, SLOT(stepSimulationEXP()));
        }

        if (!traffic_system_p)
        {
            cout << "ERROR! Failed to initialize the traffic system!\n";
            return;
        }

        traffic_system_p->initSystem();
        traffic_system_p->isUpdatingParticles = ui->radioButton_ShowParticles->isChecked();
        traffic_system_p->isUpdatingCars = ui->radioButton_ShowVehicles->isChecked();
        traffic_system_p->isUpdatingFieldMeshes = !ui->radioButton_ShowMainDensityVelocity_Nothing->isChecked();

        p_observation_list = &traffic_system_p->observation_list;

        loadSettingToUI();

        initViewers();
        initOutputFiles();
    }
}

void MainWindow::on_pushButtonStartPause_clicked()
{
    if (simulation_steptimer.isActive())
    {
        simulation_steptimer.stop();
        ui->pushButtonStartPause->setIcon(icon_start);
        cout << "Simulation stops " << endl;
    }
    else
    {
        // Start the simulation only if it is ready
        if (traffic_system_p)
        {
            simulation_steptimer.start(1);
            ui->pushButtonStartPause->setIcon(icon_stop);
            cout << "Simulation starts " << endl;
        }
    }
}

void MainWindow::on_pushButtonReset_clicked()
{
    simulation_steptimer.stop();
    ui->pushButtonStartPause->setIcon(icon_start);
    cout << "Simulation stops and resets" << endl;

    if (traffic_system_p)
    {
        if (traffic_system_p->model_parameter->SimulationMode == 0)
        {
            traffic_system_p->reset();
        }
        else
        {
            traffic_system_p->initSystem();
        }
    }

    p_viewer->updateGL();
}

void MainWindow::on_pushButtonStep_clicked()
{
    simulation_steptimer.stop();
    ui->pushButtonStartPause->setIcon(icon_start);
    cout << "Simulation stops " << endl;

    // Step the simulation only if it is ready
    if (traffic_system_p)
    {
        if (traffic_system_p->model_parameter->SimulationMode == 9)
        {
            stepSimulationEXP();
        }
        else
        {
            stepSimulation();
        }
    }
}

void MainWindow::exportCurrentVelocity(char *filename)
{
//    // Open the file as binary format.
//    ofstream ofs(filename, ios::binary);

//    int num_verts = traffic_system_p->particle_list.size();
//    int num_faces = traffic_system_p->face_list.size();
//    ofs << "#vertices - " << num_verts << " #faces - " << num_faces << endl;

//    //---------------------------------------------------------
//    // Output vertices.
//    //---------------------------------------------------------
//    stringstream buffer;
//    buffer.str("");
//    int line_count = 0;
//    for (int k=0; k < num_verts; ++k)
//    {
//        VisualParticle* pnode = traffic_system_p->visual_particle_list[k];

//        buffer << "v "
//               << pnode->position[0] << " "
//               << pnode->position[1] << " "
//               << pnode->visual_velocity_normalized << endl;
//        line_count++;

//        // Output to file every 10000 lines.
//        if (line_count >= 10000)
//        {
//            //write the buffer into the file.
//            ofs.write(buffer.str().c_str(), buffer.str().length());
//            buffer.str("");
//            line_count = 0;
//        }
//    }
//    // Write the last buffer to the file.
//    ofs.write(buffer.str().c_str(), buffer.str().length());

//    //----------------------------------------------------------
//    // Output faces.
//    //----------------------------------------------------------
//    buffer.str("");
//    line_count = 0;
//    for (int k=0; k < num_faces; ++k)
//    {
//        TriangleFace* pface = traffic_system_p->face_list[k];
//        buffer << "f "
//               << pface->parti_0->particleID + 1 << " "
//               << pface->parti_1->particleID + 1 << " "
//               << pface->parti_2->particleID + 1 << endl;
//        line_count++;

//        // Output every 10000 lines.
//        if (line_count >= 10000)
//        {
//            //write the buffer into the file.
//            ofs.write(buffer.str().c_str(), buffer.str().length());
//            buffer.str("");
//            line_count = 0;
//        }
//    }

//    //write the last buffer into the file.
//    ofs.write(buffer.str().c_str(), buffer.str().length());

//    ofs.close();
}

void MainWindow::exportCurrentDensity(char *filename)
{
//    // Open the file as binary format.
//    ofstream ofs(filename, ios::binary);

//    int num_verts = traffic_system_p->particle_list.size();
//    int num_faces = traffic_system_p->face_list.size();
//    ofs << "#vertices - " << num_verts << " #faces - " << num_faces << endl;

//    //---------------------------------------------------------
//    // Output vertices.
//    //---------------------------------------------------------
//    stringstream buffer;
//    buffer.str("");
//    int line_count = 0;
//    for (int k=0; k < num_verts; ++k)
//    {
//        VisualParticle* pnode = traffic_system_p->visual_particle_list[k];

//        buffer << "v "
//               << pnode->position[0] << " "
//               << pnode->position[1] << " "
//               << pnode->visual_density_normalized << endl;
//        line_count++;

//        // Output to file every 10000 lines.
//        if (line_count >= 10000)
//        {
//            //write the buffer into the file.
//            ofs.write(buffer.str().c_str(), buffer.str().length());
//            buffer.str("");
//            line_count = 0;
//        }
//    }
//    // Write the last buffer to the file.
//    ofs.write(buffer.str().c_str(), buffer.str().length());

//    //----------------------------------------------------------
//    // Output faces.
//    //----------------------------------------------------------
//    buffer.str("");
//    line_count = 0;
//    for (int k=0; k < num_faces; ++k)
//    {
//        TriangleFace* pface = traffic_system_p->face_list[k];

//        //if (pface->parti_0->isInRoad || pface->parti_1->isInRoad || pface->parti_2->isInRoad) // ignore the faces close to edge
//        {
//            buffer << "f "
//                   << pface->parti_0->particleID + 1 << " "
//                   << pface->parti_1->particleID + 1 << " "
//                   << pface->parti_2->particleID + 1 << endl;
//            line_count++;

//            // Output every 10000 lines.
//            if (line_count >= 10000)
//            {
//                //write the buffer into the file.
//                ofs.write(buffer.str().c_str(), buffer.str().length());
//                buffer.str("");
//                line_count = 0;
//            }
//        }
//    }
//    //write the last buffer into the file.
//    ofs.write(buffer.str().c_str(), buffer.str().length());

//    ofs.close();
}

void MainWindow::exportSensorFileDensityVelocity()
{
    stringstream buffer_0, buffer_1;
    int num_sensor_lists = traffic_system_p->sensor_lists.size();
    for (int k=0; k<num_sensor_lists; ++k)
    {
        buffer_0.str("");
        buffer_1.str("");

        buffer_0 << traffic_system_p->simulation_time << " ";
        buffer_1 << traffic_system_p->simulation_time << " ";

        buffer_0 << k << " ";
        buffer_1 << k << " ";

        vector<Node> *p_sensor_list = &traffic_system_p->sensor_lists[k];
        int num_sensors = p_sensor_list->size();
        for (int i=0; i<num_sensors; ++i)
        {
            Node *p_node = &p_sensor_list->at(i);
            buffer_0 << p_node->density << " ";
            buffer_1 << p_node->velocity << " ";
        }
        buffer_0 << "\n";
        buffer_1 << "\n";

        sensorfile_density[k].write(buffer_0.str().c_str(), buffer_0.str().length());
        sensorfile_velocity[k].write(buffer_1.str().c_str(), buffer_1.str().length());
    }

//    int num_sensor_lists = traffic_system_p->sensor_lists.size();
//    for (int k=0; k<num_sensor_lists; ++k)
//    {
//        ofstream *of_d = &sensorfile_density[k];
//        ofstream *of_v = &sensorfile_velocity[k];

//        (*of_d) << traffic_system_p->simulation_time << " " << k << " ";
//        (*of_v) << traffic_system_p->simulation_time << " " << k << " ";

//        vector<Node> *p_sensor_list = &traffic_system_p->sensor_lists[k];
//        int num_sensors = p_sensor_list->size();
//        for (int i=0; i<num_sensors; ++i)
//        {
//            Node *p_node = &p_sensor_list->at(i);
//            (*of_d) << p_node->density << " ";
//            (*of_v) << p_node->velocity << " ";
//        }
//        (*of_d) << "\r\n";
//        (*of_v) << "\r\n";

//        of_d->flush();
//        of_v->flush();
//    }

//    cellsensorfile_density << traffic_system_p->simulation_time << " ";
//    cellsensorfile_velocity << traffic_system_p->simulation_time << " ";
//    int num_sensors = traffic_system_p->cell_sensors.size();
//    for (int k=0; k<num_sensors; ++k)
//    {
//        VisualParticle* pnode = &traffic_system_p->cell_sensors[k];
//        cellsensorfile_density << pnode->density << " ";
//        cellsensorfile_velocity << pnode->velocity << " ";
//    }
//    cellsensorfile_density << "\r\n";
//    cellsensorfile_velocity << "\r\n";
}

void MainWindow::swapCamera(qglviewer::Camera *p_cam_1, qglviewer::Camera *p_cam_2)
{
    qglviewer::Vec pos = p_cam_1->position();
    qglviewer::Vec upv = p_cam_1->upVector();
    qglviewer::Vec viewv = p_cam_1->viewDirection();

    p_cam_1->setPosition(p_cam_2->position());
    p_cam_1->setUpVector(p_cam_2->upVector());
    p_cam_1->setViewDirection(p_cam_2->viewDirection());

    p_cam_2->setPosition(pos);
    p_cam_2->setUpVector(upv);
    p_cam_2->setViewDirection(viewv);
}

//======================================================================
// Functions for setting.
//======================================================================
void MainWindow::loadSettingToUI()
{
    if (!system_parameter_p)
    {
        return;
    }

    ui->Pressureforce->setChecked(system_parameter_p->UsePressureForce);
    ui->Viscosityforce->setChecked(system_parameter_p->UseViscosityForce);
    ui->Externalforce->setChecked(system_parameter_p->UseExternalForce);
    ui->checkBox_AddConstrain->setChecked(system_parameter_p->AddConstrain);

//    ui->checkBox_AddSource_Road_0->setChecked(system_parameter_p->AddSource[0]);
//    ui->checkBox_AddSource_Road_1->setChecked(system_parameter_p->AddSource[1]);
//    ui->checkBox_AddSource_Road_2->setChecked(system_parameter_p->AddSource[2]);
//    ui->checkBox_AddSource_Road_3->setChecked(system_parameter_p->AddSource[3]);

//    ui->doubleSpinBox_ProductionRate_Road_0->setValue(system_parameter_p->ProductionRate[0]);
//    ui->doubleSpinBox_ProductionRate_Road_1->setValue(system_parameter_p->ProductionRate[1]);
//    ui->doubleSpinBox_ProductionRate_Road_2->setValue(system_parameter_p->ProductionRate[2]);
//    ui->doubleSpinBox_ProductionRate_Road_3->setValue(system_parameter_p->ProductionRate[3]);

    ui->doubleSpinBox_GasStiffness->setValue(system_parameter_p->GasStiffness);
    ui->doubleSpinBox_Viscosity->setValue(system_parameter_p->Viscosity);
    ui->doubleSpinBox_CoeffFieldForce->setValue(system_parameter_p->CoeffFieldForce);
    ui->doubleSpinBox_CoeffDampingForce->setValue(system_parameter_p->CoeffDampingForce);
    ui->doubleSpinBox_supportRadius->setValue(system_parameter_p->SupportRadius);

    ui->checkBox_SensorData->setChecked(system_parameter_p->OutputCellSensorDensityVelocity);
    ui->checkBox_EntireDensityVelocity->setChecked(system_parameter_p->OutputEntireDensityVelocity);
    ui->checkBox_Screenshot_Image->setChecked(system_parameter_p->OutputScreenshot);
}

void MainWindow::saveUIToSetting()
{
    if (!system_parameter_p)
    {
        return;
    }

    system_parameter_p->UsePressureForce = ui->Pressureforce->isChecked();
    system_parameter_p->UseViscosityForce = ui->Viscosityforce->isChecked();
    system_parameter_p->UseExternalForce = ui->Externalforce->isChecked();
    system_parameter_p->AddConstrain = ui->checkBox_AddConstrain->isChecked();

//    system_parameter_p->AddSource[0] = ui->checkBox_AddSource_Road_0->isChecked();
//    system_parameter_p->AddSource[1] = ui->checkBox_AddSource_Road_1->isChecked();
//    system_parameter_p->AddSource[2] = ui->checkBox_AddSource_Road_2->isChecked();
//    system_parameter_p->AddSource[3] = ui->checkBox_AddSource_Road_3->isChecked();

//    system_parameter_p->ProductionRate[0] = ui->doubleSpinBox_ProductionRate_Road_0->value();
//    system_parameter_p->ProductionRate[1] = ui->doubleSpinBox_ProductionRate_Road_1->value();
//    system_parameter_p->ProductionRate[2] = ui->doubleSpinBox_ProductionRate_Road_2->value();
//    system_parameter_p->ProductionRate[3] = ui->doubleSpinBox_ProductionRate_Road_3->value();

    system_parameter_p->GasStiffness = ui->doubleSpinBox_GasStiffness->value();
    system_parameter_p->Viscosity = ui->doubleSpinBox_Viscosity->value();
    system_parameter_p->CoeffFieldForce = ui->doubleSpinBox_CoeffFieldForce->value();
    system_parameter_p->CoeffDampingForce = ui->doubleSpinBox_CoeffDampingForce->value();
    system_parameter_p->SupportRadius = ui->doubleSpinBox_supportRadius->value();

    system_parameter_p->OutputCellSensorDensityVelocity = ui->checkBox_SensorData->isChecked();
    system_parameter_p->OutputEntireDensityVelocity = ui->checkBox_EntireDensityVelocity->isChecked();
    system_parameter_p->OutputScreenshot = ui->checkBox_Screenshot_Image->isChecked();

    traffic_system_p->initSystem();
}

void MainWindow::on_Pressureforce_clicked(bool checked)
{
    if (system_parameter_p)
    {
        system_parameter_p->UsePressureForce = checked;
    }
}

void MainWindow::on_Viscosityforce_clicked(bool checked)
{
    if (system_parameter_p)
    {
        system_parameter_p->UseViscosityForce = checked;
    }
}

void MainWindow::on_Externalforce_clicked(bool checked)
{
    if (system_parameter_p)
    {
        system_parameter_p->UseExternalForce = checked;
    }
}

void MainWindow::on_checkBox_AddConstrain_clicked(bool checked)
{
    if (system_parameter_p)
    {
        system_parameter_p->AddConstrain = checked;
    }
}

void MainWindow::on_doubleSpinBox_GasStiffness_editingFinished()
{
    if (system_parameter_p)
    {
        system_parameter_p->GasStiffness = ui->doubleSpinBox_GasStiffness->value();
    }
}

void MainWindow::on_doubleSpinBox_Viscosity_editingFinished()
{
    if (system_parameter_p)
    {
        system_parameter_p->Viscosity = ui->doubleSpinBox_Viscosity->value();
    }
}

void MainWindow::on_doubleSpinBox_CoeffFieldForce_editingFinished()
{
    if (system_parameter_p)
    {
        system_parameter_p->CoeffFieldForce = ui->doubleSpinBox_CoeffFieldForce->value();
    }
}

void MainWindow::on_doubleSpinBox_CoeffDampingForce_editingFinished()
{
    if (system_parameter_p)
    {
        system_parameter_p->CoeffDampingForce = ui->doubleSpinBox_CoeffDampingForce->value();
    }
}

void MainWindow::on_doubleSpinBox_supportRadius_editingFinished()
{
    if (system_parameter_p)
    {
        system_parameter_p->SupportRadius = ui->doubleSpinBox_supportRadius->value();
        Kernel::initKernels(system_parameter_p->SupportRadius,system_parameter_p->SupportRadius);
        p_kernel_monitor->updateSupport(system_parameter_p->SupportRadius);
    }
}

void MainWindow::on_radioButton_kernel_poly6_clicked(bool checked)
{
    if (checked)
    {
        p_kernel_monitor->display_mode = 0;
        p_kernel_monitor->updateGL();
    }
}

void MainWindow::on_radioButton_kernel_poly6_half_clicked(bool checked)
{
    if (checked)
    {
        p_kernel_monitor->display_mode = 3;
        p_kernel_monitor->updateGL();
    }
    else
    {
        p_kernel_monitor->display_mode = 0;
        p_kernel_monitor->updateGL();
    }
}

void MainWindow::on_radioButton_kernel_spiky_clicked(bool checked)
{
    if (checked)
    {
        p_kernel_monitor->display_mode = 1;
        p_kernel_monitor->updateGL();
    }
}

void MainWindow::on_radioButton_kernel_spiky_half_clicked(bool checked)
{
    if (checked)
    {
        p_kernel_monitor->display_mode = 4;
        p_kernel_monitor->updateGL();
    }
    else
    {
        p_kernel_monitor->display_mode = 1;
        p_kernel_monitor->updateGL();
    }
}

void MainWindow::on_radioButton_kernel_viscosity_clicked(bool checked)
{
    if (checked)
    {
        p_kernel_monitor->display_mode = 2;
        p_kernel_monitor->updateGL();
    }
}

void MainWindow::on_radioButton_kernel_viscosity_half_clicked(bool checked)
{
    if (checked)
    {
        p_kernel_monitor->display_mode = 5;
        p_kernel_monitor->updateGL();
    }
    else
    {
        p_kernel_monitor->display_mode = 2;
        p_kernel_monitor->updateGL();
    }
}

void MainWindow::on_pushButtonOutput_Configuration_clicked()
{
    QString filename = QFileDialog::getSaveFileName
                   (this, tr("Save the config file"), ".", tr("*.txt"));

    if (!filename.length() || !traffic_system_p)
    {
        return;
    }

    // Set the directory.
    QDir::setCurrent(working_directory);

    // open the file
    ofstream ofs(filename.toLatin1());

    // save the roads
    int num_road = traffic_system_p->road_list.size();
    for (int k=0; k<num_road; ++k)
    {
        Road *p_road = traffic_system_p->road_list[k];

        ofs << "r " << p_road->num_lanes << endl;

        int num_bele = p_road->road_curve.beziers.size();
        for (int i=0; i<num_bele; ++i)
        {
            Bezier *p_bele = &p_road->road_curve.beziers[i];

            ofs << "b ";
            for (int j=0; j<4; ++j)
            {
                ofs << p_bele->control_point[j].x << " "
                    << p_bele->control_point[j].y << " ";
            }
            ofs << endl;
        }
    }

    // save the model parameters
    ofs << "p RestDensity " << traffic_system_p->model_parameter->RestDensity << endl;
    ofs << "p ParticleRadius " << traffic_system_p->model_parameter->ParticleRadius << endl;
//    ofs << "p ProductionRate[0] " << traffic_system_p->model_parameter->ProductionRate[0] << endl;
    ofs << "p SupportRadius " << traffic_system_p->model_parameter->SupportRadius << endl;
    ofs << "p DeltaTime " << traffic_system_p->model_parameter->DeltaTime << endl;
    ofs << "p GasStiffness " << traffic_system_p->model_parameter->GasStiffness << endl;
    ofs << "p Viscocity " << traffic_system_p->model_parameter->Viscosity << endl;

    // save the forces
    ofs << "f UsePressureForce " << traffic_system_p->model_parameter->UsePressureForce << endl;
    ofs << "f UseViscosityForce " << traffic_system_p->model_parameter->UseViscosityForce << endl;
    ofs << "f UseExternalForce " << traffic_system_p->model_parameter->UseExternalForce << endl;
//    ofs << "f AddConstrain " << traffic_system_p->model_parameter->AddConstrain << endl;

    ofs.close();
}

void MainWindow::on_checkBox_SensorData_clicked(bool checked)
{
    if (system_parameter_p)
    {
        system_parameter_p->OutputCellSensorDensityVelocity = checked;
    }
}

void MainWindow::on_checkBox_EntireDensityVelocity_clicked(bool checked)
{
    if (system_parameter_p)
    {
        system_parameter_p->OutputEntireDensityVelocity = checked;
    }
}

void MainWindow::on_checkBox_Screenshot_Image_clicked(bool checked)
{
    if (system_parameter_p)
    {
        system_parameter_p->OutputScreenshot = checked;
    }
}

void MainWindow::on_pushButtonSnapshot_clicked()
{
    QDir::setCurrent(working_directory+"/screenshot");
    QString filename = QFileDialog::getSaveFileName
                       (this, tr("Save the snapshot"), ".", tr("*.png"));
    if (!filename.length() || !traffic_system_p)
    {
        return;
    }

//    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//    glViewport(0, 0, p_viewer->width(), p_viewer->height());
//    saveScreenShotPPM(filename.toLatin1());
    saveScreenShotPNG(filename.toLatin1());
}

void MainWindow::saveScreenShotPPM(const char *filename)
{
    p_viewer->updateScreenShot();
    unsigned char *screenshot = p_viewer->screenshot;
    int h = p_viewer->height();
    int w = p_viewer->width();

    FILE *f = fopen(filename, "w");
    fprintf(f, "P3\n%d %d\n%d\n", w, h, 255);
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            int cur = 4 * ((h - i - 1) * w + j);
            fprintf(f, "%3d %3d %3d ", screenshot[cur], screenshot[cur + 1], screenshot[cur + 2]);
        }
        fprintf(f, "\n");
    }
    fclose(f);
}

void MainWindow::saveScreenShotPNG(const char *filename)
{
    p_viewer->makeCurrent();
    p_viewer->updateScreenShot();
    unsigned char *screenshot = p_viewer->screenshot;
    int h = p_viewer->height();
    int w = p_viewer->width();

//    QImage im(screenshot, w, h, QImage::Format_ARGB32);
    QImage im(screenshot, w, h, QImage::Format_RGB32);

    int len = im.bytesPerLine();
    unsigned char *im_line = new unsigned char[len];
    for (int y = 0; y < h/2; y++)
    {
        memcpy(im_line, im.scanLine(y), len);
        memcpy(im.scanLine(y), im.scanLine(h-y-1), len);
        memcpy(im.scanLine(h-y-1), im_line, len);
    }
    QImage im_rgba = im.rgbSwapped();
    im_rgba.save(filename, "PNG", 100);

    delete im_line;
}

//==============================================================================================
void MainWindow::on_pushButton_Observation_Show_clicked()
{
    p_viewer->showObservations = !p_viewer->showObservations;
    if (p_viewer->showObservations)
    {
        p_viewer->selection_choice = observation;
        ui->pushButton_Observation_Show->setText("Hide");
    }
    else
    {
        ui->pushButton_Observation_Show->setText("Show");
    }
    p_viewer->updateGL();
}

void MainWindow::on_pushButton_Observation_Add_clicked()
{
    if (traffic_system_p)
    {
        // create a observation point
        traffic_system_p->createObservationPoint();

        // create a local monitor
        LocalMonitor *p_moni = new LocalMonitor(this);
        local_monitor_list.push_back(p_moni);
        ui->verticalLayout_local_monitors->addWidget(p_moni);

        // make connection of closing monitor events.
        connect(p_moni, SIGNAL(ClosureHit()), this, SLOT(clearBadLocalMonitors()));

        int last_id = traffic_system_p->observation_list.size()-1;
        p_moni->setObservationPoint(traffic_system_p->observation_list[last_id]);
        p_moni->updateGL();

        p_viewer->showObservations = true;
        p_viewer->selection_choice = observation;
        ui->pushButton_Observation_Show->setText("Hide");
        p_viewer->updateGL();
    }
}

void MainWindow::on_pushButton_Observation_Delete_clicked()
{
    if (traffic_system_p && local_monitor_list.size() > 0)
    {
        // delete the last observation point
        int last_id = traffic_system_p->observation_list.size()-1;
        traffic_system_p->deleteObservationPoint(traffic_system_p->observation_list[last_id]);

        // delete the last local monitor
        LocalMonitor *p_moni = local_monitor_list[last_id];
        ui->verticalLayout_local_monitors->removeWidget(p_moni);
        delete p_moni;
        local_monitor_list.pop_back();

        if (p_viewer->showObservations)
        {
            p_viewer->updateGL();
        }
    }
}

void MainWindow::on_pushButton_Observation_Cancel_clicked()
{

}

void MainWindow::updateLocalMonitors()
{
    int num_observation = local_monitor_list.size();
    for (int i=0; i<num_observation; ++i)
    {
        LocalMonitor *p_moni = local_monitor_list[i];
        p_moni->stepObservationPoint();
        p_moni->updateGL();
    }
}

void MainWindow::clearBadLocalMonitors()
{
    for (vector<LocalMonitor*>::iterator it = local_monitor_list.begin();
         it != local_monitor_list.end(); ++it)
    {
        if ((*it)->isBad)
        {
            // delete the corresponding point
            traffic_system_p->deleteObservationPoint((*it)->p_observation_point);

            // delete the local monitor from the layout
            ui->verticalLayout_local_monitors->removeWidget(*it);

            // delete and destroy the local monitor
            if ((*it) != local_monitor_list.back())
            {
                swap(*it, local_monitor_list.back());
                delete local_monitor_list.back();
                local_monitor_list.pop_back();
            }
            else
            {
                delete local_monitor_list.back();
                local_monitor_list.pop_back();
                break;
            }
        }
    }

    p_viewer->updateGL();
}

//==============================================================================================
void MainWindow::on_pushButton_Blocks_Show_clicked()
{
    p_viewer->showBlocks = !p_viewer->showBlocks;

    if (p_viewer->showBlocks)
    {
        p_viewer->selection_choice = block;
        ui->pushButton_Blocks_Show->setText("Hide");
    }
    else
    {
        ui->pushButton_Blocks_Show->setText("Show");
        p_viewer->updateBlockArea();
    }

    p_viewer->updateGL();
}

void MainWindow::on_pushButton_Blocks_Add_clicked()
{
    if (traffic_system_p)
    {
        p_viewer->showBlocks = true;
        p_viewer->selection_choice = block;

        ui->pushButton_Blocks_Show->setText("Hide");
        p_viewer->addNewBlockArea();
        p_viewer->updateGL();
    }
}

void MainWindow::on_pushButton_Blocks_Delete_clicked()
{

}

void MainWindow::on_pushButton_Blocks_Cancel_clicked()
{
    if (traffic_system_p)
    {
        if (p_viewer->showBlocks)
        {
            // discard setting the block
            p_viewer->restoreBlockArea();
            p_viewer->setMouseTracking(false);
            p_viewer->updateGL();
        }
    }
}

//==============================================================================================
void MainWindow::on_pushButton_Beziers_Show_clicked()
{
    if (traffic_system_p)
    {
        p_viewer->showBeziers = !p_viewer->showBeziers;
        if (p_viewer->showBeziers)
        {
            p_viewer->selection_choice = spline;
            ui->pushButton_Beziers_Show->setText("Hide");
            p_viewer->backupSplines();
        }
        else
        {
            ui->pushButton_Beziers_Show->setText("Show");
            p_viewer->updateSplines();
        }
        p_viewer->updateGL();
    }
}

void MainWindow::on_pushButton_Beziers_Add_clicked()
{
//    if (traffic_system_p)
//    {
//        p_viewer->display_mode_S = 1;
//        ui->pushButton_Beziers_Show->setText("Hide");
//        p_viewer->updateGL();
//    }
}

void MainWindow::on_pushButton_Beziers_Delete_clicked()
{

}

void MainWindow::on_pushButton_Beziers_Cancel_clicked()
{
    if (traffic_system_p)
    {
        if (p_viewer->showBeziers)
        {
            // discard setting the spline
            p_viewer->restoreSplines();
            p_viewer->setMouseTracking(false);
            p_viewer->updateGL();
        }
    }
}

void MainWindow::on_pushButtonSnapshot_SaveCamera_clicked()
{
    QDir::setCurrent(project_directory+"/default");
    QString filename = QFileDialog::getSaveFileName
            (this, tr("Save the camera "), ".", tr("*.txt"));

    if (!filename.length() || !traffic_system_p)
    {
        return;
    }

    // open the file
    ofstream ofs(filename.toLatin1());

    // get and save the matrices
    double m_modelview[16], m_projection[16];
    p_viewer->camera()->getModelViewMatrix(m_modelview);
    p_viewer->camera()->getProjectionMatrix(m_projection);

    for (int j=0; j<16; ++j)
    {
        ofs << m_modelview[j] << " ";
    }
    ofs << "\r\n";
    for (int j=0; j<16; ++j)
    {
        ofs << m_projection[j] << " ";
    }
    ofs << "\r\n";
    ofs.close();
}

void MainWindow::on_pushButtonSnapshot_LoadCamera_clicked()
{
    QDir::setCurrent(project_directory+"/default");
    QString filename = QFileDialog::getOpenFileName
            (this, tr("Load a camera "), ".", tr("*.txt"));

    if (!filename.length() || !traffic_system_p)
    {
        return;
    }

    // open the file
    ifstream ifs(filename.toLatin1());

    // get and save the matrices
    double m_modelview[16], m_projection[16];

    char line[1024];
    char delimiters[] = " \"(),\r\n";
    char* token;
    bool flag_good_data = true;

    ifs.getline(line, 1024);
    token = strtok(line, delimiters);
    for (int j=0; j<16; ++j)
    {
        if (!token)
        {
            flag_good_data = false;
            break;
        }
        m_modelview[j] = atof(token);
        token = strtok(NULL, delimiters);
    }

    ifs.getline(line, 1024);
    token = strtok(line, delimiters);
    for (int j=0; j<16; ++j)
    {
        if (!token)
        {
            flag_good_data = false;
            break;
        }
        m_projection[j] = atof(token);
        token = strtok(NULL, delimiters);
    }
    ifs.close();

    if (flag_good_data)
    {
        p_viewer->camera()->setFromModelViewMatrix(m_modelview);
//        p_viewer->camera()->setFieldOfView(2.0 * atan(1.0/m_projection[5]));
        p_viewer->updateGL();
    }
    else
    {
        cout << "Loading camera failed!\n";
    }
}

//===================================================================================================
void MainWindow::on_radioButton_ShowMainDensity_clicked(bool checked)
{
    if (traffic_system_p)
    {
        if (checked)
        {
            p_viewer->display_mode_D = 1;
            p_viewer->updateGL();

            traffic_system_p->isUpdatingFieldMeshes = true;
        }
    }
}

void MainWindow::on_radioButton_ShowMainVelocity_clicked(bool checked)
{
    if (traffic_system_p)
    {
        if (checked)
        {
            p_viewer->display_mode_D = 2;
            p_viewer->updateGL();

            traffic_system_p->isUpdatingFieldMeshes = true;
        }
    }
}

void MainWindow::on_radioButton_ShowMainDensityVelocity_Nothing_clicked(bool checked)
{
    if (traffic_system_p)
    {
        if (checked)
        {
            p_viewer->display_mode_D = 0;
            p_viewer->updateGL();

            traffic_system_p->isUpdatingFieldMeshes = false;
        }
    }
}

void MainWindow::on_radioButton_ShowParticles_clicked(bool checked)
{
    if (traffic_system_p)
    {
        if (checked)
        {
            traffic_system_p->isUpdatingParticles = true;
            traffic_system_p->isUpdatingCars = false;

            p_viewer->display_mode_P = 1;
            p_viewer->selection_choice = particle;
            p_viewer->updateGL();
        }
    }
}

void MainWindow::on_radioButton_ShowVehicles_clicked(bool checked)
{
    if (traffic_system_p)
    {
        if (checked)
        {
            traffic_system_p->isUpdatingCars = true;
//            traffic_system_p->synchronizeCars();

            traffic_system_p->isUpdatingParticles = false;

            p_viewer->display_mode_P = 2;
            p_viewer->updateGL();
        }
    }
}

void MainWindow::on_radioButton_ShowVehicles_Bbox_clicked(bool checked)
{
    if (traffic_system_p)
    {
        if (checked)
        {
            traffic_system_p->isUpdatingCars = true;
            traffic_system_p->isUpdatingParticles = false;

            p_viewer->display_mode_P = 3;
            p_viewer->updateGL();
        }
    }
}

void MainWindow::on_radioButton_ShowParticlesVehicles_Nothing_clicked(bool checked)
{
    if (traffic_system_p)
    {
        if (checked)
        {
            traffic_system_p->isUpdatingParticles = false;
            traffic_system_p->isUpdatingCars = false;

            p_viewer->display_mode_P = 0;
            p_viewer->updateGL();
        }
    }
}

//===================================================================================================
void MainWindow::on_checkBox_ShowReferencePlanes_clicked(bool checked)
{
    p_viewer->showReferencePlanes = checked;
    p_viewer->updateGL();
}

void MainWindow::on_checkBox_ShowCells_clicked(bool checked)
{
    p_viewer->showRoadBoundaryCells = checked;
    p_viewer->updateGL();
}

void MainWindow::on_checkBox_ShowRoad_Facilities_clicked(bool checked)
{
    p_viewer->showRoadFacilities = checked;
    p_viewer->updateGL();
}

void MainWindow::on_checkBox_ShowParticle_NormalTangent_clicked(bool checked)
{
    p_viewer->showParticleNormalTangent = checked;
    p_viewer->updateGL();
}

void MainWindow::on_checkBox_ShowParticle_Force_clicked(bool checked)
{
    p_viewer->showParticleForce = checked;
    p_viewer->updateGL();
}

void MainWindow::on_checkBox_ShowParticle_Velocity_Direction_clicked(bool checked)
{
    p_viewer->showParticleVelocityDirection = checked;
    p_viewer->updateGL();
}

void MainWindow::on_checkBox_ShowVehicle_Force_clicked(bool checked)
{
    p_viewer->showCarSpringForce = checked;
    p_viewer->updateGL();
}

void MainWindow::on_checkBox_ShowVehicle_Velocity_Direction_clicked(bool checked)
{
    p_viewer->showCarVelocityDirection = checked;
    p_viewer->updateGL();
}

void MainWindow::on_checkBox_ShowVehicle_Tail_clicked(bool checked)
{
    p_viewer->showCarTail = checked;
    p_viewer->updateGL();
}

void MainWindow::on_checkBox_ShowVehicle_Trajectory_clicked(bool checked)
{
    p_viewer->showCarTrajectory = checked;
    p_viewer->updateGL();
}

//==============================================================================================
void MainWindow::on_pushButton_Selection_Particle_clicked(bool checked)
{
    if (traffic_system_p && checked)
    {
        p_viewer->selection_choice = particle;
    }
}

void MainWindow::on_pushButton_Selection_Observation_clicked(bool checked)
{
    if (traffic_system_p && checked)
    {
        p_viewer->selection_choice = observation;
    }
}

void MainWindow::on_pushButton_Selection_Spline_clicked(bool checked)
{
    if (traffic_system_p && checked)
    {
        p_viewer->selection_choice = spline;
    }
}

void MainWindow::on_pushButton_Selection_Block_clicked(bool checked)
{
    if (traffic_system_p && checked)
    {
        p_viewer->selection_choice = block;
    }
}

void MainWindow::on_pushButton_Selection_Road_clicked(bool checked)
{
    if (traffic_system_p && checked)
    {
        p_viewer->selection_choice = road;
    }
}

//==============================================================================================
void MainWindow::on_checkBox_MapElements_Buildings_clicked(bool checked)
{
    if (traffic_system_p)
    {
        vector<RoadLink*> *p_roadlink_list =
                &traffic_system_p->model_parameter->real_map->roadlink_list;
        int num_roadlink = p_roadlink_list->size();
        for (int k=0; k<num_roadlink; ++k)
        {
            RoadLink *p_roadlink = p_roadlink_list->at(k);
            if (p_roadlink->link_type == building)
            {
                p_roadlink->isVisible = checked;
            }
        }
        p_viewer->updateGL();
    }
}

void MainWindow::on_checkBox_MapElements_Railways_clicked(bool checked)
{
    if (traffic_system_p)
    {
        vector<RoadLink*> *p_roadlink_list =
                &traffic_system_p->model_parameter->real_map->roadlink_list;
        int num_roadlink = p_roadlink_list->size();
        for (int k=0; k<num_roadlink; ++k)
        {
            RoadLink *p_roadlink = p_roadlink_list->at(k);
            if (p_roadlink->link_type == railway)
            {
                p_roadlink->isVisible = checked;
            }
        }
        p_viewer->updateGL();
    }
}

void MainWindow::on_checkBox_MapElements_Highways_clicked(bool checked)
{
    if (traffic_system_p)
    {
        // filter the map
        vector<RoadLink*> *p_roadlink_list =
                &traffic_system_p->model_parameter->real_map->roadlink_list;
        int num_roadlink = p_roadlink_list->size();
        for (int k=0; k<num_roadlink; ++k)
        {
            RoadLink *p_roadlink = p_roadlink_list->at(k);
            if (p_roadlink->link_type == highway)
            {
                p_roadlink->isVisible = checked;
            }
        }

        // filter the road net
        int num_road = traffic_system_p->road_list.size();
        for (int i=0; i<num_road; ++i)
        {
            Road *p_road = traffic_system_p->road_list[i];
            p_road->isVisible = checked;
        }
        p_viewer->updateGL();

        // modified the highway view options
        ui->checkBox_RoadNet_Trunk->setChecked(checked);
        ui->checkBox_RoadNet_Motorway->setChecked(checked);
        ui->checkBox_RoadNet_Primary->setChecked(checked);
        ui->checkBox_RoadNet_Secondary->setChecked(checked);
        ui->checkBox_RoadNet_Tertiary->setChecked(checked);
        ui->checkBox_RoadNet_Residential->setChecked(checked);
        ui->checkBox_RoadNet_Service->setChecked(checked);
        ui->checkBox_RoadNet_Footway->setChecked(checked);
        ui->checkBox_RoadNet_Unclassified->setChecked(checked);
        ui->checkBox_RoadNet_Path->setChecked(checked);
        ui->checkBox_RoadNet_Oneway->setChecked(checked);
        ui->checkBox_RoadNet_Others->setChecked(checked);
    }
}

void MainWindow::on_checkBox_MapElements_Others_clicked(bool checked)
{
    if (traffic_system_p)
    {
        vector<RoadLink*> *p_roadlink_list =
                &traffic_system_p->model_parameter->real_map->roadlink_list;
        int num_roadlink = p_roadlink_list->size();
        for (int k=0; k<num_roadlink; ++k)
        {
            RoadLink *p_roadlink = p_roadlink_list->at(k);
            if (p_roadlink->link_type == otherlinks)
            {
                p_roadlink->isVisible = checked;
            }
        }
        p_viewer->updateGL();
    }
}

void MainWindow::on_checkBox_MapElements_BoundingBox_clicked(bool checked)
{
    if (traffic_system_p)
    {
        p_viewer->showRoadBoundingBox = checked;
        p_viewer->updateGL();
    }
}

//==============================================================================================
void MainWindow::on_checkBox_RoadNet_Trunk_clicked(bool checked)
{
    if (traffic_system_p)
    {
        // filter the map
        vector<RoadLink*> *roadlink_list = &traffic_system_p->model_parameter->real_map->roadlink_list;
        int num_link = roadlink_list->size();
        for (int i=0; i<num_link; ++i)
        {
            RoadLink *p_roadlink = roadlink_list->at(i);
            if (p_roadlink->link_type == highway &&
               (p_roadlink->highway_type == trunk || p_roadlink->highway_type == trunk_link))
            {
                p_roadlink->isVisible = checked;
            }
        }

        // filter the road net
        int num_road = traffic_system_p->road_list.size();
        for (int i=0; i<num_road; ++i)
        {
            Road *p_road = traffic_system_p->road_list[i];
            if (p_road->roadsubtype == trunk || p_road->roadsubtype == trunk_link)
            {
                p_road->isVisible = checked;
            }
        }
        p_viewer->updateGL();
    }
}

void MainWindow::on_checkBox_RoadNet_Motorway_clicked(bool checked)
{
    if (traffic_system_p)
    {
        // filter the map
        vector<RoadLink*> *roadlink_list = &traffic_system_p->model_parameter->real_map->roadlink_list;
        int num_link = roadlink_list->size();
        for (int i=0; i<num_link; ++i)
        {
            RoadLink *p_roadlink = roadlink_list->at(i);
            if (p_roadlink->link_type == highway &&
               (p_roadlink->highway_type == motorway || p_roadlink->highway_type == motorway_link))
            {
                p_roadlink->isVisible = checked;
            }
        }

        // filter the road net
        int num_road = traffic_system_p->road_list.size();
        for (int i=0; i<num_road; ++i)
        {
            Road *p_road = traffic_system_p->road_list[i];
            if (p_road->roadsubtype == motorway || p_road->roadsubtype == motorway_link)
            {
                p_road->isVisible = checked;
            }
        }
        p_viewer->updateGL();
    }
}

void MainWindow::on_checkBox_RoadNet_Primary_clicked(bool checked)
{
    if (traffic_system_p)
    {
        // filter the map
        vector<RoadLink*> *roadlink_list = &traffic_system_p->model_parameter->real_map->roadlink_list;
        int num_link = roadlink_list->size();
        for (int i=0; i<num_link; ++i)
        {
            RoadLink *p_roadlink = roadlink_list->at(i);
            if (p_roadlink->link_type == highway &&
               (p_roadlink->highway_type == primary || p_roadlink->highway_type == primary_link))
            {
                p_roadlink->isVisible = checked;
            }
        }

        // filter the road net
        int num_road = traffic_system_p->road_list.size();
        for (int i=0; i<num_road; ++i)
        {
            Road *p_road = traffic_system_p->road_list[i];
            if (p_road->roadsubtype == primary || p_road->roadsubtype == primary_link)
            {
                p_road->isVisible = checked;
            }
        }
        p_viewer->updateGL();
    }
}

void MainWindow::on_checkBox_RoadNet_Secondary_clicked(bool checked)
{
    if (traffic_system_p)
    {
        // filter the map
        vector<RoadLink*> *roadlink_list = &traffic_system_p->model_parameter->real_map->roadlink_list;
        int num_link = roadlink_list->size();
        for (int i=0; i<num_link; ++i)
        {
            RoadLink *p_roadlink = roadlink_list->at(i);
            if (p_roadlink->link_type == highway && p_roadlink->highway_type == secondary)
            {
                p_roadlink->isVisible = checked;
            }
        }

        // filter the road net
        int num_road = traffic_system_p->road_list.size();
        for (int i=0; i<num_road; ++i)
        {
            Road *p_road = traffic_system_p->road_list[i];
            if (p_road->roadsubtype == secondary)
            {
                p_road->isVisible = checked;
            }
        }
        p_viewer->updateGL();
    }
}

void MainWindow::on_checkBox_RoadNet_Tertiary_clicked(bool checked)
{
    if (traffic_system_p)
    {
        // filter the map
        vector<RoadLink*> *roadlink_list = &traffic_system_p->model_parameter->real_map->roadlink_list;
        int num_link = roadlink_list->size();
        for (int i=0; i<num_link; ++i)
        {
            RoadLink *p_roadlink = roadlink_list->at(i);
            if (p_roadlink->link_type == highway && p_roadlink->highway_type == tertiary)
            {
                p_roadlink->isVisible = checked;
            }
        }

        // filter the road net
        int num_road = traffic_system_p->road_list.size();
        for (int i=0; i<num_road; ++i)
        {
            Road *p_road = traffic_system_p->road_list[i];
            if (p_road->roadsubtype == tertiary)
            {
                p_road->isVisible = checked;
            }
        }
        p_viewer->updateGL();
    }
}

void MainWindow::on_checkBox_RoadNet_Residential_clicked(bool checked)
{
    if (traffic_system_p)
    {
        // filter the map
        vector<RoadLink*> *roadlink_list = &traffic_system_p->model_parameter->real_map->roadlink_list;
        int num_link = roadlink_list->size();
        for (int i=0; i<num_link; ++i)
        {
            RoadLink *p_roadlink = roadlink_list->at(i);
            if (p_roadlink->link_type == highway && p_roadlink->highway_type == residential)
            {
                p_roadlink->isVisible = checked;
            }
        }

        // filter the road net
        int num_road = traffic_system_p->road_list.size();
        for (int i=0; i<num_road; ++i)
        {
            Road *p_road = traffic_system_p->road_list[i];
            if (p_road->roadsubtype == residential)
            {
                p_road->isVisible = checked;
            }
        }
        p_viewer->updateGL();
    }
}

void MainWindow::on_checkBox_RoadNet_Service_clicked(bool checked)
{
    if (traffic_system_p)
    {
        // filter the map
        vector<RoadLink*> *roadlink_list = &traffic_system_p->model_parameter->real_map->roadlink_list;
        int num_link = roadlink_list->size();
        for (int i=0; i<num_link; ++i)
        {
            RoadLink *p_roadlink = roadlink_list->at(i);
            if (p_roadlink->link_type == highway && p_roadlink->highway_type == service)
            {
                p_roadlink->isVisible = checked;
            }
        }

        // filter the road net
        int num_road = traffic_system_p->road_list.size();
        for (int i=0; i<num_road; ++i)
        {
            Road *p_road = traffic_system_p->road_list[i];
            if (p_road->roadsubtype == service)
            {
                p_road->isVisible = checked;
            }
        }
        p_viewer->updateGL();
    }
}

void MainWindow::on_checkBox_RoadNet_Footway_clicked(bool checked)
{
    if (traffic_system_p)
    {
        // filter the map
        vector<RoadLink*> *roadlink_list = &traffic_system_p->model_parameter->real_map->roadlink_list;
        int num_link = roadlink_list->size();
        for (int i=0; i<num_link; ++i)
        {
            RoadLink *p_roadlink = roadlink_list->at(i);
            if (p_roadlink->link_type == highway && p_roadlink->highway_type == footway)
            {
                p_roadlink->isVisible = checked;
            }
        }

        // filter the road net
        int num_road = traffic_system_p->road_list.size();
        for (int i=0; i<num_road; ++i)
        {
            Road *p_road = traffic_system_p->road_list[i];
            if (p_road->roadsubtype == footway)
            {
                p_road->isVisible = checked;
            }
        }
        p_viewer->updateGL();
    }
}

void MainWindow::on_checkBox_RoadNet_Unclassified_clicked(bool checked)
{
    if (traffic_system_p)
    {
        // filter the map
        vector<RoadLink*> *roadlink_list = &traffic_system_p->model_parameter->real_map->roadlink_list;
        int num_link = roadlink_list->size();
        for (int i=0; i<num_link; ++i)
        {
            RoadLink *p_roadlink = roadlink_list->at(i);
            if (p_roadlink->link_type == highway && p_roadlink->highway_type == unclassified)
            {
                p_roadlink->isVisible = checked;
            }
        }

        // filter the road net
        int num_road = traffic_system_p->road_list.size();
        for (int i=0; i<num_road; ++i)
        {
            Road *p_road = traffic_system_p->road_list[i];
            if (p_road->roadsubtype == unclassified)
            {
                p_road->isVisible = checked;
            }
        }
        p_viewer->updateGL();
    }
}

void MainWindow::on_checkBox_RoadNet_Path_clicked(bool checked)
{
    if (traffic_system_p)
    {
        // filter the map
        vector<RoadLink*> *roadlink_list = &traffic_system_p->model_parameter->real_map->roadlink_list;
        int num_link = roadlink_list->size();
        for (int i=0; i<num_link; ++i)
        {
            RoadLink *p_roadlink = roadlink_list->at(i);
            if (p_roadlink->link_type == highway && p_roadlink->highway_type == path)
            {
                p_roadlink->isVisible = checked;
            }
        }

        // filter the road net
        int num_road = traffic_system_p->road_list.size();
        for (int i=0; i<num_road; ++i)
        {
            Road *p_road = traffic_system_p->road_list[i];
            if (p_road->roadsubtype == path)
            {
                p_road->isVisible = checked;
            }
        }
        p_viewer->updateGL();
    }
}

void MainWindow::on_checkBox_RoadNet_Oneway_clicked(bool checked)
{
    if (traffic_system_p)
    {
        // filter the map
        vector<RoadLink*> *roadlink_list = &traffic_system_p->model_parameter->real_map->roadlink_list;
        int num_link = roadlink_list->size();
        for (int i=0; i<num_link; ++i)
        {
            RoadLink *p_roadlink = roadlink_list->at(i);
            if (p_roadlink->isOneway)
            {
                p_roadlink->isVisible = checked;
            }
        }

        // filter the road net
        int num_road = traffic_system_p->road_list.size();
        for (int i=0; i<num_road; ++i)
        {
            Road *p_road = traffic_system_p->road_list[i];
            if (p_road->isOneway)
            {
                p_road->isVisible = checked;
            }
        }
        p_viewer->updateGL();
    }
}

void MainWindow::on_checkBox_RoadNet_Others_clicked(bool checked)
{
    if (traffic_system_p)
    {
        // filter the map
        vector<RoadLink*> *roadlink_list = &traffic_system_p->model_parameter->real_map->roadlink_list;
        int num_link = roadlink_list->size();
        for (int i=0; i<num_link; ++i)
        {
            RoadLink *p_roadlink = roadlink_list->at(i);
            if (p_roadlink->link_type == highway && p_roadlink->highway_type == otherhighways)
            {
                p_roadlink->isVisible = checked;
            }
        }

        // filter the road net
        int num_road = traffic_system_p->road_list.size();
        for (int i=0; i<num_road; ++i)
        {
            Road *p_road = traffic_system_p->road_list[i];
            if (p_road->roadsubtype == otherhighways)
            {
                p_road->isVisible = checked;
            }
        }
        p_viewer->updateGL();
    }
}

void MainWindow::on_tabWidget_tabBarClicked(int index)
{
    if (traffic_system_p)
    {
        if (index == 0)
        {
            traffic_system_p->isUpdatingFieldMeshes = true;
        }
        else
        {
            traffic_system_p->isUpdatingFieldMeshes =
                    !ui->radioButton_ShowMainDensityVelocity_Nothing->isChecked();
        }

        if (index == 1)
        {
            isShowingLocalMoniters = true;
        }
        else
        {
            isShowingLocalMoniters = false;
        }
    }
}
