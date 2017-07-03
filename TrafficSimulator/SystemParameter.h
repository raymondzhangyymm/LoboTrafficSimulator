#ifndef SystemParameter_H
#define SystemParameter_H

#include "global.h"
#include "BezierSequence.h"
#include "TriangleMesh.h"
#include "MapParser.h"

class SystemParameter
{
public:
    SystemParameter(const char *config_filename);

    bool SuccessfulInit;

    int SimulationMode;  // 2:2D, 1:1D, 0:trajectory following, 9:experiments
    int ExperimentType;

    double DeltaTime;    // The delta time for the fluid simulation.
    double ParticleMass;    // The mass of each particle of the fluid in Kg.
    double ParticleRadius;    // The size of each particle.
    double SupportRadius;    // The support radius for the kernels in m.
    double SupportRadiusHK;    // The support radius for the kernels in m.
    double LaneChangingDensityGap;
    double LaneChangingVelocityGap;
    double RestDensity;
    double MaxDensity;
    double MaxVelocity;
    bool UseHalfKernel;
    string mapfilename;
    string configuration_dir;

    // The interactive parameters
    double GasStiffness;    // The gass stiffness constant of the fluid in J.
    double Viscosity;    // The viscosity coeficient of the fluid in Pa * s.
    double CoeffDampingForce;    // The damping of the fluid.
    double CoeffFieldForce;    // The speed limit for the vehicles.

    bool UseViscosityForce;
    bool UsePressureForce;
    bool UseExternalForce;
    bool AddConstrain;

    bool OutputCellSensorDensityVelocity;
    bool OutputEntireDensityVelocity;
    bool OutputDensityVelocityMesh;
    bool OutputScreenshot;

    // The meshes used for rendering.
    TriangleMesh circle_mesh;
    TriangleMesh sphere_mesh;
    TriangleMesh block_mesh;
    vector<TriangleMesh*> car_mesh_list;
    vector<zVec4> car_color_list;
    vector<zVec4> observation_color_list;

    vector<vector<Bezier>> bezier_array_list;
    vector<int> lane_number_list;

    // The factors map raw GIS data to opengl display.
    MapParser *real_map;

private:
    void loadMeshesAndColors();
    void loadConfigFile(const char *filename);

    void setExperimentParameters();
};

#endif
