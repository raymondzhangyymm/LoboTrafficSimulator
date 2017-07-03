#ifndef ROAD_H
#define ROAD_H

#include "global.h"
#include "BezierSequence.h"
#include "TriangleMesh.h"
#include "CellSequence.h"
#include "SystemParameter.h"
#include "Particle.h"
#include "CarParticle.h"
#include "randomer.h"
#include "HashTableX.h"

class Road
{
public:
    // instantate the road
    Road(){}
    Road(SystemParameter* p_SysParam_, RoadLink *p_road_link_, int road_idx);

    SystemParameter *p_SysParam;

    // The arttributes of the road
    int index;
    long long id;
    long long osm_id;
    int num_lanes;
    MapElementType roadtype;
    HighwayType roadsubtype;

    bool isOneway;
    bool isVisible;
    bool isConstrained;
    bool isInflow[2]; // two directions

    double speed_limit;
    double speed_constrain;
    double inflow_rate[2]; // two directions

    // The geometry of the road
    RoadLink *p_road_link;
    BezierSequence road_curve;
    double road_length;
    double road_width;
    double cell_length;
    double lane_width;

    CellSequence bound_area, road_area;

    // the drain mesh, road side boundary and the solid or dash lanes for visualization.
    TriangleMesh drain_1, drain_2;
    PointArray visualroad_side_left;
    PointArray visualroad_side_right;
    vector<PointArray> visuallane_solid;
    vector<PointArray> visuallane_dash;

    // The lanes of the road are for 1D simulation.
    vector<TriangleMesh*> lane_density_meshes;
    vector<TriangleMesh*> lane_velocity_meshes;
    vector<double> bias_distances;
    double field_mesh_bottom, field_mesh_top;

    // The particle lists.
    vector<vector<Particle*>> particle_lists;
    int num_particles;
    long long max_particleID;

    // The virtual car particles.
    vector<vector<CarParticle*>> car_lists;
    int num_cars;
    long long max_carID;

    vector<HashTableX*> hash_tables;
    vector<Cell> *block_list;
    vector<int> lane_direction;

    vector<vector<Node>> sensor_lists;
    void createSensors();
    void updateSensors();

    void initRoadAttributes();
    void initRoadGeometry();
    void initRoadSimulation();
    void initRoad();

    void stepX();
    void stepXNOFieldMeshes();
    void stepXNOCars();
    void stepXNOFieldMeshesCars();

    vector<Trajectory*> traj_list;
    void buildRandomTrajectories();
    void stepXByTrajectories();

private:
    vector<double> production_particle;
    vector<double> production_car;

    // Initialize the road curve from the input road nodes.
    void buildRoadCurve(vector<RoadNode*> &p_roadnode_list);

    // This function remove NULL and re-assign id for each particle/car.
    void cleanLists();

    void buildDrain();
    void buildBoundary();
    void buildCells();
    void buildVisualRoadAndLanes();

    void produceParticles();
    void produceCarsWithTrajectory();
    void consumeParticles();

    void createParticles();
    void updateNeighborsAndDensityPressure();
    void updateParticles();

    void buildLaneMeshes();
    void updateLaneMeshes();

    void createCarParticles();
    void updateCarParticles();
    void updateCarsWithTrajectory();

    double getDensityByLaneMesh(double lane_id, double x_pos);
    double getVelocityByLaneMesh(double lane_id, double x_pos);
    void getDensityVelocityByInterpolation(double lane_id, double x_pos, double &dd, double &vv);

    // Compute the traffic forces for all particles
    void computeXTrafficForces();

    // Compute the sub forces for all particles
    void computeXPressureForces();
    void computeXPressureForcesHalfRegion();
    void computeXViscosityForces();
    void computeXViscosityForcesHalfRegion();
    void computeXDampingForces();  // donot need neighbers
    void computeXExternalForces();  // donot need neighbers

    // For Lanechanging.
    void handleLaneChangingParticles();
    void handleLaneChangingCarParticles();

    // This function duplicates a new particle and add to lane lane_id
    void duplicateParticleForLaneChanging(int lane_id, Particle *p_part);

    // This function duplicates a new car and add to lane lane_id
    // Could fail because of collision to other cars in the lane
    void duplicateCarParticleForLaneChanging(int lane_id, CarParticle *p_car, int dd);

    void cleanParticles();
    void cleanCars();

    void resetInflowCounter(int lane_id);
};

#endif // ROAD_H
