#include "ParticleSystemTrafficTrj.h"

ParticleSystemTrafficTrj::ParticleSystemTrafficTrj(SystemParameter *model_parameter_)
    : ParticleSystemTraffic1D(model_parameter_)
{
    // set the reference time to 2000-1-1 00:00:00
    struct tm rt = {0};
    rt.tm_year = 100; rt.tm_mon = 0; rt.tm_mday = 1;
    rt.tm_hour = 0;   rt.tm_min = 0; rt.tm_sec = 0;
    reference_time = mktime(&rt);

    // set the simulation start time and position
    start_position.clear(); //(0,0)
    start_timestamp = "2015-11-01 23:50:00";
    sprintf(simulation_timestamp, start_timestamp.c_str());
    simulation_time = convertTimestamp(string(simulation_timestamp));
}

void ParticleSystemTrafficTrj::initSystem()
{
    buildTrajectoriesByTaxiFiles();

    cout << "===========================================================================\n"
         << "Running on mode 9 : trajectories following.\n" << std::flush;
}

void ParticleSystemTrafficTrj::step()
{
    for (int i=0; i<num_cars; ++i)
    {
        CarParticle *p_car = car_list[i];
        int lastid = p_car->taxinode_list.size()-1;

        while (1)
        {
            TaxiNode *p_taxinode = p_car->taxinode_list[p_car->current_taxinode];
            if (p_taxinode->timemark >= simulation_time)
            {
                p_car->position = p_taxinode->position;
                break;
            }

            if (p_car->current_taxinode >= lastid)
            {
                break;
            }
            else
            {
                p_car->current_taxinode++;
            }
        }
    }

    simulation_time += model_parameter->DeltaTime * 100;

    time_t seconds(simulation_time);
    struct tm *ts = gmtime(&seconds);
    sprintf(simulation_timestamp, "%04i-%02i-%02i %02i:%02i:%02i",
            ts->tm_year+1930, ts->tm_mon+1, ts->tm_mday, ts->tm_hour, ts->tm_min, ts->tm_sec);
}

void ParticleSystemTrafficTrj::reset()
{
    // reset cars to the initial positions
    for (int i=0; i<num_cars; ++i)
    {
        CarParticle *p_car = car_list[i];
        p_car->current_taxinode = 0;
        p_car->position = start_position;
    }

    // reset the simulation time
    sprintf(simulation_timestamp, start_timestamp.c_str());
    simulation_time = convertTimestamp(string(simulation_timestamp));
}

void ParticleSystemTrafficTrj::buildTrajectoriesByTaxiFiles()
{
    cout << "===========================================================================\n"
         << "Build taxi trajectories ... \n" << std::flush;

    // Get the taxi files
    string taxi_dir = model_parameter->configuration_dir+"taxis/";
    QDir taxifiles_dir(QString::fromStdString(taxi_dir));
    QStringList filters;
    filters << "*.csv";
    QStringList taxifiles_list = taxifiles_dir.entryList(filters);

    // Load and parse the files
    QElapsedTimer loadtaxidata_timer;
    loadtaxidata_timer.start();

    long long count_goodrecord = 0, count_badrecord = 0;
    int num_files = taxifiles_list.size();
    for (int i=0; i<num_files; ++i)
    {
        string taxifilename = taxi_dir + taxifiles_list[i].toStdString();
        ifstream ifs(taxifilename.c_str());

        char line[1024];
        char delimiters[] = ",";
        char* token;
        while (!ifs.eof())
        {
            TaxiNode *p_taxinode = new TaxiNode;
            bool flag_good_record = true;

            ifs.getline(line, 1024);
            token = strtok(line, delimiters);

            // company name
            if (token)
            {
                p_taxinode->company_name = string(token);
                token = strtok(NULL, delimiters);
            }
            else
            {
                flag_good_record = false;
            }

            // id
            if (token)
            {
                p_taxinode->id = atoll(token);
                token = strtok(NULL, delimiters);
            }
            else
            {
                flag_good_record = false;
            }

            // plate number
            if (token)
            {
                p_taxinode->plate_number = string(token);
                token = strtok(NULL, delimiters);
            }
            else
            {
                flag_good_record = false;
            }

            // timestamp
            if (token)
            {
                p_taxinode->timestamp = string(token);
                token = strtok(NULL, delimiters);
            }
            else
            {
                flag_good_record = false;
            }

            // longitude
            if (token)
            {
                p_taxinode->longitude = atof(token);
                token = strtok(NULL, delimiters);
            }
            else
            {
                flag_good_record = false;
            }

            // latitude
            if (token)
            {
                p_taxinode->latitude = atof(token);
                token = strtok(NULL, delimiters);
            }
            else
            {
                flag_good_record = false;
            }

            // speed
            if (token)
            {
                p_taxinode->speed = atof(token);
                token = strtok(NULL, delimiters);
            }
            else
            {
                flag_good_record = false;
            }

            // state 1
            if (token)
            {
                p_taxinode->state_1 = atoi(token);
                token = strtok(NULL, delimiters);
            }
            else
            {
                flag_good_record = false;
            }

            // state 2
            if (token)
            {
                p_taxinode->state_2 = atoi(token);
                token = strtok(NULL, delimiters);
            }
            else
            {
                flag_good_record = false;
            }

            // angle to north
            if (token)
            {
                p_taxinode->angle_to_north = atof(token);
                token = strtok(NULL, delimiters);
            }
            else
            {
                flag_good_record = false;
            }

            // vehicle type
            if (token)
            {
                p_taxinode->vehicle_type = string(token);
                token = strtok(NULL, delimiters);
            }
            else
            {
                flag_good_record = false;
            }

            if (flag_good_record)
            {
                // add the taxi node to the corresponding car
                addTaxiNodeToACar(p_taxinode);
                count_goodrecord++;
            }
            else
            {
                count_badrecord++;
            }
        }
        ifs.close();
    }

    cout << "Taxis " << num_cars << ", GPS nodes "
         << count_goodrecord << "/" << count_badrecord << " (good/bad)\n";
    cout << "OK! " << loadtaxidata_timer.nsecsElapsed()/1e9 << " seconds.\n" << std::flush;
}

