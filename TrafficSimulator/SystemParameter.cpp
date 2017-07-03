#include "SystemParameter.h"

SystemParameter::SystemParameter(const char *config_filename)
{
    //=========================================================================================
    // Default Setting
    // simulation parameters
    DeltaTime             = 0.025; //0.025
    ParticleMass          = 1; //10.0  // one vehicle means ten particles
    ParticleRadius        = 0.5; // meter
    SupportRadius         = DEFAULT_VEHICLE_LENGTH * 2;
    SupportRadiusHK       = SupportRadius;
    MaxDensity            = 10;  //vpm: vehicles per mile
    MaxVelocity           = 40;  //mph: miles per hour
    UseHalfKernel         = false;

    LaneChangingDensityGap   = 0.2;
    LaneChangingVelocityGap  = 0.8;
    RestDensity           = 20;

    GasStiffness      = 10;
    Viscosity         = 10;
    CoeffDampingForce = 4;
    CoeffFieldForce   = 100;

    UseViscosityForce = true;
    UsePressureForce  = true;
    UseExternalForce  = true;
    AddConstrain      = false;

    // output file options
    OutputCellSensorDensityVelocity = false;
    OutputEntireDensityVelocity = false;
    OutputDensityVelocityMesh = false;
    OutputScreenshot = false;

    // default is trajectory following
    SimulationMode = 0;
    ExperimentType = 0;

    SuccessfulInit = true;

    // Initialize the meshes and color lists
    cout << "===========================================================================\n"
         << "Load meshes and colors ... \n" << std::flush;
    QElapsedTimer ss_timer;
    ss_timer.start();
    loadMeshesAndColors();
    if (car_mesh_list.size() > 0)
    {
        cout << "Car meshes " << car_mesh_list.size() << endl;
    }
    else
    {
        cout << "Error! No car meshes! " << car_mesh_list.size() << endl;
        SuccessfulInit = false;
        return;
    }
    cout << "OK! " << ss_timer.nsecsElapsed()/1e9 << " seconds\n" << std::flush;

    // Initialize the map
    cout << "===========================================================================\n"
         << "Load map files ... \n" << std::flush;
    configuration_dir = QDir::currentPath().toStdString() + "/configuration/";
    string default_mapfilename = configuration_dir + "maps/default_map_oneroad.osm";
    mapfilename = "";

    // Load the config file
    ss_timer.start();
    loadConfigFile(config_filename);

    if (mapfilename == "beijing_china.osm")
    {
        // Beijing map is too big for GitHub and must be at other place out of the repository
        mapfilename = "C:/Users/Yuming Zhang/Documents/GitHub/" + mapfilename;
    }
    else
    {
        // Small maps
        mapfilename = configuration_dir + "maps/" + mapfilename;
    }

    real_map = new MapParser;
    if (real_map->loadByTinyXML2(mapfilename.c_str()))
    {
        cout << mapfilename << endl;
        cout << "OK! " << ss_timer.nsecsElapsed()/1e9 << " seconds\n" << std::flush;
    }
    else if (real_map->loadByTinyXML2(default_mapfilename.c_str()))
    {
        cout << default_mapfilename << endl;
        cout << "OK! " << ss_timer.nsecsElapsed()/1e9 << " seconds\n" << std::flush;
    }
    else
    {
        cout << "Failed!\n" << std::flush;
        delete real_map;
        real_map = NULL;
        SuccessfulInit = false;
        return;
    }

    if (SimulationMode == 9)
    {
        setExperimentParameters();
    }
}

