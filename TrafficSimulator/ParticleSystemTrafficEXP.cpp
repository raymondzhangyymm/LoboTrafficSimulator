#include "ParticleSystemTrafficEXP.h"

ParticleSystemTrafficEXP::ParticleSystemTrafficEXP(SystemParameter *model_parameter_)
    : ParticleSystemTraffic(model_parameter_)
{
    // Determine the visual space for the field meshes.
//    field_mesh_top = DEFAULT_LANE_WIDTH * 20;
//    field_mesh_bottom = DEFAULT_LANE_WIDTH * 2;
    field_mesh_top = DEFAULT_LANE_WIDTH * 10;
    field_mesh_bottom = DEFAULT_LANE_WIDTH * 0.5;

    // Check if it is a 1D or 2D experiment.
    if (model_parameter->ExperimentType == 21 ||
        model_parameter->ExperimentType == 22 ||
        model_parameter->ExperimentType == 23 ||
        model_parameter->ExperimentType == 24)
    {
        isTwoDimension = true;
    }
    else
    {
        isTwoDimension = false;
    }    

    isUpdatingFieldMeshes = true;
}

//==============================================================================================
void ParticleSystemTrafficEXP::initSystem()
{
    buildRoads();

    if (isTwoDimension)
    {
        hash_table = new HashTable(model_parameter->SupportRadius);

        buildFieldMeshes();

        createSensors();

        if (model_parameter->ExperimentType == 21 ||
            model_parameter->ExperimentType == 22)
        {
            createParticles();
        }

        // add the active particle for EXP 22.
        if (model_parameter->ExperimentType == 22)
        {
            Road *p_road = road_list[0];
            zVec2 pos = p_road->road_curve.getPointByXPosition(0);

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
                p_particle->isActive = true;

                particle_list.push_back(p_particle);

                num_particles++;
            }
            else
            {
                cout << "Failed to initialize the active particle!\n";
            }
        }

        if (num_roads > 0)
        {
            // set up a block for EXP 3
            Road *p_road = road_list[0];
            int cell_id = p_road->bound_area.cells.size() * 0.7;
            Cell cc = *p_road->bound_area.cells[cell_id];
            Cell cc0 = *p_road->bound_area.cells[cell_id-1];
            Cell cc1 = *p_road->bound_area.cells[cell_id+1];

            cc.v[0] = (cc.v[0] + cc.v[1]) / 2.0;
            cc.v[3] = (cc.v[2] + cc.v[3]) / 2.0;
            cc.v[1] = (cc0.v[1] + cc0.v[2]) / 2.0;
            cc.v[2] = (cc1.v[1] + cc1.v[2]) / 2.0;

            cc.setCenterToGeometricCenter();
            cc.init();
            block_list.push_back(cc);
        }

        // initialize the simulation count for each road
        production_particle.resize(num_roads);
        production_car.resize(num_roads);
        for (int i=0; i<num_roads; ++i)
        {
            production_particle[i] = 0;
            production_car[i] = 0;
        }

        cout << "===========================================================================\n"
             << "Running on mode 9 : 2D experiment " << model_parameter->ExperimentType
             << ".\n" << std::flush;
    }

    else
    {
        createSensors();

        // set up the simulation for all roads
        for (int i=0; i<num_roads; ++i)
        {
            Road *p_road = road_list[i];
            p_road->initRoadSimulation();

            road_list[i]->block_list = &block_list;
        }

        synchronizeParticles();
        synchronizeCars();
//        cout << "Particles " << num_particles << " and cars " << num_cars << "\n";

        if (num_roads > 0)
        {
            // set up a block for EXP 3
            Road *p_road = road_list[0];
            int cell_id = p_road->bound_area.cells.size() * 0.7;
            Cell cc = *p_road->bound_area.cells[cell_id];
            //        Cell cc0 = *p_road->bound_area.cells[cell_id-1];
            //        Cell cc1 = *p_road->bound_area.cells[cell_id+1];

            //        cc.v[0] = (cc.v[0] + cc.v[1]) / 2.0;
            //        cc.v[3] = (cc.v[2] + cc.v[3]) / 2.0;
            //        cc.v[1] = (cc0.v[1] + cc0.v[2]) / 2.0;
            //        cc.v[2] = (cc1.v[1] + cc1.v[2]) / 2.0;

            cc.setCenterToGeometricCenter();
            cc.init();
            block_list.push_back(cc);
        }

        cout << "===========================================================================\n"
             << "Running on mode 9 : 1D experiment " << model_parameter->ExperimentType
             << ".\n" << std::flush;
    }
}

void ParticleSystemTrafficEXP::step()
{
    if (isTwoDimension)
    {
        consumeParticles();
        produceParticles();

        updateParticleNeighbors();
        updateParticleDensityPressure();

        if (isUpdatingCars)
        {
            updateCarParticles();
        }

        if (isUpdatingFieldMeshes)
        {
            updateFieldMeshes();
        }

        updateObservation();

        updateSensors();

        updateParticles();
    }

    else
    {
        if (!isUpdatingFieldMeshes)
        {
            for (int i=0; i<num_roads; ++i)
            {
                Road *p_road = road_list[i];
                p_road->stepXNOFieldMeshes();
            }
        }
        else
        {
            for (int i=0; i<num_roads; ++i)
            {
                Road *p_road = road_list[i];
                p_road->stepX();
            }
        }

        synchronizeParticles();
        synchronizeCars();

        updateObservation();

        // update the sensors for the first road only.
        if (road_list.size() > 0)
        {
            Road *p_road = road_list[0];
            sensor_lists = p_road->sensor_lists;
        }
    }

    simulation_time += model_parameter->DeltaTime;
}

void ParticleSystemTrafficEXP::updateParticles()
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

        // get acceleration
        p_part->acceleration = p_part->trafficForce / p_part->density;

        // step-try according to individual particles
        if (p_part->isActive && model_parameter->ExperimentType == 22)
        {
            // a special particle for EXP 2 only.
            //p_part->stepByConstantSpeed(model_parameter->DeltaTime);
            p_part->stepTry(model_parameter->DeltaTime, p_part->tangent*10.0);
        }
        else if (p_part->isAnchored)
        {
            // a constrained particle
            zVec2 vel = p_part->velocity * (1.0*CONSTRAIN_SPEED / p_part->velocity.length());
            p_part->stepTry(model_parameter->DeltaTime, vel);
        }
        else
        {
            // a regular particle
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
    }
}

