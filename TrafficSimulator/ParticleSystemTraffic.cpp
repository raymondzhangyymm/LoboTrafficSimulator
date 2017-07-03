#include "ParticleSystemTraffic.h"

// Constructor
ParticleSystemTraffic::ParticleSystemTraffic(SystemParameter *model_parameter_)
{
    // Initialize the material.
    model_parameter = model_parameter_;

    // Initialize the random number generator.
    p_randomer = new Randomer();

    // Initialize the kernels
    Kernel::initKernels(model_parameter->SupportRadius, model_parameter->SupportRadiusHK);

    isUpdatingFieldMeshes = false;
    isUpdatingCars = false;
    isUpdatingParticles = false;

    num_particles = 0;
    max_particleID = 0;
    num_cars = 0;
    max_carID = 0;

    field_mesh_top = DEFAULT_LANE_WIDTH * 4.0 * 2.5;
    field_mesh_bottom = DEFAULT_LANE_WIDTH * 4.0 * 0.8;

    density_average = 0;
    density_peak = 0;
    velocity_average = 0;
    velocity_peak = 0;

    simulation_time = 0;

    // The bounding box of the road net should be at least 1 by 1.
    min_corner = model_parameter->real_map->min_corner;
    max_corner = model_parameter->real_map->max_corner;
    if (min_corner.x > -0.5)
    {
        min_corner.x = -0.5;
    }
    if (min_corner.y > -0.5)
    {
        min_corner.y = -0.5;
    }
    if (max_corner.x < 0.5)
    {
        max_corner.x = 0.5;
    }
    if (max_corner.y < 0.5)
    {
        max_corner.y = 0.5;
    }
}

//==============================================================================================
void ParticleSystemTraffic::initSystem()
{
    buildRoads();

    hash_table = new HashTable(model_parameter->SupportRadius);

    buildRoadsOverlappingCells();

    buildFieldMeshes();

//    createCarParticles();

    createSensors();

    // initialize the simulation count for each road
    production_particle.resize(num_roads);
    production_car.resize(num_roads);
    for (int i=0; i<num_roads; ++i)
    {
        production_particle[i] = 0;
        production_car[i] = 0;
    }

    cout << "===========================================================================\n"
         << "Running on mode 2 : 2D simulation.\n" << std::flush;
}

void ParticleSystemTraffic::step()
{
    consumeParticles();
    produceParticles();

    updateParticleNeighbors();
    updateParticleDensityPressure();

    if (isUpdatingCars)
    {
        updateCarParticles();
    }

    updateObservation();

    if (isUpdatingFieldMeshes)
    {
        updateFieldMeshes();
    }

    updateSensors();

    updateParticles();

    simulation_time += model_parameter->DeltaTime;
}

void ParticleSystemTraffic::reset()
{

}

//==============================================================================================
void ParticleSystemTraffic::buildRoads()
{
    cout << "===========================================================================\n"
         << "Build the road network ... \n" << std::flush;

    QElapsedTimer ss_timer;
    ss_timer.start();

    road_list.clear();
    int num_links = model_parameter->real_map->roadlink_list.size();
    for (int i=0; i<num_links; ++i)
    {
        RoadLink *p_roadlink = model_parameter->real_map->roadlink_list[i];
        if (p_roadlink->link_type == highway || p_roadlink->link_type == beziers)
        {
            Road *p_road = new Road(model_parameter, p_roadlink, i);
            road_list.push_back(p_road);
        }
    }
    num_roads = road_list.size();

    // get the number of primitives of all roads.
    int num_quads = 0;
    long long num_segs = 0;
    int num_cells = 0;
    long long num_beziers = 0;
    long long num_spline_points = 0;
    for (int i=0; i<num_roads; ++i)
    {
        Road *p_road = road_list[i];

        num_quads += p_road->visualroad_side_left.size()-1;
        num_segs += 2*(p_road->visuallane_solid[0].size()-1);
        int num_lanes = p_road->num_lanes;
        if (num_lanes > 1)
        {
            num_segs += (p_road->visuallane_dash[0].size()-1)*(num_lanes-1)/2.0;
        }
        num_cells += p_road->bound_area.cells.size();

        num_beziers += p_road->road_curve.beziers.size();
        num_spline_points += p_road->road_curve.num_points;
    }

    cout << "Roads " << num_roads << ", cells " << num_cells
         << ", quads " << num_quads << ", lines " << num_segs << endl;
    cout << "Beziers " << num_beziers << ", spline points " << num_spline_points << endl;

    cout << "OK! " << ss_timer.nsecsElapsed()/1e9 << " seconds\n" << std::flush;
}

void ParticleSystemTraffic::buildRoadsOverlappingCells()
{
    for (int i=0; i<num_roads; ++i)
    {
        vector<Cell*> *cells_array_1 = &road_list[i]->bound_area.cells;
        int size_1 = cells_array_1->size();
        for (int m=0; m<size_1; ++m)
        {
            Cell *p_cell_1 = cells_array_1->at(m);
            for (int j=i+1; j<num_roads; ++j)
            {
                vector<Cell*> *cells_array_2 = &road_list[j]->bound_area.cells;
                int size_2 = cells_array_2->size();
                for (int n=0; n<size_2; ++n)
                {
                    Cell *p_cell_2 = cells_array_2->at(n);
                    if (isOverlappingBySAT(p_cell_1->v, p_cell_2->v))
                    {
                        p_cell_1->isOverlappingCell = true;
                        p_cell_1->overlapping_cells.push_back(p_cell_2);

                        p_cell_2->isOverlappingCell = true;
                        p_cell_2->overlapping_cells.push_back(p_cell_1);
                    }
                }
            }
        }
    }

//    for (int i=0; i<num_roads; ++i)
//    {
//        vector<Cell*> *cells_array_1 = &road_list[i].bound_area.cells;
//        cout << "bound areas : " << i << " " << cells_array_1->size() << endl;
//    }
}

void ParticleSystemTraffic::buildFieldMeshesByGrids()
{
//    // Clear the meshs
//    field_mesh_density.node_list.clear();
//    field_mesh_density.triangle_list.clear();
//    field_mesh_velocity.node_list.clear();
//    field_mesh_velocity.triangle_list.clear();

//    // The bounding box
//    double x0 = min_corner.x*1.1;
//    double x1 = max_corner.x*1.1;
//    double y0 = min_corner.y*1.1;
//    double y1 = max_corner.y*1.1;
//    double step = DEFAULT_LANE_WIDTH * 0.2;

//    // The numbers of the two most recent columns of visual particles
//    int column_0 = 0,column_1 = 0;

//    for (double x=x0; x<=x1; x += step)
//    {
//        column_0 = column_1;
//        column_1 = 0;

//        for (double y=y0; y<=y1; y += step)
//        {
//            vector<int> roadcell_id_list;
//            zVec2 pos(x,y);
//            if (locateCarParticlePosition(pos, roadcell_id_list))
//            {
//                // a node for density
//                Node *p_node = new Node;
//                p_node->original_position.x = pos.x;
//                p_node->original_position.y = pos.y;
//                p_node->original_position.z = field_mesh_bottom;

//                p_node->position = p_node->original_position;
//                p_node->density = 0.0;
//                p_node->velocity = 0.0;
//                p_node->normalized_density = 0.0;
//                p_node->normalized_velocity = 0.0;

//                field_mesh_density.node_list.push_back(p_node);

//                // a node for velocity
//                p_node = new Node;
//                p_node->original_position.x = pos.x;
//                p_node->original_position.y = pos.y;
//                p_node->original_position.z = field_mesh_bottom;

//                p_node->position = p_node->original_position;
//                p_node->density = 0.0;
//                p_node->velocity = 0.0;
//                p_node->normalized_density = 0.0;
//                p_node->normalized_velocity = 0.0;

//                field_mesh_velocity.node_list.push_back(p_node);

//                column_1++;
//            }
//        }

//        // generate the faces corresponding to the most recent two columns.
//        field_mesh_density.buildMeshByRecentTwoColumnNodes(column_0, column_1, step);
//        field_mesh_velocity.buildMeshByRecentTwoColumnNodes(column_0, column_1, step);
//    }

//    field_mesh_density.computeNormal();
//    field_mesh_velocity.computeNormal();

//    density_average = 0;
//    density_peak = 0;
//    velocity_average = 0;
//    velocity_peak = 0;

//    cout << "The field mesh #nodes = " << field_mesh_density.node_list.size()
//         << " #triangles = " << field_mesh_density.triangle_list.size() << endl;
}

void ParticleSystemTraffic::buildFieldMeshes()
{
    cout << "===========================================================================\n"
         << "Build field meshes ... \n" << std::flush;

    QElapsedTimer ss_timer;
    ss_timer.start();

    field_mesh_density.node_list.clear();
    field_mesh_density.triangle_list.clear();
    field_mesh_velocity.node_list.clear();
    field_mesh_velocity.triangle_list.clear();

    // Determine the number of segments by which a lane is divied.
    int num_seg_per_lane = 5;

    for (int k=0; k<num_roads; ++k)
    {
        Road *p_road = road_list[k];
        int count_node = 0;

        // Determine the total sampling points along normal of the road.
        int num_sampling = num_seg_per_lane * p_road->num_lanes + 1;

        // Determine the step length along x and y directions.
        double step_y = DEFAULT_LANE_WIDTH / double(num_seg_per_lane);
        double step_x = step_y * 2.0;

        double x_pos = 0.0;
        while (x_pos < p_road->road_length)
        {
            zVec2 nl = p_road->road_curve.getNormalByXPosition(x_pos);
            zVec2 pos = p_road->road_curve.getPointByXPosition(x_pos) + nl*(p_road->road_width*0.5);

            nl *= step_y;
            for (int i=0; i<num_sampling; ++i)
            {
                count_node++;

                // a node for density
                Node *p_node = new Node;
                p_node->original_position.x = pos.x;
                p_node->original_position.y = pos.y;
                p_node->original_position.z = field_mesh_bottom;

                p_node->position = p_node->original_position;
                p_node->density = 0.0;
                p_node->velocity = 0.0;
                p_node->normalized_density = 0.0;
                p_node->normalized_velocity = 0.0;

                field_mesh_density.node_list.push_back(p_node);

                // a node for velocity
                p_node = new Node;
                p_node->original_position.x = pos.x;
                p_node->original_position.y = pos.y;
                p_node->original_position.z = field_mesh_bottom;

                p_node->position = p_node->original_position;
                p_node->density = 0.0;
                p_node->velocity = 0.0;
                p_node->normalized_density = 0.0;
                p_node->normalized_velocity = 0.0;

                field_mesh_velocity.node_list.push_back(p_node);

                pos -= nl;
            }

            x_pos += step_x;
        }

        field_mesh_density.buildMeshByNodesGroup(count_node, num_sampling);
        field_mesh_velocity.buildMeshByNodesGroup(count_node, num_sampling);
    }

    field_mesh_density.computeNormal();
    field_mesh_velocity.computeNormal();

    cout << "Nodes " << field_mesh_density.node_list.size()
         << " and triangles " << field_mesh_density.triangle_list.size() << " per mesh\n";

    cout << "OK! " << ss_timer.nsecsElapsed()/1e9 << " seconds\n" << std::flush;
}