void ParticleSystemTrafficTrj::addTaxiNodeToACar(TaxiNode *p_taxinode)
{
    // compute the opengl position and timemark
    p_taxinode->timemark = convertTimestamp(p_taxinode->timestamp);
    p_taxinode->position = model_parameter->real_map->
            convertLongitudeLatitudeToXY(p_taxinode->longitude, p_taxinode->latitude);

    // add the node to the correct car or a new car
    CarParticle *p_car = findCarByTaxiNode(p_taxinode);
    if (p_car)
    {
        insertTaxiNode(p_car, p_taxinode);
    }
    else
    {
        p_car = new CarParticle;
        p_car->id = p_taxinode->id;
        p_car->taxinode_list.clear();
        p_car->taxinode_list.push_back(p_taxinode);
        p_car->current_taxinode = 0;
        p_car->position = start_position;

        // assign car mesh and color
        int num_carmesh = model_parameter->car_mesh_list.size();
        int num_carcolor = model_parameter->car_color_list.size();
        p_car->carmesh_id = Randomer::generateRandomId(num_carmesh);
        p_car->carmesh = model_parameter->car_mesh_list[p_car->carmesh_id];
        p_car->carcolor_id = Randomer::generateRandomId(num_carcolor);
        p_car->carcolor = model_parameter->car_color_list[p_car->carcolor_id];

        insertCar(p_car);
    }
}

CarParticle* ParticleSystemTrafficTrj::findCarByTaxiNode(TaxiNode* p_taxinode)
{
    // binary search
    long long idx = p_taxinode->id;
    int low = 0, high = num_cars - 1, middle;
    while (low <= high)
    {
        middle = (low + high) / 2;
        long long idx_1 = car_list[middle]->id;

        if (idx == idx_1)
        {
            return car_list[middle];
        }
        else if (idx < idx_1)
        {
            high = middle - 1;
        }
        else
        {
            low = middle + 1;
        }
    }

    return NULL;
}

void ParticleSystemTrafficTrj::insertTaxiNode(CarParticle *p_car,
                                              TaxiNode *p_taxinode)
{
    // binary search by the timemark
    double tt = p_taxinode->timemark, tt_1;
    int num_taxinode = p_car->taxinode_list.size();
    int low = 0, high = num_taxinode - 1, middle;
    while (low <= high)
    {
        middle = (low + high) / 2;
        tt_1 = p_car->taxinode_list[middle]->timemark;

        if (tt == tt_1)
        {
            break;
        }
        else if (tt < tt_1)
        {
            high = middle - 1;
        }
        else
        {
            low = middle + 1;
        }
    }

    // determine the place to insert the taxi node before or after [middle]
    if (tt > tt_1)
    {
        middle++;
    }

    // insert the taxi node
    p_car->taxinode_list.push_back(NULL);
    for (int i=num_taxinode; i>middle; --i)
    {
        p_car->taxinode_list[i] = p_car->taxinode_list[i-1];
    }
    p_car->taxinode_list[middle] = p_taxinode;

    if (p_car->current_taxinode >= middle)
    {
        p_car->current_taxinode++;
    }
}

void ParticleSystemTrafficTrj::insertCar(CarParticle *p_car)
{
    // binary search by the timemark
    long long idx = p_car->id, idx_1 = idx;
    int low = 0, high = num_cars - 1, middle = 0;
    while (low <= high)
    {
        middle = (low + high) / 2;
        idx_1 = car_list[middle]->id;

        if (idx == idx_1)
        {
            break;
        }
        else if (idx < idx_1)
        {
            high = middle - 1;
        }
        else
        {
            low = middle + 1;
        }
    }

    // determine the place to insert the car before or after [middle]
    if (idx > idx_1)
    {
        middle++;
    }

    // insert the car
    car_list.push_back(NULL);
    for (int i=num_cars; i>middle; --i)
    {
        car_list[i] = car_list[i-1];
    }
    car_list[middle] = p_car;

    num_cars++;
}

double ParticleSystemTrafficTrj::convertTimestamp(const string &str)
{
    struct tm ts = {0};
    sscanf(str.c_str(), "%i-%i-%i %i:%i:%i",
           &ts.tm_year, &ts.tm_mon, &ts.tm_mday,
           &ts.tm_hour, &ts.tm_min, &ts.tm_sec);

    ts.tm_year -= 1900; // Years from 1900
    ts.tm_mon -= 1; // Months form January

    return difftime(mktime(&ts), reference_time);
}
