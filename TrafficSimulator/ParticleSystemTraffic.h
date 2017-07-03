#ifndef ParticleSystemTraffic_H
#define ParticleSystemTraffic_H

#include "global.h"
#include "SystemParameter.h"
#include "HashTable.h"
#include "Road.h"
#include "TriangleMesh.h"
#include "CarParticle.h"
#include <QImage>
#include <QDir>
#include "randomer.h"

class ParticleSystemTraffic
{
public:
    ParticleSystemTraffic(SystemParameter *model_parameter_);

    SystemParameter* model_parameter;

    Randomer *p_randomer;

    vector<Road*> road_list;
    int num_roads;
    zVec2 min_corner, max_corner;

    vector<Particle*> particle_list;
    int num_particles;
    long long max_particleID;

    vector<CarParticle*> car_list;
    int num_cars;
    long long max_carID;

    bool isUpdatingFieldMeshes;
    bool isUpdatingCars;
    bool isUpdatingParticles;
    bool isTwoDimension;

    // The associated field mesh for visualization.
    // each node has a fixed 2D coordinates fixed.
    // the z coordinate is determined by other quantities like density/speed/flow
    TriangleMesh field_mesh_density;
    TriangleMesh field_mesh_velocity;
    double field_mesh_bottom, field_mesh_top;
    double velocity_average, velocity_peak;
    double density_average, density_peak;

    // The observation points list and manipulation functions
    vector<Node*> observation_list;
    virtual void createObservationPoint();
    virtual void moveObservationPoint(Node *p_node, zVec2 pos);
    virtual void updateObservation();
    void deleteObservationPoint(Node *p_node);

    // The block areas list and manipulation functions
    vector<Cell> block_list;
    void createBlockArea();
    void deleteBlockArea(int idx);
    void moveBlockArea(int idx, zVec2 pos);
    zVec2 getTangentFromRoadNet(zVec2 pos);

    // The sensor list corresponding to the first road
    vector<vector<Node>> sensor_lists;
    void createSensors();
    void updateSensors();

    // record the total simulation time in second
    char simulation_timestamp[128];
    double simulation_time;
    vector<double> production_particle;
    vector<double> production_car;

    virtual void initSystem();
    virtual void step();
    virtual void reset();

    // This function builds the roads for their shapes/boundary.
    virtual void buildRoads();

    // synchronize particles/cars list from all roads.
    void synchronizeParticles();
    void synchronizeCars();

    // Locate the position. Find out all boundary cells where it is in.
    // Return an integer list patterned by road_id and cell_id pairs.
    bool locateParticlePosition(zVec2 pos, vector<int> &rc_id_list);
    bool locateCarParticlePosition(zVec2 pos, vector<int> &rc_id_list);

    // This hash table is used to perform nearest-neighbour-seach (NNS) algorithm.
    HashTable* hash_table;

    // This function links the overlapping cells of the roads.
    void buildRoadsOverlappingCells();

    // This function builds the field meshes by uniformly distributed nodes.
    void buildFieldMeshesByGrids();

    // This function builds the field meshes by nodes along each road.
    void buildFieldMeshes();

    // This function creates particles uniformly distributed within the overlapping area
    // of the road boundary and the big bounding box of the roads.
    void createParticlesByGrids();

    // This function creates particles along each road.
    void createParticles();

    void createCarParticles();

    void produceParticles();
    void consumeParticles();

    void updateParticleNeighbors();
    void updateParticleDensityPressure();
    void updateFieldMeshes();
    virtual void updateParticles();

    // Handle the collison between the particle/car and the cell
    // If successed, the road id and cell id might be updated.
    bool handleCollisionBetweenCellParticle(Particle* p_part);
    bool handleCollisionBetweenCellCarByReflection(CarParticle* p_car);
    bool handleCollisionBetweenCellCarBySpring(CarParticle* p_car);

    // Handle the collison between the cars
    void handleCollisionBetweenCars();

    // Compute the traffic forces for all particles
    void computeTrafficForces();

    // Compute the sub forces for all particles
    void computePressureForces();
    void computePressureForcesHalfRegion();
    void computeViscosityForces();
    void computeViscosityForcesHalfRegion();
    void computeDampingForces();  // donot need neighbers
    void computeExternalForces();  // donot need neighbers

    void updateSpringDisplacementOfCars();
    void updateDensitiesAndPressures();

    void updateCarParticles();
    bool detectCarCollision(CarParticle *p_car);
    bool detectCarCollision(CarParticle *p_car, vector<CarParticle*> &collision_car_list);
    void respondCarCollision(CarParticle *p_car, vector<CarParticle*> &collision_car_list, double t);
    bool overlapBbox(vector<zVec2> &bb0, vector<zVec2> &bb1);

    // Check if a position is in boundary/road area.
    bool isInBoundaryArea(zVec2 pos);
    bool isInRoadArea(zVec2 pos);

    // Check if two bboxes are overlapping using SAT or Minkowski Sum.
    // The Minkowski method can return the minimum translational distance vector.
    bool isOverlappingBySAT(zVec2 bbox_1[], zVec2 bbox_2[]);
    bool isOverlappingByMinkowski(zVec2 bbox_1[], zVec2 bbox_2[], zVec2 &mtd);

    void resetInflowCounter(Road *p_road);
};

#endif