void ParticleSystemTraffic::createParticlesByGrids()
{
    particle_list.clear();
    max_particleID = 0;

    double x0 = min_corner.x*1.1;
    double x1 = max_corner.x*1.1;
    double y0 = min_corner.y*1.1;
    double y1 = max_corner.y*1.1;
    double step = DEFAULT_LANE_WIDTH * 0.5;

    for (double x=x0; x<=x1; x += step)
    {
        for (double y=y0; y<=y1; y += step)
        {
            vector<int> roadcell_id_list;
            zVec2 pos(x,y);
            if (locateParticlePosition(pos, roadcell_id_list))
            {
                Particle* p_particle = new Particle();

                p_particle->id = max_particleID++;

                p_particle->sphereRadius = model_parameter->ParticleRadius;
                p_particle->mass = model_parameter->ParticleMass;

                // set initial status
                p_particle->orig_position = pos;
                p_particle->orig_velocity.clear();
                p_particle->orig_acceleration.clear();

                // set initial position for determining the initial neighbours
                p_particle->position = p_particle->orig_position;

                // set initial velocity for determining viscosity force
                p_particle->velocity = p_particle->orig_velocity;

                // set initial acceleration
                p_particle->acceleration = p_particle->orig_acceleration;

                // initialize road and cell ids.
                p_particle->roadcell_id_list = roadcell_id_list;
                p_particle->road_id = roadcell_id_list[0];
                p_particle->cell_id = roadcell_id_list[1];

                // determine the normal and tangent
                vector<Cell*>* p_cell_list = &road_list[p_particle->road_id]->bound_area.cells;
                Cell *p_cell = p_cell_list->at(p_particle->cell_id);
                p_particle->normal = p_cell->center_normal;
                p_particle->tangent = p_cell->center_tangent;
                p_particle->isInRoad = isInRoadArea(p_particle->position);

                // Adding particle to the array of particles.
                particle_list.push_back(p_particle);
            }
        }
    }

    num_particles = particle_list.size();
//    cout << "Created particles = " << num_particles << endl;
}

void ParticleSystemTraffic::createParticles()
{
    particle_list.clear();
    max_particleID = 0;

    for (int k=0; k<num_roads; ++k)
    {
        Road *p_road = road_list[k];

        // There are about 5 particles over one cell length.
        double step_x = p_road->cell_length / 5.0;

        // Determine the number of sampling points over the road width
        double bound_width = p_road->road_width + p_road->lane_width*1.8;
        int num_sampling_width = bound_width / step_x + 2;
        double step_y = bound_width / double(num_sampling_width-1);

        double x_pos = 0.0;
        while (x_pos < p_road->road_length)
        {
            zVec2 nl = p_road->road_curve.getNormalByXPosition(x_pos);
            zVec2 pos = p_road->road_curve.getPointByXPosition(x_pos) + nl*(bound_width*0.5);

            nl *= step_y;
            for (int i=0; i<num_sampling_width; ++i)
            {
                vector<int> roadcell_id_list;
                if (locateParticlePosition(pos, roadcell_id_list))
                {
                    Particle* p_particle = new Particle();

                    p_particle->id = max_particleID++;
                    p_particle->roadcell_id_list = roadcell_id_list;
                    p_particle->road_id = roadcell_id_list[0];
                    p_particle->cell_id = roadcell_id_list[1];

                    p_particle->mass = model_parameter->ParticleMass;
                    p_particle->sphereRadius = model_parameter->ParticleRadius;

                    p_particle->initByPositionAndCellEnvolop
                            (pos, road_list[p_particle->road_id]->bound_area.cells[p_particle->cell_id]);

                    p_particle->isInRoad = isInRoadArea(p_particle->position);

                    particle_list.push_back(p_particle);
                }
                pos -= nl;
            }

            x_pos += step_x;
        }
    }

    num_particles = particle_list.size();
//    cout << "Created particles = " << num_particles << endl;
}

void ParticleSystemTraffic::createCarParticles()
{
    car_list.clear();
    max_carID = 0;

    int num_carmesh = model_parameter->car_mesh_list.size();
    int num_carcolor = model_parameter->car_color_list.size();

    int car_count = 0;
    for (int k=0; k<num_roads; ++k)
    {
        Road *p_road = road_list[k];
        CellSequence *p_bound = &p_road->road_area;
        int num_lanes = p_road->num_lanes;

        // place num_lanes cars every four cells.
        int num_cells = p_bound->cells.size();
        for (int i=0; i<num_cells; i+=4)
        {
            Cell *p_cell = p_bound->cells[i];

            zVec2 v1 = (p_cell->v[0] + p_cell->v[3]) * 0.5;
            zVec2 norm = p_cell->center_normal;
            double lane_width = road_list[k]->road_width / double(num_lanes);

            for (int j=0; j<num_lanes; ++j)
            {
                CarParticle *p_car = new CarParticle(p_cell);
                max_carID++;

                p_car->id = car_count++;
                p_car->road_id = k;
                p_car->cell_id = i;

                p_car->carmesh_id = Randomer::generateRandomId(num_carmesh);
                p_car->carmesh = model_parameter->car_mesh_list[p_car->carmesh_id];
                p_car->carcolor_id = Randomer::generateRandomId(num_carcolor);
                p_car->carcolor = model_parameter->car_color_list[p_car->carcolor_id];

                p_car->initByPosition(v1 - norm * (lane_width * (double(j) + 0.5)));

                car_list.push_back(p_car);
            }
        }
    }

    num_cars = max_carID;
}

void ParticleSystemTraffic::createSensors()
{
    // build the sensors for the first road only.
    if (road_list.size() > 0)
    {
        Road *p_road = road_list[0];
        sensor_lists = p_road->sensor_lists;
    }
}

//============================================================================================
void ParticleSystemTraffic::produceParticles()
{
    for (int k=0; k<num_roads; ++k)
    {
        Road *p_road = road_list[k];
        if (p_road->isInflow[0])
        {
            double inc = p_road->inflow_rate[0] * double(p_road->num_lanes) *
                    model_parameter->DeltaTime * (1.2 - 0.4 * Randomer::generateRandomFromZeroToOne());
            production_car[k] += inc;
            production_particle[k] += inc * 10.0;

            //-----------------------------------------------------------------------
            // produce particles
            int num_newpart = production_particle[k];
            production_particle[k] -= num_newpart;

            // determine the line segement where particles will be produced.
            zVec2 line_start = p_road->bound_area.source.v[3];
            zVec2 line_v = p_road->bound_area.source.v[2] - line_start;

            for (int i=0; i<num_newpart; ++i)
            {
                int cell_id;
                zVec2 vv = line_start + line_v * Randomer::generateRandomFromZeroToOne();

                if (p_road->bound_area.isInArea(vv, cell_id))
                {
                    Particle* p_particle = new Particle();

                    p_particle->id = max_particleID++;
                    p_particle->road_id = k;
                    p_particle->cell_id = cell_id;

                    p_particle->mass = model_parameter->ParticleMass;
                    p_particle->sphereRadius = model_parameter->ParticleRadius;

                    p_particle->initByPositionAndCellEnvolop
                            (vv, road_list[p_particle->road_id]->bound_area.cells[p_particle->cell_id]);

                    p_particle->isInRoad = p_road->road_area.isInArea(p_particle->position);

                    // assign destination
//                    double fac = Randomer::generateRandomFromZeroToOne();
//                    if (fac <= 0.2) {
//                        p_particle->destination_id = 1;
//                    } else if (fac <= 0.7) {
//                        p_particle->destination_id = 0;
//                    } else {
//                        p_particle->destination_id = 2;
//                    }

                    particle_list.push_back(p_particle);
                }
            }
        }

        //continue;

        //-----------------------------------------------------------------------
        // produce cars
        int num_newcar = production_car[k];
        production_car[k] -= num_newcar;

        int num_carmesh = model_parameter->car_mesh_list.size();
        int num_carcolor = model_parameter->car_color_list.size();

        for (int i=0; i<num_newcar; ++i)
        {
            Cell *p_cell = p_road->road_area.cells[0];
            int lane_id = Randomer::generateRandomId(p_road->num_lanes);

            CarParticle *p_car = new CarParticle(p_cell);

            p_car->id = max_carID++;
            p_car->road_id = k;
            p_car->cell_id = 0;

            p_car->carmesh_id = Randomer::generateRandomId(num_carmesh);
            p_car->carmesh = model_parameter->car_mesh_list[p_car->carmesh_id];
            p_car->carcolor_id = Randomer::generateRandomId(num_carcolor);
            p_car->carcolor = model_parameter->car_color_list[p_car->carcolor_id];

            p_car->initByPosition((p_cell->v[0] + p_cell->v[3]) * 0.5 - p_cell->center_normal *
                    (p_road->lane_width * (double(lane_id) + 0.5)));

            // discard the car if it overlaps with the existing cars.
            zVec2 bbox_1[4];
            p_car->getBbox(bbox_1);
            for (int k=0; k<num_cars; ++k)
            {
                zVec2 bbox_2[4];
                car_list[k]->getBbox(bbox_2);
                if (isOverlappingBySAT(bbox_1, bbox_2))
                {
                    delete p_car;
                    p_car = NULL;
                    break;
                }
            }
            if (p_car)
            {
                car_list.push_back(p_car);
            }
        }
    }

    num_particles = particle_list.size();
    num_cars = car_list.size();
}