void SystemParameter::loadMeshesAndColors()
{
    // circle mesh (radius = 1m)
    circle_mesh.buildMeshAsOneCircle(0.5, 16);

    // sphere mesh
    QString dir = QDir::currentPath();
    QString meshfilename = dir + "/meshes/sphere.obj";
    sphere_mesh.loadMeshFile(meshfilename.toLatin1());
    sphere_mesh.setOriginalMesh(ParticleRadius, 0.0,0.0,0.0);
//    cout << "#triangles of sphere = " << sphere_mesh.triangle_list.size() << endl;

    // block mesh
    meshfilename = dir + "/meshes/Lattice_Barrier_01.obj";
    block_mesh.loadMeshFile(meshfilename.toLatin1());
    block_mesh.setOriginalMesh(0.2,PI/2,0.0,0.0);
//    cout << "#triangles of Lattice_Barrier_01 = " << block_mesh.triangle_list.size() << endl;

    // car meshs
    vector<QString> car_names(10);
    car_names[0] = "83_camaro_4k";
    car_names[1] = "bus_6k";
    car_names[2] = "CBRed_2k";
    car_names[3] = "CheckerMarathon_5k";
    car_names[4] = "Datsun240Z_4k";
    car_names[5] = "FiatX1-9_2k";
    car_names[6] = "VWBus_3k";
    car_names[7] = "Y2918_Car_sports_utility_2_door_80s_3k";
    car_names[8] = "Y4122_g-klasse_2k";
    car_names[9] = "Y4129_pajero_3k";

    car_mesh_list.clear();
    for (int i=0; i<10; ++i)
    {
        TriangleMesh *p_car_mesh = new TriangleMesh;

        meshfilename = dir + "/meshes/" + car_names[i] + ".obj";
        p_car_mesh->loadMeshFile(meshfilename.toLatin1());
        p_car_mesh->setOriginalMesh(0.3, PI/2, 0.0, 0.0);

        car_mesh_list.push_back(p_car_mesh);
    }

    // car color list
    car_color_list.clear();
    car_color_list.push_back(zVec4(0.3, 0.8, 0.4, 1.0));
    car_color_list.push_back(zVec4(0.3, 0.3, 0.9, 1.0));
    car_color_list.push_back(zVec4(0.8, 0.3, 0.2, 1.0));
    car_color_list.push_back(zVec4(0.9, 0.2, 0.3, 1.0));
    car_color_list.push_back(zVec4(0.8, 0.8, 0.2, 1.0));
    car_color_list.push_back(zVec4(0.5, 0.2, 0.8, 1.0));

    // observation color list
    observation_color_list.clear();
    observation_color_list.push_back(zVec4(1.0, 0.0, 0.0, 1.0));
    observation_color_list.push_back(zVec4(0.0, 1.0, 0.0, 1.0));
    observation_color_list.push_back(zVec4(0.0, 0.0, 1.0, 1.0));
    observation_color_list.push_back(zVec4(0.8, 0.6, 0.0, 1.0));
    observation_color_list.push_back(zVec4(0.0, 1.0, 1.0, 1.0));
    observation_color_list.push_back(zVec4(1.0, 0.0, 1.0, 1.0));
}

