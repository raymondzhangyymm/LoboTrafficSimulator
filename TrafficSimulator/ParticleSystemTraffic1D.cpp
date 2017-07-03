#include "ParticleSystemTraffic1D.h"

ParticleSystemTraffic1D::ParticleSystemTraffic1D(SystemParameter *model_parameter_)
    : ParticleSystemTraffic(model_parameter_)
{

}

void ParticleSystemTraffic1D::initSystem()
{    
    buildRoads();

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

    cout << "===========================================================================\n"
         << "Running on mode 1 : 1D simulation.\n" << std::flush;
}

void ParticleSystemTraffic1D::step()
{
    if (isUpdatingFieldMeshes)
    {
        for (int i=0; i<num_roads; ++i)
        {
            Road *p_road = road_list[i];
            p_road->stepX();
        }
    }
    else
    {
        for (int i=0; i<num_roads; ++i)
        {
            Road *p_road = road_list[i];
            p_road->stepXNOFieldMeshes();
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

    simulation_time += model_parameter->DeltaTime;
}

//====================================================================================================
void ParticleSystemTraffic1D::updateObservation()
{
    int num_nodes = observation_list.size();
    for (int i=0; i<num_nodes; ++i)
    {
        Node *p_node = observation_list[i];

        if (p_node->isBad)
        {
            p_node->density = 0;
            p_node->normalized_density = 0;

            p_node->velocity = 0;
            p_node->normalized_velocity = 0;
        }
        else
        {
            // Get and re-build the hash table.
            Road *p_road = road_list[p_node->road_id];
            vector<Particle*> *p_list = &p_road->particle_lists[p_node->lane_id];
            HashTableX *hash_table_x = p_road->hash_tables[p_node->lane_id];
            hash_table_x->buildHashTableX(p_list, model_parameter->SupportRadius);

            // find neighbours by the position
            vector<Particle*> neighbors;
            hash_table_x->getNeighborsByXPosition(p_node->x_position, neighbors);

            // interpolate the density and the speed
            double density = 0, velocity = 0;
            int num_neigh = neighbors.size();
            for (int j=0; j<num_neigh; ++j)
            {
                Particle* p_neigh = neighbors[j];

                double k = Kernel::getPoly1D(p_node->x_position - p_neigh->x_position);
                density += p_neigh->mass * k;
                velocity += p_neigh->x_velocity * (p_neigh->mass * k / p_neigh->density);
            }

            p_node->density = density;
            p_node->normalized_density = p_node->density / model_parameter->MaxDensity;

            p_node->velocity = velocity;
            p_node->normalized_velocity = p_node->velocity / model_parameter->MaxVelocity;
        }
    }
}

void ParticleSystemTraffic1D::createObservationPoint()
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

    // find the cell where the observation is in
    vector<int> roadcell_id_list;
    zVec2 pos(p_node->position.x, p_node->position.y);
    if (locateCarParticlePosition(pos, roadcell_id_list))
    {
        p_node->isBad = false;
        p_node->road_id = roadcell_id_list[0];
        p_node->cell_id = roadcell_id_list[1];

        // get the cell
        Road *p_road = road_list[p_node->road_id];
        Cell *p_cell = p_road->road_area.cells[p_node->cell_id];
        zVec2 n0 = (p_cell->v[0] + p_cell->v[3]) / 2.0;
        zVec2 n1 = (p_cell->v[1] + p_cell->v[2]) / 2.0;

        // project the obs. point to the center line n0n1 to determine the lane id
        zVec2 a = pos - n0;
        zVec2 b = n1 - n0;
        p_node->lane_id = a.dotproduct(b) / b.length() * p_road->num_lanes;
        if (p_node->lane_id >= p_road->num_lanes)
        {
            p_node->lane_id = p_road->num_lanes-1;
        }

        // determine its x_position
        p_node->x_position = p_road->road_curve.getXPositionByPoint(pos);
    }
    else
    {
        // the observation is not in any road cell.
        p_node->isBad = true;
    }

    observation_list.push_back(p_node);
}

void ParticleSystemTraffic1D::moveObservationPoint(Node *p_node, zVec2 pos)
{
    p_node->position.x = pos.x;
    p_node->position.y = pos.y;

    // update the road cell where the observation is in
    vector<int> roadcell_id_list;
    zVec2 nodepos(p_node->position.x, p_node->position.y);
    if (locateCarParticlePosition(nodepos, roadcell_id_list))
    {
        p_node->isBad = false;
        p_node->road_id = roadcell_id_list[0];
        p_node->cell_id = roadcell_id_list[1];

        // get the cell
        Road *p_road = road_list[p_node->road_id];
        Cell *p_cell = p_road->road_area.cells[p_node->cell_id];
        zVec2 n0 = (p_cell->v[0] + p_cell->v[3]) / 2.0;
        zVec2 n1 = (p_cell->v[1] + p_cell->v[2]) / 2.0;

        // project the obs. point to the center line n0n1 to determine the lane id
        zVec2 a = nodepos - n0;
        zVec2 b = n1 - n0;
        p_node->lane_id = a.dotproduct(b) / b.length() * p_road->num_lanes;
        if (p_node->lane_id >= p_road->num_lanes)
        {
            p_node->lane_id = p_road->num_lanes-1;
        }

        // determine its x_position
        p_node->x_position = p_road->road_curve.getXPositionByPoint(nodepos);
    }
    else
    {
        // the observation is not in any road cell.
        p_node->isBad = true;
    }
}