void ParticleSystemTraffic::consumeParticles()
{
    //===========================================================================
    // set flag for bad particles, e.g. having reached the destination.
    for (int i=0; i<num_particles; ++i)
    {
        Particle *p_part = particle_list[i];

        for (int k=0; k<num_roads; ++k)
        {
            Cell *p_destination = &road_list[k]->bound_area.destination;
            if (p_destination->isInZone(p_part->position))
            {
                p_part->isBad = true;
                break;
            }
        }
    }

    // remove bad particles.
    for (vector<Particle*>::iterator it = particle_list.begin();
         it != particle_list.end(); ++it)
    {
        if ((*it)->isBad)
        {
            if ((*it) != particle_list.back())
            {
                swap(*it, particle_list.back());
                delete particle_list.back();  // destroy the particle
                particle_list.pop_back();
            }
            else
            {
                delete particle_list.back();  // destroy the particle
                particle_list.pop_back();
                break;
            }
        }
    }
    num_particles = particle_list.size();


    //return;

    //===========================================================================
    // set flag for bad cars, e.g. having reached the destination.
    for (int i=0; i<num_cars; ++i)
    {
        CarParticle *p_car = car_list[i];
        for (int k=0; k<num_roads; ++k)
        {
            Cell *p_destination = &road_list[k]->bound_area.destination;
            if (p_destination->isInZone(p_car->position))
            {
                p_car->isBad = true;
                break;
            }
        }
    }

    // remove bad cars
    // make a copy
    vector<CarParticle*> temp_car_list = car_list;
    car_list.clear();

    // determine the first car to be reserved.
    vector<CarParticle*>::iterator ss = temp_car_list.begin();
    while (ss != temp_car_list.end())
    {
        if (!(*ss)->isBad)
        {
            break;
        }
        delete (*ss);
        ss++;
    }

    if (ss != temp_car_list.end())
    {
        vector<CarParticle*>::iterator ee = ss+1;
        while (ee != temp_car_list.end())
        {
            if ((*ee)->isBad)
            {
                car_list.insert(car_list.end(), ss, ee-1);

                // determine the next car to be reserved.
                delete (*ee);
                ss = ee + 1;
                while (ss != temp_car_list.end())
                {
                    if (!(*ss)->isBad)
                    {
                        break;
                    }
                    delete (*ss);
                    ss++;
                }
                ee = ss;
            }
            else
            {
                ee++;
            }
        }

        car_list.insert(car_list.end(), ss, ee);
    }
    num_cars = car_list.size();


    // remove the particles
//    vector<Particle*> temp_list = particle_list;
//    particle_list.clear();

//    for (int i=0; i<num_particles; ++i)
//    {
//        Particle *p_part = temp_list[i];

//        if (p_part->isBad)
//        {
//            delete p_part;
//        }
//        else
//        {
//            for (int k=0; k<num_roads; ++k)
//            {
//                Cell *p_destination = &road_list[k].bound_area.destination;
//                if (p_destination->isInZone(p_part->position))
//                {
//                    p_part->isBad = true;
//                    break;
//                }
//            }

//            if (p_part->isBad)
//            {
//                delete p_part;
//            }
//            else
//            {
//                particle_list.push_back(p_part);
//            }
//        }
//    }
//    num_particles = particle_list.size();

    // remove the car particles
//    vector<CarParticle*> temp_car_list = car_list;
//    car_list.clear();

//    for (int i=0; i<num_cars; ++i)
//    {
//        CarParticle *p_car = temp_car_list[i];

//        if (p_car->isBad)
//        {
//            delete p_car;
//        }
//        else
//        {
//            for (int k=0; k<num_roads; ++k)
//            {
//                Cell *p_destination = &road_list[k].bound_area.destination;
//                if (p_destination->isInZone(p_car->position))
//                {
//                    p_car->isBad = true;
//                    break;
//                }
//            }

//            if (p_car->isBad)
//            {
//                delete p_car;
//            }
//            else
//            {
//                car_list.push_back(p_car);
//            }
//        }
//    }
//    num_cars = car_list.size();
}

void ParticleSystemTraffic::updateParticleNeighbors()
{
    // Re-build the hash table.
    hash_table->buildHashTable(particle_list, model_parameter->SupportRadius);

    // Determine the new neighbour list of each particle.
    for (int i=0; i<num_particles; ++i)
    {
        Particle *p_part = particle_list[i];

//        if (p_part->isSelected)
//        {
//            int aa = 0;
//        }

        hash_table->getTwoNeighborListsByXPosition(p_part->position, p_part->tangent,
                                                   p_part->neighbors,
                                                   p_part->neighborsHalfRegion);
//        hash_table->getNearestNeighbors(p_part->position, p_part->neighbors);
//        hash_table->getNearestNeighborsHalfRegion(p_part->position, p_part->tangent,
//                                                  p_part->neighborsHalfRegion);
    }
}

void ParticleSystemTraffic::updateParticleDensityPressure()
{
    for (int i=0; i<num_particles; ++i)
    {
        Particle *p_part = particle_list[i];

        // determine the density
        double density = 0;
        int num_neigh = p_part->neighbors.size();
        for (int j=0; j<num_neigh; ++j)
        {
            Particle *p_neigh = p_part->neighbors[j];
            density += p_neigh->mass * Kernel::getPoly(p_part->position - p_neigh->position);
        }
        // Note: computing the density should take itself into account
//        density += p_part->mass * Kernel::getPoly(zVec2(0.0,0.0));
        p_part->density = density;

        // determine the pressure
//            p_part->pressure = model_parameter->GasStiffness * density * 0.15 ;//1.0
        //    p_part->pressure = model_parameter->GasStiffness * density * sqrt(density) * 0.4;//1.5
        p_part->pressure = model_parameter->GasStiffness * density * density;//2.0
        //    p_part->pressure = model_parameter->GasStiffness * density * density * sqrt(density) *1.7;//2.5
    }
}

void ParticleSystemTraffic::updateFieldMeshes()
{
    density_average = 0;
    density_peak = 0;
    velocity_average = 0;
    velocity_peak = 0;

    double field_mesh_height = field_mesh_top - field_mesh_bottom;
    int num_nodes = field_mesh_density.node_list.size();
    for (int i=0; i<num_nodes; ++i)
    {
        Node *p_node = field_mesh_density.node_list[i];

        // find neighbours by the position
        vector<Particle*> neighbors;
        zVec2 pos(p_node->position.x, p_node->position.y);
        hash_table->getNearestNeighbors(pos, neighbors);

        // interpolate the density and the speed
//        double density = 0, velocity = 0;
//        int num_neigh = neighbors.size();
//        for (int j=0; j<num_neigh; ++j)
//        {
//            Particle* p_neigh = neighbors[j];

//            double k = Kernel::getPoly(pos - p_neigh->position);
//            density += p_neigh->mass * k;
//            velocity += p_neigh->velocity.length() * (p_neigh->mass * k / p_neigh->density);
//        }

        // interpolate the density and the speed
        double density = 0;
        zVec2 velocity(0,0);
        int num_neigh = neighbors.size();
        for (int j=0; j<num_neigh; ++j)
        {
            Particle* p_neigh = neighbors[j];

            double k = Kernel::getPoly(pos - p_neigh->position);
            density += p_neigh->mass * k;
            velocity += p_neigh->velocity * (p_neigh->mass * k / p_neigh->density);
        }

        p_node->density = density; // real density
        p_node->normalized_density = density / model_parameter->MaxDensity; // normalized density
        p_node->position.z = field_mesh_bottom + field_mesh_height * p_node->normalized_density;
        if (p_node->position.z > field_mesh_top)
        {
            p_node->position.z = field_mesh_top;
        }
        density_average += density;
        if (density > density_peak)
        {
            density_peak = density;
        }

        double vel_length = velocity.length();
        p_node = field_mesh_velocity.node_list[i];
        p_node->velocity = vel_length; // real velocity
        p_node->normalized_velocity = vel_length / model_parameter->MaxVelocity;  // normalized velocity
        p_node->position.z = field_mesh_bottom + field_mesh_height * p_node->normalized_velocity;
        if (p_node->position.z > field_mesh_top)
        {
            p_node->position.z = field_mesh_top;
        }
        velocity_average += vel_length;
        if (vel_length > velocity_peak)
        {
            velocity_peak = vel_length;
        }
    }

    field_mesh_density.computeNormal();
    field_mesh_velocity.computeNormal();

    density_average /= double(num_nodes);
    velocity_average /= double(num_nodes);
}

void ParticleSystemTraffic::updateParticles()
{
    computeTrafficForces();

    for (int i=0; i<num_particles; ++i)
    {
        Particle *p_part = particle_list[i];

        p_part->isAnchored = false;
        if (model_parameter->AddConstrain)
        {
            int num_block = block_list.size();
            for (int j=0; j<num_block; ++j)
            {
                if (block_list[j].isInZone(p_part->position))
                {
                    p_part->isAnchored = true;
                    break;
                }
            }
        }

        // step-try according to individual particles
        if (p_part->isAnchored)
        {
            // a constrained particle
            double vel_mag = p_part->velocity.length();
            if (vel_mag >= 1e-6)
            {
                p_part->stepTry(model_parameter->DeltaTime, p_part->velocity * (1.0*CONSTRAIN_SPEED/vel_mag));
            }
        }
        else
        {            
            // a regular particle
            p_part->acceleration = p_part->trafficForce / p_part->density;
            p_part->stepTry(model_parameter->DeltaTime);
        }

        // Handle the collision including velocity/position/cellid
        if (handleCollisionBetweenCellParticle(p_part))
        {
            p_part->stepConfirm();

            // Update the normal/tangent according to the cell which it is in.
            vector<Cell*>* p_cell_list = &road_list[p_part->road_id]->bound_area.cells;
            Cell *p_cell = p_cell_list->at(p_part->cell_id);
            p_part->normal = p_cell->center_normal;
            p_part->tangent = p_cell->center_tangent;
            p_part->isInRoad = isInRoadArea(p_part->position);
        }
        else
        {
            p_part->isBad = true;
        }


        // switch to the destination road
        vector<Cell*>* p_cell_list = &road_list[p_part->road_id]->bound_area.cells;
        Cell *p_cell = p_cell_list->at(p_part->cell_id);
        if (p_cell->isOverlappingCell)
        {
            vector<Cell*> *p_overlapping_cells = &p_cell->overlapping_cells;
            int num_cells = p_overlapping_cells->size();
            for (int i=0; i<num_cells; ++i)
            {
                Cell *p_cell_overlap = p_overlapping_cells->at(i);
                if (p_cell_overlap->isInZone(p_part->position) &&
                        p_cell_overlap->road_id == p_part->destination_id)
                {
                    p_part->road_id = p_cell_overlap->road_id;
                    p_part->cell_id = p_cell_overlap->id;
                }
            }
        }
    }
}