void SystemParameter::loadConfigFile(const char *filename)
{
    // open the obj file.
    ifstream ifs(filename);
    if (ifs.fail())
    {
        cout << "Config file opening failed" << endl;
        return;
    }

    lane_number_list.clear();
    bezier_array_list.clear();

    char line[1024];
    char delimiters[] = " \"(),\r\n";
    char* token;
    char commt[] = "#";
    char mode[] = "mode";
    char label_mapfile[] = "mapfile";
    char type[] = "t";
    char road[] = "r";
    char bezier[] = "b";
    char parameter[] = "p";
    char checkbox[] = "c";

    while (!ifs.eof())
    {
        ifs.getline(line, 1024);

        token = strtok(line, delimiters);

        if (strlen(line) == 0 || strcmp(token, commt) == 0)
        {
            continue;
        }

        // a line for the experiment type
        else if (strcmp (token, mode) == 0)
        {
            token = strtok(NULL, delimiters);
            SimulationMode = atof(token);
        }

        // a line for the experiment type
        else if (strcmp (token, type) == 0)
        {
            token = strtok(NULL, delimiters);
            ExperimentType = atof(token);
        }

        // a line for a map file
        else if (strcmp(token, label_mapfile) == 0 )
        {
            token = strtok(NULL, delimiters);
            mapfilename = string(token);
        }

        // a line for a road with number of lanes
        else if (strcmp(token, road) == 0 )
        {
            token = strtok(NULL, delimiters);
            lane_number_list.push_back(atof(token));

            vector<Bezier> bez_array;
            bezier_array_list.push_back(bez_array);
        }

        // a line of four bezier control point
        else if (strcmp(token, bezier) == 0 )
        {
            int last_road_id = bezier_array_list.size()-1;
            if (last_road_id == -1)
            {
                lane_number_list.push_back(4);
                vector<Bezier> bez_array;
                bezier_array_list.push_back(bez_array);
                last_road_id = 0;
            }

            vector<Bezier>* p_bez_arr = &bezier_array_list[last_road_id];
            PointArray pa(4);
            for (int i=0; i<4; ++i)
            {
                token = strtok(NULL, delimiters);
                pa[i].x = atof(token);
                token = strtok(NULL, delimiters);
                pa[i].y = atof(token);
            }

            p_bez_arr->push_back(Bezier(pa));
        }

        // a line of parameter
        else if (strcmp (token, parameter) == 0)
        {
            token = strtok(NULL, delimiters);
            if (strcmp (token, "GasStiffness") == 0)
            {
                token = strtok(NULL, delimiters);
                GasStiffness = atof(token);
            }
            else if (strcmp (token, "Viscosity") == 0)
            {
                token = strtok(NULL, delimiters);
                Viscosity = atof(token);
            }
            else if (strcmp (token, "CoeffDampingForce") == 0)
            {
                token = strtok(NULL, delimiters);
                CoeffDampingForce = atof(token);
            }
            else if (strcmp (token, "CoeffFieldForce") == 0)
            {
                token = strtok(NULL, delimiters);
                CoeffFieldForce = atof(token);
            }
            else if (strcmp (token, "MaxDensity") == 0)
            {
                token = strtok(NULL, delimiters);
                MaxDensity = atof(token);
            }
            else if (strcmp (token, "MaxVelocity") == 0)
            {
                token = strtok(NULL, delimiters);
                MaxVelocity = atof(token);
            }
        }

        // a line of switch/checkbox
        else if (strcmp (token, checkbox) == 0)
        {
            token = strtok(NULL, delimiters);
            if (strcmp (token, "UsePressureForce") == 0)
            {
                token = strtok(NULL, delimiters);
                if (atoi(token) == 0)
                {
                    UsePressureForce = false;
                }
                else
                {
                    UsePressureForce = true;
                }
            }
            else if (strcmp (token, "UseViscosityForce") == 0)
            {
                token = strtok(NULL, delimiters);
                if (atoi(token) == 0)
                {
                    UseViscosityForce = false;
                }
                else
                {
                    UseViscosityForce = true;
                }
            }
            else if (strcmp (token, "UseExternalForce") == 0)
            {
                token = strtok(NULL, delimiters);
                if (atoi(token) == 0)
                {
                    UseExternalForce = false;
                }
                else
                {
                    UseExternalForce = true;
                }
            }
            else if (strcmp (token, "AddConstrain") == 0)
            {
                token = strtok(NULL, delimiters);
                if (atoi(token) == 0)
                {
                    AddConstrain = false;
                }
                else
                {
                    AddConstrain = true;
                }
            }
            else if (strcmp (token, "OutputCellSensorDensityVelocity") == 0)
            {
                token = strtok(NULL, delimiters);
                if (atoi(token) == 0)
                {
                    OutputCellSensorDensityVelocity = false;
                }
                else
                {
                    OutputCellSensorDensityVelocity = true;
                }
            }
            else if (strcmp (token, "OutputEntireDensityVelocity") == 0)
            {
                token = strtok(NULL, delimiters);
                if (atoi(token) == 0)
                {
                    OutputEntireDensityVelocity = false;
                }
                else
                {
                    OutputEntireDensityVelocity = true;
                }
            }
            else if (strcmp (token, "OutputDensityVelocityMesh") == 0)
            {
                token = strtok(NULL, delimiters);
                if (atoi(token) == 0)
                {
                    OutputDensityVelocityMesh = false;
                }
                else
                {
                    OutputDensityVelocityMesh = true;
                }
            }
            else if (strcmp (token, "OutputScreenshot") == 0)
            {
                token = strtok(NULL, delimiters);
                if (atoi(token) == 0)
                {
                    OutputScreenshot = false;
                }
                else
                {
                    OutputScreenshot = true;
                }
            }
        }
    }
    ifs.close();

    return;
}

