#ifndef KERNELMONITOR
#define KERNELMONITOR

#include <QGLViewer/qglviewer.h>
#include <QMouseEvent>
#include <math.h>
#include <ParticleSystemTraffic.h>

class KernelMonitor : public QGLViewer
{
public :
    KernelMonitor(QWidget *parent);
    ~KernelMonitor();

    // 0:poly6, 1:spiky, 2:viscosity.
    // 3:poly6_half, 4:spiky_half, 5:viscosity_half
    int display_mode;

    bool isEditing;

    double support;

    void init(double support_,
              double particle_radius_, zVec4 particle_color_, TriangleMesh part_mesh);

    void updateSupport(double _h);

    void updateKernelShape();
    void restoreKernelShape();

protected :
    virtual void init();
    virtual void draw();
    virtual void drawWithNames();

    virtual void mousePressEvent(QMouseEvent* e);
    virtual void mouseMoveEvent(QMouseEvent* e);

private:
    vector<TriangleMesh> kernel_mesh_list;
    zVec4 kernel_mesh_color;

    double particle_radius;
    zVec4 particle_color;
    TriangleMesh particle_mesh;

    double kernel_plane_height;

    TriangleMesh active_mesh;
    zVec3 active_ball;
    double active_region_width;

    // Particle's Lighting Materials.
    GLfloat material_ambient[4];
    GLfloat material_diffuse[4];
    GLfloat material_specular[4];
    GLfloat material_shininess[1];

    double axis_length;
    double scale_factor_poly;
    double scale_factor_spiky;
    double scale_factor_viscosity;

    void drawOneParticle(Particle* p_particle, zVec4 color_);
    void drawSphere(zVec3 pos, zVec4 color_);

    void drawKernelMesh(TriangleMesh &mesh);
    void drawActiveMesh();
    void drawActiveBall();

    void buildAllKernelMesh();
    void updateActiveMesh(int idx_mesh);

    //----------------------------------------------------------------------------------
    // Functions and variables for computing kernel meshes
    //----------------------------------------------------------------------------------
    double curr_support;
    double max_poly, max_spikygradient, max_viscositylaplacian;

    void updateSupportByHeightPoly(double _height);
    void updateSupportByHeightSpikyGradient(double _height);
    void updateSupportByHeightViscosityLaplacian(double _height);

    double computePoly(zVec2 r);
    double computeSpikyGradient(zVec2 r);
    double computeViscosityLaplacian(zVec2 r);

    typedef double (KernelMonitor::*KernelFunction)(zVec2);

    // The number of nodes on each circle is identical.
    void buildKernelMesh(TriangleMesh &mesh, KernelFunction kernel_fun, double scale_factor);
    void buildKernelMeshHalf(TriangleMesh &mesh, KernelFunction kernel_fun, double scale_factor);

    // The number of nodes on each circle decreases when approaching to the center.
    void buildKernelMeshCoarse(TriangleMesh &mesh, KernelFunction kernel_fun, double scale_factor);
    void buildKernelMeshHalfCoarse(TriangleMesh &mesh, KernelFunction kernel_fun, double scale_factor);
};

#endif