void ParticleSystemTraffic::updateSpringDisplacementOfCars()
{
    // clear the current spring displacement.
    for (int i=0; i<num_cars; ++i)
    {
        CarParticle *p_car = car_list[i];
        p_car->spring_displacement.clear();
        p_car->isColliding = false;
    }

    // generate spring force once the collision is detected.
    for (int i=0; i<num_cars; ++i)
    {
        CarParticle *p_car = car_list[i];
        zVec2 bbox[4];
        p_car->getBbox(bbox);

        for (int j=i+1; j<num_cars; ++j)
        {
            CarParticle *p_car_1 = car_list[j];
            zVec2 bbox_1[4];
            p_car_1->getBbox(bbox_1);

            // We define the direction of dist is identical to the resulting force.
            zVec2 dist;
            if (isOverlappingByMinkowski(bbox, bbox_1, dist))
            {
                p_car->spring_displacement += dist;
                p_car_1->spring_displacement += -dist;

                p_car->isColliding = true;
                p_car_1->isColliding = true;

//                for (int k=0; k<4; ++k)
//                {
//                    cout << bbox[k].x << "," << bbox[k].y << ";";
//                }
//                for (int k=0; k<4; ++k)
//                {
//                    cout << bbox_1[k].x << "," << bbox_1[k].y << ";";
//                }
//                cout << endl;
//                cout << dist << endl;
            }
        }
    }

//    for (int i=0; i<num_cars; ++i)
//    {
//        CarParticle *p_car = car_list[i];

//        if (abs(p_car->spring_force.x) > 0.0001 && abs(p_car->spring_force.y) > 0.0001)
//        {
//            cout << "car collision :" << p_car->id << p_car->spring_force << endl;
//        }
//    }
}

void ParticleSystemTraffic::updateCarParticles()
{
    int num_cars = car_list.size();
    for (int i=0; i<num_cars; ++i)
    {
        CarParticle *p_car = car_list[i];

        vector<Particle*> neighbours;

        // find the velocity of the car particle
        hash_table->getNearestNeighbors(p_car->position, neighbours);
        zVec2 velocity;
        velocity.clear();
        int num_neigh = neighbours.size();
        for (int j=0; j<num_neigh; ++j)
        {
            Particle* p_neigh = neighbours[j];
            double k = Kernel::getPoly(p_car->position - p_neigh->position);
            velocity += p_neigh->velocity * (p_neigh->mass * k / p_neigh->density);
        }
        p_car->velocity = velocity;

        // compute the spring acceleration.
        if (p_car->isColliding)
        {
            p_car->computeSpringAcceleration(model_parameter->DeltaTime);
        }
        else
        {
            p_car->clearSpring();
        }

        // try a step and handle the collision between the car and the cells
        p_car->stepTry(model_parameter->DeltaTime);
//        if (!handleCollisionBetweenCellCarByReflection(p_car))
        if (!handleCollisionBetweenCellCarBySpring(p_car))
        {
            p_car->isBad = true;
        }
        else
        {
            p_car->stepConfirm();
        }
    }

//    handleCollisionBetweenCars();

    // Update the normal/orientation according to the cell which it is in.
    for (int i=0; i<num_cars; ++i)
    {
        CarParticle *p_car = car_list[i];
        if (!p_car->isBad)
        {
            Cell *p_cell = road_list[p_car->road_id]->road_area.cells[p_car->cell_id];
            p_car->normal = p_cell->center_normal;
            p_car->orientation = p_cell->center_tangent;
        }
    }

    // The spring forces are used to handle collisions between cars.
    updateSpringDisplacementOfCars();
}

void ParticleSystemTraffic::updateSensors()
{
    int num_sensor_lists = sensor_lists.size();
    for (int k=0; k<num_sensor_lists; ++k)
    {
        vector<Node> *p_sensor_list = &sensor_lists[k];
        int num_sensors = p_sensor_list->size();
        for (int i=0; i<num_sensors; ++i)
        {
            Node *p_node = &p_sensor_list->at(i);

            // find neighbours by the position
            vector<Particle*> neighbors;
            zVec2 pos(p_node->position.x, p_node->position.y);
            hash_table->getNearestNeighbors(pos, neighbors);

            // interpolate the density and the speed
            double density = 0;
            zVec2 velocity(0,0);
            int num_neigh = neighbors.size();
            for (int j=0; j<num_neigh; ++j)
            {
                Particle* p_neigh = neighbors[j];

                double k = Kernel::getPoly(pos - p_neigh->position);
                density += p_neigh->mass * k;
                velocity += p_neigh->velocity * (p_neigh->mass * k / p_neigh->density);
            }

            p_node->density = density; // real density
            p_node->normalized_density = density / model_parameter->MaxDensity; // normalized density

            p_node->velocity = velocity.length(); // real velocity
            p_node->normalized_velocity = velocity.length() / model_parameter->MaxVelocity;  // normalized velocity
        }
    }
}

void ParticleSystemTraffic::synchronizeParticles()
{
    particle_list.clear();
    int num_roads = road_list.size();
    for (int i=0; i<num_roads; ++i)
    {
        Road *p_road = road_list[i];
        int num_lanes = p_road->num_lanes;
        for (int j=0; j<num_lanes; ++j)
        {
            vector<Particle*> *p_list = &p_road->particle_lists[j];
            particle_list.insert(particle_list.end(),
                                 p_list->begin(), p_list->end());
        }
    }

    num_particles = particle_list.size();
    max_particleID = num_particles;
}

void ParticleSystemTraffic::synchronizeCars()
{
    car_list.clear();
    int num_roads = road_list.size();
    for (int i=0; i<num_roads; ++i)
    {
        Road *p_road = road_list[i];
        int num_lanes = p_road->num_lanes;
        for (int j=0; j<num_lanes; ++j)
        {
            vector<CarParticle*> *p_list = &p_road->car_lists[j];
            car_list.insert(car_list.end(), p_list->begin(), p_list->end());
        }
    }

    num_cars = car_list.size();
    max_carID = num_cars;
}

void ParticleSystemTraffic::computeTrafficForces()
{
    computeDampingForces();

    if (model_parameter->UseExternalForce)
    {
        computeExternalForces();
    }

    if (model_parameter->UsePressureForce)
    {
        if (model_parameter->UseHalfKernel)
        {
            computePressureForcesHalfRegion();
        }
        else
        {
            computePressureForces();
        }
    }

    if (model_parameter->UseViscosityForce)
    {
        if (model_parameter->UseHalfKernel)
        {
            computeViscosityForcesHalfRegion();
        }
        else
        {
            computeViscosityForces();
        }
    }

    // Sum up to traffic forces.
    for (int i=0; i<num_particles; ++i)
    {
        Particle *p_part = particle_list[i];
        p_part->trafficForce = p_part->dampingForce +
                               p_part->externalForce +
                               p_part->pressureForce +
                               p_part->viscosityForce;
    }
}

void ParticleSystemTraffic::computeDampingForces()
{
    for (int i=0; i<num_particles; ++i)
    {
        Particle *p_part = particle_list[i];
        p_part->dampingForce = p_part->velocity *
                               (- model_parameter->CoeffDampingForce * p_part->density);
    }
}

void ParticleSystemTraffic::computeExternalForces()
{
    for (int i=0; i<num_particles; ++i)
    {
        Particle *p_part = particle_list[i];
        p_part->externalForce = p_part->tangent *
                                (model_parameter->CoeffFieldForce * p_part->density);
    }
}

void ParticleSystemTraffic::computePressureForces()
{
    for (int i=0; i<num_particles; ++i)
    {
        Particle *p_part = particle_list[i];

        vector<Particle*>* p_list = &p_part->neighbors;
        int num_neigh = p_list->size();

        zVec2 pressureForce(0.0, 0.0);
        for (int j=0; j<num_neigh; j++)
        {
            Particle* p_neigh = p_list->at(j);

            if (p_part != p_neigh)
            {
                double coef = (p_part->pressure + p_neigh->pressure)
                              * p_neigh->mass / (2.0 * p_neigh->density);
                pressureForce += Kernel::getSpikyGradient(p_part->position - p_neigh->position)
                                 * coef;
            }
        }
        p_part->pressureForce = pressureForce * (-1.0);
    }
}

void ParticleSystemTraffic::computePressureForcesHalfRegion()
{
    for (int i=0; i<num_particles; ++i)
    {
        Particle *p_part = particle_list[i];

        vector<Particle*>* p_list = &p_part->neighborsHalfRegion;
        int num_neigh = p_list->size();

        zVec2 pressureForce(0.0, 0.0);
        for (int j=0; j<num_neigh; j++)
        {
            Particle* p_neigh = p_list->at(j);

            if (p_part != p_neigh)
            {
                double coef = (p_part->pressure + p_neigh->pressure)
                              * p_neigh->mass / (2.0 * p_neigh->density);
                pressureForce += Kernel::getSpikyGradientHK(p_part->position - p_neigh->position)
                                 * coef;
            }
        }
        p_part->pressureForce = pressureForce * (-1.0);
    }
}

void ParticleSystemTraffic::computeViscosityForces()
{
    for (int i=0; i<num_particles; ++i)
    {
        Particle *p_part = particle_list[i];

        vector<Particle*>* p_list = &p_part->neighbors;
        int num_neigh = p_list->size();

        zVec2 viscosityForce(0,0);
        for (int j=0; j<num_neigh; ++j)
        {
            Particle* p_neigh = p_list->at(j);

            if (p_part != p_neigh)
            {
                double coef = Kernel::getViscosityLaplacian(p_neigh->position - p_part->position)
                              * p_neigh->mass / p_neigh->density;
                viscosityForce += (p_neigh->velocity - p_part->velocity) * coef;
            }
        }
        p_part->viscosityForce = viscosityForce * model_parameter->Viscosity;
    }
}

void ParticleSystemTraffic::computeViscosityForcesHalfRegion()
{
    for (int i=0; i<num_particles; ++i)
    {
        Particle *p_part = particle_list[i];

        vector<Particle*>* p_list = &p_part->neighborsHalfRegion;
        int num_neigh = p_list->size();

        zVec2 viscosityForce(0,0);
        for (int j=0; j<num_neigh; ++j)
        {
            Particle* p_neigh = p_list->at(j);

            if (p_part != p_neigh)
            {
                double coef = Kernel::getViscosityLaplacianHK(p_neigh->position - p_part->position)
                              * p_neigh->mass / p_neigh->density;
                viscosityForce += (p_neigh->velocity - p_part->velocity) * coef;
            }
        }
        p_part->viscosityForce = viscosityForce * model_parameter->Viscosity;
    }
}