void SystemParameter::setExperimentParameters()
{
    // Experiment for the 2D pressure force
    if (ExperimentType == 21)
    {
        ParticleMass = 20;
        SupportRadius = DEFAULT_LANE_WIDTH * 2.5;
        SupportRadiusHK = SupportRadius;
        MaxDensity = 5.0;
        MaxVelocity = 10.0;

        GasStiffness = 80;

        UseViscosityForce = false;
        UseExternalForce = false;
        OutputScreenshot = true;
        UseHalfKernel = false;
    }

    // Experiment for the 2D viscosity force
    else if (ExperimentType == 22)
    {
        ParticleMass = 20;
        SupportRadius = DEFAULT_LANE_WIDTH * 4.5;
        SupportRadiusHK = SupportRadius;
        MaxDensity = 10.0;
        MaxVelocity = 5.0;

        Viscosity = 100;
        CoeffDampingForce = 0.0;

        UsePressureForce = false;
        UseExternalForce = false;
        OutputScreenshot = true;
        UseHalfKernel = false;
    }

    // Experiment for 2D congestion scenarios
    else if (ExperimentType == 23)
    {
        ParticleMass = 20.0;
        SupportRadius = DEFAULT_LANE_WIDTH * 2.5;
        SupportRadiusHK = SupportRadius;
        MaxDensity = 20.0;
        MaxVelocity = 30.0;

        GasStiffness = 60;
        Viscosity = 0.5;  //0.5
        CoeffDampingForce = 2.0;
        CoeffFieldForce = 25;

        UseHalfKernel = false;

        // set the initial inflow rate of road 0
        RoadLink *p_roadlink = real_map->roadlink_list[0];
        p_roadlink->InflowRate[0] = 4.0;
    }

    // Experiment for 2D fds stable
    else if (ExperimentType == 24)
    {
        ParticleMass = 20.0;
        SupportRadius = DEFAULT_LANE_WIDTH * 2.5;
        SupportRadiusHK = SupportRadius;
        MaxDensity = 60.0;
        MaxVelocity = 20.0;

        GasStiffness = 60;
        Viscosity = 0.5;
        CoeffDampingForce = 2;
        CoeffFieldForce   = 40;

        UseHalfKernel = false;

        // set the inflow rate of road 0
        RoadLink *p_roadlink = real_map->roadlink_list[0];
        p_roadlink->InflowRate[0] = 1.0;
    }

    // Experiment for 1D congestion scenarios
    else if (ExperimentType == 13)
    {
        ParticleMass = 10.0;
        SupportRadius = DEFAULT_LANE_WIDTH * 10;
        SupportRadiusHK = SupportRadius;
        MaxDensity = 60.0;
        MaxVelocity = 20.0;

        GasStiffness = 6;
        Viscosity = 10000;
        CoeffDampingForce = 2;
        CoeffFieldForce   = 40;

        UseHalfKernel = true;

        // set the initial inflow rate of road 0
        RoadLink *p_roadlink = real_map->roadlink_list[0];
        p_roadlink->InflowRate[0] = 4.0;
    }

    // Experiment for 1D fds stable
    else if (ExperimentType == 14)
    {
        DeltaTime = 0.025;
        ParticleMass = 40.0;//10
        SupportRadius = DEFAULT_LANE_WIDTH * 10.0;
        SupportRadiusHK = SupportRadius;
        MaxDensity = 60.0;
        MaxVelocity = 30.0;

        GasStiffness = 6.0;
        Viscosity = 10000; //10000
        CoeffDampingForce = 2;
        CoeffFieldForce   = 60;

        UseHalfKernel = true;

        // set the initial inflow rate of road 0
        RoadLink *p_roadlink = real_map->roadlink_list[0];
        p_roadlink->InflowRate[0] = 0.1;
    }

    // Experiment for 1D accident congestion forming then releasing
    else if (ExperimentType == 15)
    {
        ParticleMass = 10.0;
        SupportRadius = DEFAULT_LANE_WIDTH * 10;
        SupportRadiusHK = SupportRadius;
        MaxDensity = 60.0;
        MaxVelocity = 20.0;

        GasStiffness = 6;
        Viscosity = 10000;
        CoeffDampingForce = 2;
        CoeffFieldForce   = 40;

        UseHalfKernel = true;

        // set the initial inflow rate of road 0
        RoadLink *p_roadlink = real_map->roadlink_list[0];
        p_roadlink->InflowRate[0] = 4.0;
    }
}