//============================================================================================
// Assistant Functions
//============================================================================================
bool ParticleSystemTraffic::handleCollisionBetweenCellParticle(Particle* p_part)
{
    // Get the boundary cell list
    vector<Cell*>* p_cell_list = &road_list[p_part->road_id]->bound_area.cells;

    // check if the new position is still in the current cell
    Cell* p_cell = p_cell_list->at(p_part->cell_id);
    if (p_cell->isInZone(p_part->position_try))
    {
        return true;
    }

    // check if the new position is in the neighbor cells
    Cell* p_cell_previous = NULL;
    if (p_part->cell_id >= 1)
    {
        p_cell_previous = p_cell_list->at(p_part->cell_id - 1);
        if (p_cell_previous->isInZone(p_part->position_try))
        {
            p_part->cell_id--;
            return true;
        }
    }
    Cell* p_cell_next = NULL;
    if (p_part->cell_id < p_cell_list->size()-1)
    {
        p_cell_next = p_cell_list->at(p_part->cell_id + 1);
        if (p_cell_next->isInZone(p_part->position_try))
        {
            p_part->cell_id++;
            return true;
        }
    }

    // check if the new position is in other overlapping cells
//    if (p_cell->isOverlappingCell)
//    {
//        vector<Cell*> *p_overlapping_cells = &p_cell->overlapping_cells;
//        int num_cells = p_overlapping_cells->size();
//        for (int i=0; i<num_cells; ++i)
//        {
//            Cell *p_cell_overlap = p_overlapping_cells->at(i);
//            if (p_cell_overlap->isInZone(p_part->position_try))
//            {
//                p_part->road_id = p_cell_overlap->road_id;
//                p_part->cell_id = p_cell_overlap->id;
//                return true;
//            }
//        }
//    }

    // the position_try is not in any cells, collision MUST have happened.
    zVec2 cp, norm, vel_new;
    if (p_cell->isIntersectedWithCellSides(cp, norm, p_part->position, p_part->position_try))
    {
        // re-step the particle using the new reflected velocity.
        vel_new = p_part->velocity_try - norm * (p_part->velocity_try.dotproduct(norm) * 2.0);
        p_part->stepTry(model_parameter->DeltaTime, vel_new);

        if (p_cell->isInZone(p_part->position_try))
        {
            return true;
        }
        else if (p_cell_next && p_cell_next->isInZone(p_part->position_try))
        {
            p_part->cell_id++;
            return true;
        }
        else if (p_cell_previous && p_cell_previous->isInZone(p_part->position_try))
        {
            p_part->cell_id--;
            return true;
        }
        else
        {
            // the updated position_try is still out side of the boundary
            // must handle that again
            if (p_cell_next && p_cell_next->isIntersectedWithCellSides(cp, norm,
                                                                       p_part->position,
                                                                       p_part->position_try))
            {
                // re-step the particle using the new reflected velocity.
                vel_new = p_part->velocity_try - norm*(p_part->velocity_try.dotproduct(norm)*2.0);
                p_part->stepTry(model_parameter->DeltaTime, vel_new);

                if (p_cell->isInZone(p_part->position_try))
                {
                    return true;
                }
                else if (p_cell_next && p_cell_next->isInZone(p_part->position_try))
                {
                    p_part->cell_id++;
                    return true;
                }
                else if (p_cell_previous && p_cell_previous->isInZone(p_part->position_try))
                {
                    p_part->cell_id--;
                    return true;
                }
            }
            else if (p_cell_previous && p_cell_previous->isIntersectedWithCellSides(cp, norm,
                                                                                    p_part->position,
                                                                                    p_part->position_try))
            {
                // re-step the particle using the new reflected velocity.
                vel_new = p_part->velocity_try - norm*(p_part->velocity_try.dotproduct(norm)*2.0);
                p_part->stepTry(model_parameter->DeltaTime, vel_new);

                if (p_cell->isInZone(p_part->position_try))
                {
                    return true;
                }
                else if (p_cell_next && p_cell_next->isInZone(p_part->position_try))
                {
                    p_part->cell_id++;
                    return true;
                }
                else if (p_cell_previous && p_cell_previous->isInZone(p_part->position_try))
                {
                    p_part->cell_id--;
                    return true;
                }
            }
        }
    }

    else if (p_cell_next && p_cell_next->isIntersectedWithCellSides(cp, norm,
                                                                    p_part->position,
                                                                    p_part->position_try))
    {
        // re-step the particle using the new reflected velocity.
        vel_new = p_part->velocity_try - norm*(p_part->velocity_try.dotproduct(norm)*2.0);
        p_part->stepTry(model_parameter->DeltaTime, vel_new);

        if (p_cell->isInZone(p_part->position_try))
        {
            return true;
        }
        else if (p_cell_next && p_cell_next->isInZone(p_part->position_try))
        {
            p_part->cell_id++;
            return true;
        }
        else if (p_cell_previous && p_cell_previous->isInZone(p_part->position_try))
        {
            p_part->cell_id--;
            return true;
        }
    }

    else if (p_cell_previous && p_cell_previous->isIntersectedWithCellSides(cp, norm,
                                                                            p_part->position,
                                                                            p_part->position_try))
    {
        // re-step the particle using the new reflected velocity.
        vel_new = p_part->velocity_try - norm*(p_part->velocity_try.dotproduct(norm)*2.0);
        p_part->stepTry(model_parameter->DeltaTime, vel_new);

        if (p_cell->isInZone(p_part->position_try))
        {
            return true;
        }
        else if (p_cell_next && p_cell_next->isInZone(p_part->position_try))
        {
            p_part->cell_id++;
            return true;
        }
        else if (p_cell_previous && p_cell_previous->isInZone(p_part->position_try))
        {
            p_part->cell_id--;
            return true;
        }
    }

    // Fail to handle the collision.
    // The only reason is the particle is in cell 0 and is pushed back out,
    // or, is in the last cell and pushed out without being consumed.
    if (p_part->cell_id != 0 && p_part->cell_id != p_cell_list->size()-1)
    {
        cout << "ERROR! Fail in handleCollision of part " << p_part->id << " in cell "
             << p_part->cell_id << " of road " << p_part->road_id << " !" << endl;
    }
    return false;
}

bool ParticleSystemTraffic::handleCollisionBetweenCellCarByReflection(CarParticle* p_car)
{
    // The reflecting/bouncing factor (1.0 ~ 2.0).
    double bouncing_factor = 1.5;

    // Get the boundary cell list
    vector<Cell*>* p_cell_list = &road_list[p_car->road_id]->road_area.cells;

    // check if the new position is still in the current cell
    Cell* p_cell = p_cell_list->at(p_car->cell_id);
    if (p_cell->isInZone(p_car->position_try))
    {
        p_car->cell_id_try = p_car->cell_id;
        return true;
    }

    // check if the new position is in the neighbor cells
    Cell* p_cell_previous = NULL;
    if (p_car->cell_id >= 1)
    {
        p_cell_previous = p_cell_list->at(p_car->cell_id - 1);
        if (p_cell_previous->isInZone(p_car->position_try))
        {
            p_car->cell_id_try = p_car->cell_id - 1;
            return true;
        }
    }
    Cell* p_cell_next = NULL;
    if (p_car->cell_id < p_cell_list->size()-1)
    {
        p_cell_next = p_cell_list->at(p_car->cell_id + 1);
        if (p_cell_next->isInZone(p_car->position_try))
        {
            p_car->cell_id_try = p_car->cell_id + 1;
            return true;
        }
    }

    // check if the new position is in other overlapping cells
//    if (p_cell->isOverlappingCell)
//    {
//        vector<Cell*> *p_overlapping_cells = &p_cell->overlapping_cells;
//        int num_cells = p_overlapping_cells->size();
//        for (int i=0; i<num_cells; ++i)
//        {
//            Cell *p_cell_overlap = p_overlapping_cells->at(i);
//            if (p_cell_overlap->isInZone(p_part->position_try))
//            {
//                p_part->road_id = p_cell_overlap->road_id;
//                p_part->cell_id = p_cell_overlap->id;
//                return true;
//            }
//        }
//    }

    // the position_try is not in any cells, collision MUST have happened.
    zVec2 cp, norm, vel_new;
    if (p_cell->isIntersectedWithCellSides(cp, norm, p_car->position, p_car->position_try))
    {
        // re-step the car using the new reflected velocity.
        vel_new = p_car->velocity_try - norm * (p_car->velocity_try.dotproduct(norm) * bouncing_factor);
        p_car->stepTry(model_parameter->DeltaTime, vel_new);

        if (p_cell->isInZone(p_car->position_try))
        {
            p_car->cell_id_try = p_car->cell_id;
            return true;
        }
        else if (p_cell_next && p_cell_next->isInZone(p_car->position_try))
        {
            p_car->cell_id_try = p_car->cell_id + 1;
            return true;
        }
        else if (p_cell_previous && p_cell_previous->isInZone(p_car->position_try))
        {
            p_car->cell_id_try = p_car->cell_id - 1;
            return true;
        }
        else
        {
            // the updated position_try is still out side of the boundary
            // must handle that again
            if (p_cell_next && p_cell_next->isIntersectedWithCellSides(cp, norm,
                                                                       p_car->position,
                                                                       p_car->position_try))
            {
                // re-step the car using the new reflected velocity.
                vel_new = p_car->velocity_try - norm * (p_car->velocity_try.dotproduct(norm) * bouncing_factor);
                p_car->stepTry(model_parameter->DeltaTime, vel_new);

                if (p_cell->isInZone(p_car->position_try))
                {
                    p_car->cell_id_try = p_car->cell_id;
                    return true;
                }
                else if (p_cell_next && p_cell_next->isInZone(p_car->position_try))
                {
                    p_car->cell_id_try = p_car->cell_id + 1;
                    return true;
                }
                else if (p_cell_previous && p_cell_previous->isInZone(p_car->position_try))
                {
                    p_car->cell_id_try = p_car->cell_id - 1;
                    return true;
                }
            }
            else if (p_cell_previous && p_cell_previous->isIntersectedWithCellSides(cp, norm,
                                                                                    p_car->position,
                                                                                    p_car->position_try))
            {
                // re-step the car using the new reflected velocity.
                vel_new = p_car->velocity_try - norm * (p_car->velocity_try.dotproduct(norm) * bouncing_factor);
                p_car->stepTry(model_parameter->DeltaTime, vel_new);

                if (p_cell->isInZone(p_car->position_try))
                {
                    p_car->cell_id_try = p_car->cell_id;
                    return true;
                }
                else if (p_cell_next && p_cell_next->isInZone(p_car->position_try))
                {
                    p_car->cell_id_try = p_car->cell_id + 1;
                    return true;
                }
                else if (p_cell_previous && p_cell_previous->isInZone(p_car->position_try))
                {
                    p_car->cell_id_try = p_car->cell_id - 1;
                    return true;
                }
            }
        }
    }

    else if (p_cell_next && p_cell_next->isIntersectedWithCellSides(cp, norm,
                                                                    p_car->position,
                                                                    p_car->position_try))
    {
        // re-step the car using the new reflected velocity.
        vel_new = p_car->velocity_try - norm * (p_car->velocity_try.dotproduct(norm) * bouncing_factor);
        p_car->stepTry(model_parameter->DeltaTime, vel_new);

        if (p_cell->isInZone(p_car->position_try))
        {
            p_car->cell_id_try = p_car->cell_id;
            return true;
        }
        else if (p_cell_next && p_cell_next->isInZone(p_car->position_try))
        {
            p_car->cell_id_try = p_car->cell_id + 1;
            return true;
        }
        else if (p_cell_previous && p_cell_previous->isInZone(p_car->position_try))
        {
            p_car->cell_id_try = p_car->cell_id - 1;
            return true;
        }
    }

    else if (p_cell_previous && p_cell_previous->isIntersectedWithCellSides(cp, norm,
                                                                            p_car->position,
                                                                            p_car->position_try))
    {
        // re-step the car using the new reflected velocity.
        vel_new = p_car->velocity_try - norm * (p_car->velocity_try.dotproduct(norm) * bouncing_factor);
        p_car->stepTry(model_parameter->DeltaTime, vel_new);

        if (p_cell->isInZone(p_car->position_try))
        {
            p_car->cell_id_try = p_car->cell_id;
            return true;
        }
        else if (p_cell_next && p_cell_next->isInZone(p_car->position_try))
        {
            p_car->cell_id_try = p_car->cell_id + 1;
            return true;
        }
        else if (p_cell_previous && p_cell_previous->isInZone(p_car->position_try))
        {
            p_car->cell_id_try = p_car->cell_id - 1;
            return true;
        }
    }

    if (p_car->cell_id != 0 && p_car->cell_id != p_cell_list->size() - 1)
    {
        cout << "ERROR! Fail in handleCollision of car " << p_car->id << " in cell "
             << p_car->cell_id << " of road " << p_car->road_id << " !" << endl;
    }
    return false;
}

bool ParticleSystemTraffic::handleCollisionBetweenCellCarBySpring(CarParticle* p_car)
{
    // The reflecting/bouncing factor (1.0 ~ 2.0).
    double stiffness = 150;
    double damping = 2.0 * stiffness * 0.005;

    // Get the boundary cell list
    vector<Cell*>* p_cell_list = &road_list[p_car->road_id]->road_area.cells;

    // check if the new position is still in the current cell
    Cell* p_cell = p_cell_list->at(p_car->cell_id);
    if (p_cell->isInZone(p_car->position_try))
    {
        p_car->cell_id_try = p_car->cell_id;
        p_car->isOutOfCell = false;
        return true;
    }

    // check if the new position is in the neighbor cells
    Cell* p_cell_previous = NULL;
    if (p_car->cell_id >= 1)
    {
        p_cell_previous = p_cell_list->at(p_car->cell_id - 1);
        if (p_cell_previous->isInZone(p_car->position_try))
        {
            p_car->cell_id_try = p_car->cell_id - 1;
            p_car->isOutOfCell = false;
            return true;
        }
    }
    Cell* p_cell_next = NULL;
    if (p_car->cell_id < p_cell_list->size()-1)
    {
        p_cell_next = p_cell_list->at(p_car->cell_id + 1);
        if (p_cell_next->isInZone(p_car->position_try))
        {
            p_car->cell_id_try = p_car->cell_id + 1;
            p_car->isOutOfCell = false;
            return true;
        }
    }

    // check if the new position is in other overlapping cells
//    if (p_cell->isOverlappingCell)
//    {
//        vector<Cell*> *p_overlapping_cells = &p_cell->overlapping_cells;
//        int num_cells = p_overlapping_cells->size();
//        for (int i=0; i<num_cells; ++i)
//        {
//            Cell *p_cell_overlap = p_overlapping_cells->at(i);
//            if (p_cell_overlap->isInZone(p_part->position_try))
//            {
//                p_part->road_id = p_cell_overlap->road_id;
//                p_part->cell_id = p_cell_overlap->id;
//                return true;
//            }
//        }
//    }

    // the position_try is not in any cells, collision MUST have happened.
    double penetration_depth;
    zVec2 norm;
    if (p_cell->isInExtendedZone(penetration_depth, norm, p_car->position_try))
    {
        // re-step the car using the new velocity with spring acceleration applied.
        zVec2 spring_acceleration = norm * (-p_car->velocity_try.dotproduct(norm) * damping +
                                            penetration_depth * stiffness);
        zVec2 vel_new = p_car->velocity_try + spring_acceleration * model_parameter->DeltaTime;
        p_car->stepTry(model_parameter->DeltaTime, vel_new);
        p_car->cell_id_try = p_car->cell_id;

        if (p_cell->isInZone(p_car->position_try))
        {
            p_car->isOutOfCell = false;
        }
        else
        {
            p_car->isOutOfCell = true;
        }
        return true;
    }

    else if (p_cell_next && p_cell_next->isInExtendedZone(penetration_depth, norm, p_car->position_try))
    {
        // re-step the car using the new velocity with spring acceleration applied.
        zVec2 spring_acceleration = norm * (p_car->velocity_try.dotproduct(norm) * damping +
                                            penetration_depth * stiffness);
        zVec2 vel_new = p_car->velocity_try + spring_acceleration * model_parameter->DeltaTime;
        p_car->stepTry(model_parameter->DeltaTime, vel_new);
        p_car->cell_id_try = p_car->cell_id + 1;

        if (p_cell_next->isInZone(p_car->position_try))
        {
            p_car->isOutOfCell = false;
        }
        else
        {
            p_car->isOutOfCell = true;
        }
        return true;
    }

    else if (p_cell_previous && p_cell_previous->isInExtendedZone(penetration_depth, norm, p_car->position_try))
    {
        // re-step the car using the new velocity with spring acceleration applied.
        zVec2 spring_acceleration = norm * (p_car->velocity_try.dotproduct(norm) * damping +
                                            penetration_depth * stiffness);
        zVec2 vel_new = p_car->velocity_try + spring_acceleration * model_parameter->DeltaTime;
        p_car->stepTry(model_parameter->DeltaTime, vel_new);
        p_car->cell_id_try = p_car->cell_id - 1;

        if (p_cell_previous->isInZone(p_car->position_try))
        {
            p_car->isOutOfCell = false;
        }
        else
        {
            p_car->isOutOfCell = true;
        }
        return true;
    }

    if (p_car->cell_id != 0 && p_car->cell_id != p_cell_list->size() - 1)
    {
        cout << "ERROR! Fail in handleCollision of car " << p_car->id << " in cell "
             << p_car->cell_id << " of road " << p_car->road_id << " !" << endl;
    }
    return false;
}

void ParticleSystemTraffic::handleCollisionBetweenCars()
{
//    // sort the cars by the road_id_try and the cell_id_try
//    // such that the cars are arranged along the traffic direction
//    vector<vector<CarParticle*>> sub_car_list(num_roads);
//    for (int i=0; i<num_cars; ++i)
//    {
//        CarParticle *p_car = car_list[i];
//        sub_car_list[p_car->road_id_try].push_back(p_car);
//    }
//    for (int k=0; k<num_roads; ++k)
//    {
//        vector<CarParticle*>* p_list = &sub_car_list[k];
//        int size = p_list->size();
//        for (int i=0; i<size; ++i)
//        {
//            for (int j=i+1; j<size; ++j)
//            {
//                if (p_list->at(i)->cell_id_try < p_list->at(j)->cell_id_try)
//                {
//                    CarParticle *p_car = p_list->at(i);
//                    p_list->at(i) = p_list->at(j);
//                    p_list->at(j) = p_car;
//                }
//            }
//        }
//    }

////    for (int k=0; k<num_roads; ++k)
////    {
////        vector<CarParticle*>* p_list = &sub_car_list[k];
////        int size = p_list->size();
////        for (int i=0; i<size; ++i)
////        {
////            CarParticle *p_car = p_list->at(i);
////            cout << p_car->road_id << " " << p_car->cell_id << endl;
////        }
////        cout << endl;
////    }

//    // handle the collision between the cars and determine the final car position.
//    for (int k=0; k<num_roads; ++k)
//    {
//        vector<CarParticle*>* p_list = &sub_car_list[k];
//        int size = p_list->size();
//        for (int i=0; i<size; ++i)
//        {
//            CarParticle *p_car = p_list->at(i);
//            zVec2 bbox[4];
//            p_car->getBbox(bbox);

//            // Check if it collides with any car in front of it.
//            bool collision_flag = false;
////            for (int j=i-1; j>=0; --j)
//            for (int j=size-1; i!=j && j>=0; --j)
//            {
//                CarParticle *p_car_1 = p_list->at(j);
//                zVec2 bbox_1[4];
//                p_car_1->getBbox(bbox_1);
//                if (isOverlappingBySAT(bbox, bbox_1))
//                {
//                    collision_flag = true;
//                    break;
//                }
//            }

//            if (!collision_flag)
//            {
//                p_car->stepConfirm();
//            }
//        }
//    }
}

bool ParticleSystemTraffic::detectCarCollision(CarParticle *target_car)
{
//    int num_cars = car_list.size();
//    for (int i=0; i<num_cars; ++i)
//    {
//        CarParticle *p_car = car_list[i];
//        if (p_car != target_car)
//        {
//            if (overlapBbox(p_car->bbox, target_car->bbox) ||
//                overlapBbox(target_car->bbox, p_car->bbox))
//            {
//                return true;
//            }
//        }
//    }
    return false;
}

bool ParticleSystemTraffic::detectCarCollision(CarParticle *target_car,
                                               vector<CarParticle*> &collision_car_list)
{
//    collision_car_list.clear();

//    int num_cars = car_list.size();
//    for (int i=0; i<num_cars; ++i)
//    {
//        CarParticle *p_car = car_list[i];

//        //if (p_car->particleID != target_car->particleID)
//        if (p_car != target_car)
//        {
//            if (overlapBbox(p_car->bbox, target_car->bbox) ||
//                overlapBbox(target_car->bbox, p_car->bbox))
//            {
//                collision_car_list.push_back(p_car);
//            }
//        }
//    }

//    if (collision_car_list.size() == 0)
//    {
//        return false;
//    }
//    else
//    {
        return true;
//    }
}

void ParticleSystemTraffic::respondCarCollision(CarParticle *target_car,
                                                vector<CarParticle*> &collision_car_list, double tt)
{
////    // method 1
////    target_car->tryStep(-tt);

//    // method 2
//    // clear the elastic force.
//    int size = collision_car_list.size();
//    for (int i=0; i<size; ++i)
//    {
//        CarParticle *p_car = collision_car_list[i];
//        p_car->elastic_force.clear();
//    }
//    target_car->elastic_force.clear();

//    // compute the elastic force between the collision car pairs.
//    double stiffness = 200;
//    for (int i=0; i<size; ++i)
//    {
//        CarParticle *p_car = collision_car_list[i];

//        // determine the rest distance
//        double len_rest = (target_car->carmesh->bbox[0] + p_car->carmesh->bbox[0]) * 0.5;

//        // determine the actual distance
//        zVec2 d = target_car->position - p_car->position;
//        double len = d.length();

//        // compute the elastic force by Hook's law
//        zVec2 elastic_force = d * (stiffness * (len_rest-len) / len);
//        target_car->elastic_force += elastic_force;
//        p_car->elastic_force += elastic_force * (-1);
//    }

////    cout << "collision target_car " << target_car->particleID
////         << target_car->elastic_force << target_car->position
////         << " to " << collision_car_list[0]->particleID
////         << collision_car_list[0]->elastic_force << collision_car_list[0]->position
////         << endl;

//    // compute the elastic acceleration and hence the new positions
//    Road *p_road = road_shape;
//    for (int i=0; i<size; ++i)
//    {
//        CarParticle *p_car = collision_car_list[i];
//        p_car->elastic_acceleration = p_car->elastic_force / model_parameter->ParticleMass;
//        p_car->tryStepByElasticForce(p_road, tt);
//    }
//    target_car->elastic_acceleration = target_car->elastic_force / model_parameter->ParticleMass;
//    target_car->tryStepByElasticForce(p_road, tt);
}

bool ParticleSystemTraffic::overlapBbox(vector<zVec2> &bb0, vector<zVec2> &bb1)
{
    for (int i=0; i<4; ++i)
    {
        bool flag_in = true;

        zVec2 v0 = bb0[1] - bb0[0];
        zVec2 v1 = bb1[i] - bb0[0];
        if (zVec2::signofCrossProduct(v0, v1) < 0 )
        {
            flag_in = false;
        }

        v0 = bb0[2] - bb0[1];
        v1 = bb1[i] - bb0[1];
        if (zVec2::signofCrossProduct(v0, v1) < 0 )
        {
            flag_in = false;
        }

        v0 = bb0[3] - bb0[2];
        v1 = bb1[i] - bb0[2];
        if (zVec2::signofCrossProduct(v0, v1) < 0 )
        {
            flag_in = false;
        }

        v0 = bb0[0] - bb0[3];
        v1 = bb1[i] - bb0[3];
        if (zVec2::signofCrossProduct(v0, v1) < 0 )
        {
            flag_in = false;
        }

        if (flag_in)
        {
            return true;
        }
    }

    return false;
}

//====================================================================================================
void ParticleSystemTraffic::updateObservation()
{
    int num_nodes = observation_list.size();
    for (int i=0; i<num_nodes; ++i)
    {
        Node *p_node = observation_list[i];

        // find neighbours by the position
        vector<Particle*> neighbors;
        zVec2 pos(p_node->position.x, p_node->position.y);
        hash_table->getNearestNeighbors(pos, neighbors);

        // interpolate the density and the speed
        double density = 0;
        zVec2 velocity(0,0);
        int num_neigh = neighbors.size();
        for (int j=0; j<num_neigh; ++j)
        {
            Particle* p_neigh = neighbors[j];

            double k = Kernel::getPoly(pos - p_neigh->position);
            density += p_neigh->mass * k;
            velocity += p_neigh->velocity * (p_neigh->mass * k / p_neigh->density);
        }

//        // find neighbours by the position
//        vector<Particle*> neighbors;
//        zVec2 pos(p_node->position.x, p_node->position.y);
//        hash_table->getNearestNeighbors(pos, neighbors);

//        // interpolate the density and the speed
//        double density = 0, velocity = 0;
//        int num_neigh = neighbors.size();
//        for (int j=0; j<num_neigh; ++j)
//        {
//            Particle* p_neigh = neighbors[j];

//            double k = Kernel::getPoly(pos - p_neigh->position);
//            density += p_neigh->mass * k;
//            velocity += p_neigh->velocity.length() * (p_neigh->mass * k / p_neigh->density);
//        }

        p_node->density = density;
        p_node->normalized_density = p_node->density / model_parameter->MaxDensity;

        p_node->velocity = velocity.length();
        p_node->normalized_velocity = p_node->velocity / model_parameter->MaxVelocity;
    }
}

void ParticleSystemTraffic::createObservationPoint()
{
    // create a new observation point at the origin
    Node* p_node = new Node;
    p_node->original_position.x = 0.0;
    p_node->original_position.y = 0.0;
    p_node->original_position.z = 0.2;

    p_node->position = p_node->original_position;
    p_node->density = 0.0;
    p_node->velocity = 0.0;
    p_node->normalized_density = 0.0;
    p_node->normalized_velocity = 0.0;

    p_node->isBad = false;

    // determine the color which is least used by the previous observation
    vector<zVec4> *p_color_list = &model_parameter->observation_color_list;
    int num_colors = p_color_list->size();
    vector<int> color_count_array(num_colors);
    for (int i=0; i<num_colors; ++i)
    {
        color_count_array[i] = 0;
    }

    int num_obs = observation_list.size();
    for (int i=0; i<num_obs; ++i)
    {
        color_count_array[observation_list[i]->color_id]++;
    }

    int color_count_min = num_obs, color_id = 0;
    for (int i=0; i<num_colors; ++i)
    {
        if (color_count_min > color_count_array[i])
        {
           color_count_min = color_count_array[i];
           color_id = i;
        }
    }

    p_node->color_id = color_id;
    p_node->color = p_color_list->at(color_id);

    if (num_obs > 0)
    {
        int max_id = 1;
        for (int i=0; i<num_obs; ++i)
        {
            if (max_id < observation_list[i]->id)
            {
                max_id = observation_list[i]->id;
            }
        }
        p_node->id = max_id + 1;
    }
    else
    {
        p_node->id = 1;
    }

    observation_list.push_back(p_node);
}

void ParticleSystemTraffic::deleteObservationPoint(Node *p_node)
{
    for (vector<Node*>::iterator it = observation_list.begin();
         it != observation_list.end(); ++it)
    {
        if ((*it) == p_node)
        {
            if ((*it) != observation_list.back())
            {
                swap(*it, observation_list.back());
            }
            observation_list.pop_back();
            break;
        }
    }
}

void ParticleSystemTraffic::moveObservationPoint(Node *p_node, zVec2 pos)
{
    p_node->position.x = pos.x;
    p_node->position.y = pos.y;
}

//void ParticleSystemTraffic::deleteObservationPoint(int idx)
//{
//    int num_observation = observation_list.size();
//    if (idx >= 0 && idx < num_observation)
//    {
//        for (int i=idx; i<num_observation-1; ++i)
//        {
//            observation_list[i] = observation_list[i+1];
//        }
//        observation_list.pop_back();
//    }
//}

//void ParticleSystemTraffic::moveObservationPoint(int idx, zVec2 pos)
//{
//    if (idx >= 0 && idx < observation_list.size())
//    {
//        observation_list[idx].position.x = pos.x;
//        observation_list[idx].position.y = pos.y;
//    }
//}

//====================================================================================================
void ParticleSystemTraffic::createBlockArea()
{
    double block_halflen_x = DEFAULT_LANE_WIDTH / 2;
    double block_halflen_y = DEFAULT_LANE_WIDTH;

    Cell cc;
    cc.v[0].x = - block_halflen_x;
    cc.v[0].y = - block_halflen_y;
    cc.v[1].x =   block_halflen_x;
    cc.v[1].y = - block_halflen_y;
    cc.v[2].x =   block_halflen_x;
    cc.v[2].y =   block_halflen_y;
    cc.v[3].x = - block_halflen_x;
    cc.v[3].y =   block_halflen_y;
    cc.init();
    cc.align(getTangentFromRoadNet(cc.center_point));

    block_list.push_back(cc);    
}

void ParticleSystemTraffic::deleteBlockArea(int idx)
{
    int num_block = block_list.size();
    if (idx >= 0 && idx < num_block)
    {
        for (int i=idx; i<num_block-1; ++i)
        {
            block_list[i] = block_list[i+1];
        }
        block_list.pop_back();
    }
}

void ParticleSystemTraffic::moveBlockArea(int idx, zVec2 pos)
{
    if (idx >= 0 && idx < block_list.size())
    {
        Cell *p_cell = &block_list[idx];

        double block_halflen_x = DEFAULT_LANE_WIDTH / 2;
        double block_halflen_y = DEFAULT_LANE_WIDTH;
        p_cell->v[0].x = pos.x - block_halflen_x;
        p_cell->v[0].y = pos.y - block_halflen_y;
        p_cell->v[1].x = pos.x + block_halflen_x;
        p_cell->v[1].y = pos.y - block_halflen_y;
        p_cell->v[2].x = pos.x + block_halflen_x;
        p_cell->v[2].y = pos.y + block_halflen_y;
        p_cell->v[3].x = pos.x - block_halflen_x;
        p_cell->v[3].y = pos.y + block_halflen_y;
        p_cell->init();
        p_cell->align(getTangentFromRoadNet(pos));
    }
}

zVec2 ParticleSystemTraffic::getTangentFromRoadNet(zVec2 pos)
{
    for (int i=0; i<num_roads; ++i)
    {
        vector<Cell*> *cells_array = &road_list[i]->bound_area.cells;
        int size = cells_array->size();
        for (int m=0; m<size; ++m)
        {
            Cell *p_cell = cells_array->at(m);
            if (p_cell->isInZone(pos))
            {
                return p_cell->center_tangent * (-1);
            }
        }
    }
    return zVec2(1,0);
}

//====================================================================================================
bool ParticleSystemTraffic::locateParticlePosition(zVec2 pos, vector<int> &roadcell_id_list)
{
    roadcell_id_list.clear();

    for (int i=0; i<num_roads; ++i)
    {
        Road *p_road = road_list[i];
        int id;
        if (p_road->bound_area.isInArea(pos, id))
        {
            roadcell_id_list.push_back(i);
            roadcell_id_list.push_back(id);
        }
    }

    if (roadcell_id_list.size() == 0)
    {
        // the position is not in any boundary area.
        return false;
    }
    else
    {
        return true;
    }
}

bool ParticleSystemTraffic::locateCarParticlePosition(zVec2 pos, vector<int> &roadcell_id_list)
{
    roadcell_id_list.clear();

    for (int i=0; i<num_roads; ++i)
    {
        Road *p_road = road_list[i];
        int id;
        if (p_road->road_area.isInArea(pos, id))
        {
            roadcell_id_list.push_back(i);
            roadcell_id_list.push_back(id);
        }
    }

    if (roadcell_id_list.size() == 0)
    {
        // the position is not in any boundary area.
        return false;
    }
    else
    {
        return true;
    }
}

bool ParticleSystemTraffic::isInBoundaryArea(zVec2 pos)
{
    for (int i=0; i<num_roads; ++i)
    {
        Road *p_road = road_list[i];
        if (p_road->bound_area.isInArea(pos))
        {
            return true;
        }
    }
    return false;
}

bool ParticleSystemTraffic::isInRoadArea(zVec2 pos)
{
    for (int i=0; i<num_roads; ++i)
    {
        Road *p_road = road_list[i];
        if (p_road->road_area.isInArea(pos))
        {
            return true;
        }
    }
    return false;
}

bool ParticleSystemTraffic::isOverlappingBySAT(zVec2 bbox_1[], zVec2 bbox_2[])
{
    zVec2 v1[4], v2[4];
    for (int i=0; i<4; i++)
    {
        v1[i] = bbox_1[i];
        v2[i] = bbox_2[i];
    }

    // test cell 1
    for (int i=0; i<4; i++)
    {
        // compute the normal
        zVec2 e = v1[(i+1)%4] - v1[i];
        zVec2 n;
        n.x = -e.y;
        n.y = e.x;

        // project the vertices of cell 1 to the normal vector and find the min and max.
        double projection[4];
        for (int j=0; j<4; j++)
        {
            projection[j] = n.dotproduct(v1[j]);
        }
        double max_1 = projection[0], min_1 = projection[0];
        for (int j=1; j<4; j++)
        {
            if (max_1 < projection[j])
            {
                max_1 = projection[j];
            }
            if (min_1 > projection[j])
            {
                min_1 = projection[j];
            }
        }

        // project the vertices of cell 2 to the normal vector and find the min and max.
        for (int j=0; j<4; j++)
        {
            projection[j] = n.dotproduct(v2[j]);
        }
        double max_2 = projection[0], min_2 = projection[0];
        for (int j=1; j<4; j++)
        {
            if (max_2 < projection[j])
            {
                max_2 = projection[j];
            }
            if (min_2 > projection[j])
            {
                min_2 = projection[j];
            }
        }

        // check if the projections overlaps.
        if (min_1 > max_2 || min_2 > max_1)
        {
            // separating axis exists, no overlapping.
            return false;
        }
    }

    // test cell 2
    for (int i=0; i<4; i++)
    {
        // compute the normal
        zVec2 e = v2[(i+1)%4] - v2[i];
        zVec2 n;
        n.x = -e.y;
        n.y = e.x;

        // project the vertices of cell 1 to the normal vector and find the min and max.
        double projection[4];
        for (int j=0; j<4; j++)
        {
            projection[j] = n.dotproduct(v1[j]);
        }
        double max_1 = projection[0], min_1 = projection[0];
        for (int j=1; j<4; j++)
        {
            if (max_1 < projection[j])
            {
                max_1 = projection[j];
            }
            if (min_1 > projection[j])
            {
                min_1 = projection[j];
            }
        }

        // project the vertices of cell 2 to the normal vector and find the min and max.
        for (int j=0; j<4; j++)
        {
            projection[j] = n.dotproduct(v2[j]);
        }
        double max_2 = projection[0], min_2 = projection[0];
        for (int j=1; j<4; j++)
        {
            if (max_2 < projection[j])
            {
                max_2 = projection[j];
            }
            if (min_2 > projection[j])
            {
                min_2 = projection[j];
            }
        }

        // check if the projections overlaps.
        if (min_1 > max_2 || min_2 > max_1)
        {
            // separating axis exists, no overlapping.
            return false;
        }
    }

    return true;
}

bool ParticleSystemTraffic::isOverlappingByMinkowski(zVec2 bbox_1[], zVec2 bbox_2[], zVec2 &mtd)
{
    // Rotate B(bbox_2) around the origin, i.e. get -B.
    // Thus can compute the desired Minkowski sum of A*(-B).
    bbox_2[0] = - bbox_2[0];
    bbox_2[1] = - bbox_2[1];
    bbox_2[2] = - bbox_2[2];
    bbox_2[3] = - bbox_2[3];

    // get the norms of A and B
    // Note that the norms must point outside of the bbox.
    zVec2 norm_1[4], norm_2[4];
    for (int i=0; i<4; ++i)
    {
        zVec2 vv = bbox_1[(i+1)%4] - bbox_1[i];
        norm_1[i].x = vv.y;
        norm_1[i].y = -vv.x;
        norm_1[i].normalize();

        vv = bbox_2[(i+1)%4] - bbox_2[i];
        norm_2[i].x = vv.y;
        norm_2[i].y = -vv.x;
        norm_2[i].normalize();
    }

//    for (int i=0; i<4; ++i)
//    {
//        cout << bbox_1[i];
//    }
//    for (int i=0; i<4; ++i)
//    {
//        cout << bbox_2[i];
//    }cout << endl;


    // translate the edges from A by the supporting vertices from B
    double least_penetrating_distance = -1e15;
    double least_positive_distance = 1e15;
    zVec2 minimum_penetrating_vector, minimum_positive_vector;
    for (int i=0; i<4; ++i)
    {
        // get the supporting vertex that can translate the edge farthest away alone the norm.
        int idx_supporting_vertex = 0;
        double max_translation = -1e15;
        for (int j=0; j<4; ++j)
        {
            double translation = norm_1[i].dotproduct(bbox_2[j]);
            if (translation > max_translation)
            {
                max_translation = translation;
                idx_supporting_vertex = j;
            }
        }

        // compute the translated line segment and the distance (could be negative) from the origin.
        zVec2 r0 = bbox_1[i] + bbox_2[idx_supporting_vertex];
        zVec2 r1 = bbox_1[(i+1)%4] + bbox_2[idx_supporting_vertex];
        double a = r1.y - r0.y;
        double b = r0.x - r1.x;
        double dist = (- r0.x*a - r0.y*b) / sqrt(a*a + b*b);

        // compare with the global least distance
        if (dist > least_penetrating_distance)
        {
            least_penetrating_distance = dist;
            minimum_penetrating_vector = norm_1[i];
        }

        // Compare with the least_positive_distance.
        // Note that if dist>0, it can be concluded no intersection.
        if (dist > 0)
        {
            // Check if the projection of origin is within r0 to r1
            zVec2 rr = r1-r0;
            if (rr.dotproduct(r0) * rr.dotproduct(r1) <= 0.0)
            {
                if (dist < least_positive_distance)
                {
                    least_positive_distance = dist;
                    minimum_positive_vector = -norm_1[i];
                }
            }
            else
            {
                // Must also consider the distances to the endpoints of the edge
                dist = r0.length();
                if (dist < least_positive_distance)
                {
                    least_positive_distance = dist;
                    minimum_positive_vector = r0 / dist;
                }

                dist = r1.length();
                if (dist < least_positive_distance)
                {
                    least_positive_distance = dist;
                    minimum_positive_vector = r1 / dist;
                }
            }
        }
    }

    // translate the edges from B by the supporting vertices from A
    for (int i=0; i<4; ++i)
    {
        // get the supporting vertex that can translate the edge farthest away alone the norm.
        int idx_supporting_vertex = 0;
        double max_translation = -1e15;
        for (int j=0; j<4; ++j)
        {
            double translation = norm_2[i].dotproduct(bbox_1[j]);
            if (translation > max_translation)
            {
                max_translation = translation;
                idx_supporting_vertex = j;
            }
        }

        // compute the translated line segment and the distance (could be negative) from the origin.
        zVec2 r0 = bbox_2[i] + bbox_1[idx_supporting_vertex];
        zVec2 r1 = bbox_2[(i+1)%4] + bbox_1[idx_supporting_vertex];
        double a = r1.y - r0.y;
        double b = r0.x - r1.x;
        double dist = (- r0.x*a - r0.y*b) / sqrt(a*a + b*b);

        // compare with the global least distance
        if (dist > least_penetrating_distance)
        {
            least_penetrating_distance = dist;
            minimum_penetrating_vector = norm_2[i];
        }

        // Compare with the least_positive_distance.
        // Note that if dist>0, it can be concluded no intersection.
        if (dist > 0)
        {
            // Check if the projection of origin is within r0 to r1
            zVec2 rr = r1-r0;
            if (rr.dotproduct(r0) * rr.dotproduct(r1) <= 0.0)
            {
                if (dist < least_positive_distance)
                {
                    least_positive_distance = dist;
                    minimum_positive_vector = -norm_2[i];
                }
            }
            else
            {
                // Must also consider the distances to the endpoints of the edge
                dist = r0.length();
                if (dist < least_positive_distance)
                {
                    least_positive_distance = dist;
                    minimum_positive_vector = r0 / dist;
                }

                dist = r1.length();
                if (dist < least_positive_distance)
                {
                    least_positive_distance = dist;
                    minimum_positive_vector = r1 / dist;
                }
            }
        }
    }

    if (least_penetrating_distance <= 0.0)
    {
        // This the penetrating vector describing how deep bbox_1 is in bbox_2.
        // bbox_1 should move opposite the vector in order to separate them.
        mtd = minimum_penetrating_vector * (-least_penetrating_distance);
        return true;
    }
    else
    {
        mtd = minimum_positive_vector * least_positive_distance;
        return false;
    }
}

//void ParticleSystemTraffic::resetInflowCounter(Road *p_road)
//{
//    int road_id = p_road->id;
//    simulation_stepcount[road_id] = 0;
//    double sc = 1.0 / (10.0 * model_parameter->RoadInflowRate[road_id] * model_parameter->DeltaTime)
//                * (1.2 - 0.4 * Randomer::generateRandomFromZeroToOne());
//    if (sc < 1)
//    {
//        simulation_stepcount_target[road_id] = 1;
//    }
//    else
//    {
//        simulation_stepcount_target[road_id] = sc;
//    }
//}
