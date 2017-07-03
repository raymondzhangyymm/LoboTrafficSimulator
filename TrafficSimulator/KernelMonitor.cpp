#include "KernelMonitor.h"

// Constructor must call the base class constructor.
KernelMonitor::KernelMonitor(QWidget *parent) : QGLViewer(parent)
{
//    kernel_mesh_color[0] = 1;
//    kernel_mesh_color[1] = 1;
//    kernel_mesh_color[2] = 1;
//    kernel_mesh_color[3] = 0.8;
    kernel_mesh_color[0] = 0;
    kernel_mesh_color[1] = 0;
    kernel_mesh_color[2] = 1;
    kernel_mesh_color[3] = 1;

    particle_color[0] = 1;
    particle_color[1] = 0;
    particle_color[2] = 0;
    particle_color[3] = 1;

    material_ambient[0] = 0.5;
    material_ambient[1] = 0.5;
    material_ambient[2] = 0.5;
    material_ambient[3] = 1.0;

    material_diffuse[0] = 0.5;
    material_diffuse[1] = 0.5;
    material_diffuse[2] = 0.5;
    material_diffuse[3] = 1.0;

    material_specular[0] = 0.5;
    material_specular[1] = 0.5;
    material_specular[2] = 0.5;
    material_specular[3] = 1.0;

    material_shininess[0] = 100.0;

    QGLFormat format;
    format.setDoubleBuffer(true);
    format.setDepth(false);
    format.setAlpha(false);
    format.setSampleBuffers(true);
    format.setSamples(16);
    this->setFormat(format);

    init();
}
KernelMonitor::~KernelMonitor()
{
}

//=========================================================================
// Functions for the main viewer.
//=========================================================================
void KernelMonitor::init()
{
    glShadeModel(GL_FLAT);

    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);

    // set the light
    GLfloat lightAmbient[] = {0.5, 0.5, 0.5, 1};
    GLfloat lightDiffuse[] = {0.5, 0.5, 0.5, 1};
    GLfloat lightPosition[] = {1.0, -3.0, 6, 0};

    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);	//Setup The Ambient Light
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);	//Setup The Diffuse Light
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);	//Position The Light
    glEnable(GL_LIGHT0);

//    lightPosition[3] = -3;
//    glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmbient);	//Setup The Ambient Light
//    glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffuse);	//Setup The Diffuse Light
//    glLightfv(GL_LIGHT1, GL_POSITION, lightPosition);	//Position The Light
//    glEnable(GL_LIGHT1);

    glEnable(GL_LIGHTING);

    glClearColor(1.0,1.0,1.0,0);
    glClear(GL_COLOR_BUFFER_BIT);
}

void KernelMonitor::init(double support_,
                         double particle_radius_, zVec4 particle_color_, TriangleMesh part_mesh)
{
    particle_radius = particle_radius_;
    particle_color = particle_color_;
    particle_mesh = part_mesh;

    double scene_radius = DEFAULT_LANE_WIDTH * 5;
    this->camera()->setSceneRadius(scene_radius);
    this->camera()->setOrientation(0,-M_PI/3);
    this->camera()->setZNearCoefficient(0.00001);
    this->camera()->setZClippingCoefficient(1000.0);
    this->camera()->showEntireScene();

    // the axis length determines a better visualization, set the real radius to 80%.
    axis_length = 0.6 * scene_radius;

    kernel_plane_height = 1.2 * particle_radius;

    display_mode = 0;

    isEditing = false;

    kernel_mesh_list.resize(6);

    support = support_;

    max_poly = 4.0 / (PI * support * support);
    max_spikygradient = 30.0 / (PI * support * support * support);
    max_viscositylaplacian = 20.0 / (PI * support * support * support * support);

    scale_factor_poly = axis_length / max_poly;
    scale_factor_spiky = axis_length / max_spikygradient;
    scale_factor_viscosity = axis_length / max_viscositylaplacian;

    buildAllKernelMesh();

    updateGL();
}

//=========================================================================
// Functions for interactive operations.
//=========================================================================
void KernelMonitor::mousePressEvent(QMouseEvent* e)
{
    if ((e->button() == Qt::LeftButton) && (e->modifiers() == Qt::ShiftModifier))
    {
        select(e->pos());
    }
    else
    {
        QGLViewer::mousePressEvent(e);
    }
}

void KernelMonitor::mouseMoveEvent(QMouseEvent* e)
{
    if (isEditing && selectedName() != -1)
    {
        qglviewer::Vec orig, dir;
        camera()->convertClickToLine(e->pos(), orig, dir);

        double t = ( active_ball.x*dir.x - dir.x*orig.x
                   + active_ball.y*dir.y - dir.y*orig.y)
                   / (dir.x*dir.x + dir.y*dir.y);

        active_ball.z = dir.z * t + orig.z;

        if (active_ball.z < 1.1*kernel_plane_height)
        {
            active_ball.z = 1.1*kernel_plane_height;
        }

        if (display_mode == 0)
        {
            updateSupportByHeightPoly
                    ((active_ball.z - kernel_plane_height) * scale_factor_poly);
        }
        else if (display_mode == 1)
        {
            updateSupportByHeightSpikyGradient
                    ((active_ball.z - kernel_plane_height) * scale_factor_spiky);
        }
        else if (display_mode == 2)
        {
            updateSupportByHeightViscosityLaplacian
                    ((active_ball.z - kernel_plane_height) * scale_factor_spiky);
        }
        else if (display_mode == 3)
        {
            updateSupportByHeightPoly
                    ((active_ball.z - kernel_plane_height) * 2*scale_factor_poly);
        }
        else if (display_mode == 4)
        {
            updateSupportByHeightSpikyGradient
                    ((active_ball.z - kernel_plane_height) * 2*scale_factor_spiky);
        }
        else if (display_mode == 5)
        {
            updateSupportByHeightViscosityLaplacian
                    ((active_ball.z - kernel_plane_height) * 2*scale_factor_spiky);
        }

        buildAllKernelMesh();

        updateGL();
    }
    else
    {
        QGLViewer::mouseMoveEvent(e);
    }
}

//============================================================================
// Functions for Drawing.
//============================================================================
void KernelMonitor::draw()
{
    if (kernel_mesh_list.size() == 0)
    {
        return;
    }

    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);

    setGridIsDrawn(true);

    drawKernelMesh(kernel_mesh_list[display_mode]);
//    if (isEditing)
//    {
//        drawActiveMesh();
//        drawActiveBall();
//    }
}

void KernelMonitor::drawWithNames()
{
    if (isEditing)
    {
        glPushName(0);
        drawActiveBall();
        glPopName();
    }
}

void KernelMonitor::drawKernelMesh(TriangleMesh &mesh)
{
    // Setting Lighting materials
//    glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
//    glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
//    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
//    glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);

    glDisable(GL_LIGHTING);

    glColor4f(kernel_mesh_color.x, kernel_mesh_color.y,
              kernel_mesh_color.z, kernel_mesh_color.a);
    glBegin(GL_TRIANGLES);
    int num_face = mesh.triangle_list.size();
    for (int i=0; i<num_face; ++i)
    {
        Triangle* p_tri = &mesh.triangle_list[i];

//        // one side of the triangle
//        glNormal3f(p_tri->v0->norm.x, p_tri->v0->norm.y, p_tri->v0->norm.z);
//        glVertex3f(p_tri->v0->position.x, p_tri->v0->position.y, p_tri->v0->position.z);

//        glNormal3f(p_tri->v1->norm.x, p_tri->v1->norm.y, p_tri->v1->norm.z);
//        glVertex3f(p_tri->v1->position.x, p_tri->v1->position.y, p_tri->v1->position.z);

//        glNormal3f(p_tri->v2->norm.x, p_tri->v2->norm.y, p_tri->v2->norm.z);
//        glVertex3f(p_tri->v2->position.x, p_tri->v2->position.y, p_tri->v2->position.z);

//        // the other side of the triangle
//        glNormal3f(-p_tri->v0->norm.x, -p_tri->v0->norm.y, -p_tri->v0->norm.z);
//        glVertex3f(p_tri->v0->position.x, p_tri->v0->position.y, p_tri->v0->position.z);

//        glNormal3f(-p_tri->v2->norm.x, -p_tri->v2->norm.y, -p_tri->v2->norm.z);
//        glVertex3f(p_tri->v2->position.x, p_tri->v2->position.y, p_tri->v2->position.z);

//        glNormal3f(-p_tri->v1->norm.x, -p_tri->v1->norm.y, -p_tri->v1->norm.z);
//        glVertex3f(p_tri->v1->position.x, p_tri->v1->position.y, p_tri->v1->position.z);

        // one side of the triangle
        glNormal3f(p_tri->norm.x, p_tri->norm.y, p_tri->norm.z);
        glVertex3f(p_tri->v0->position.x, p_tri->v0->position.y, p_tri->v0->position.z);
        glVertex3f(p_tri->v1->position.x, p_tri->v1->position.y, p_tri->v1->position.z);
        glVertex3f(p_tri->v2->position.x, p_tri->v2->position.y, p_tri->v2->position.z);

        // the other side of the triangle
        glNormal3f(-p_tri->norm.x, -p_tri->norm.y, -p_tri->norm.z);
        glVertex3f(p_tri->v0->position.x, p_tri->v0->position.y, p_tri->v0->position.z);
        glVertex3f(p_tri->v2->position.x, p_tri->v2->position.y, p_tri->v2->position.z);
        glVertex3f(p_tri->v1->position.x, p_tri->v1->position.y, p_tri->v1->position.z);
    }
    glEnd();

    glLineWidth(1.5);
    glColor4f(0.8, 0.8, 0.8, 1.0);
    glBegin(GL_LINES);
    for (int i=0; i<num_face; ++i)
    {
        Triangle* p_tri = &mesh.triangle_list[i];

        glNormal3f(p_tri->v0->norm.x, p_tri->v0->norm.y, p_tri->v0->norm.z);
        glVertex3f(p_tri->v0->position.x, p_tri->v0->position.y, p_tri->v0->position.z);

        glNormal3f(p_tri->v1->norm.x, p_tri->v1->norm.y, p_tri->v1->norm.z);
        glVertex3f(p_tri->v1->position.x, p_tri->v1->position.y, p_tri->v1->position.z);

        glNormal3f(p_tri->v2->norm.x, p_tri->v2->norm.y, p_tri->v2->norm.z);
        glVertex3f(p_tri->v2->position.x, p_tri->v2->position.y, p_tri->v2->position.z);

        glNormal3f(p_tri->v0->norm.x, p_tri->v0->norm.y, p_tri->v0->norm.z);
        glVertex3f(p_tri->v0->position.x, p_tri->v0->position.y, p_tri->v0->position.z);
    }
    glEnd();

    glEnable(GL_LIGHTING);
}

void KernelMonitor::drawActiveMesh()
{
    // Setting Lighting materials
    glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);

    glColor4f(kernel_mesh_color.x, kernel_mesh_color.y, 1.0, 0.2);
    glBegin(GL_TRIANGLES);
    int num_face = active_mesh.triangle_list.size();
    for (int i=0; i<num_face; ++i)
    {
        Triangle* p_tri = &active_mesh.triangle_list[i];

        // one side of the triangle
        glNormal3f(p_tri->v0->norm.x, p_tri->v0->norm.y, p_tri->v0->norm.z);
        glVertex3f(p_tri->v0->position.x, p_tri->v0->position.y, p_tri->v0->position.z);

        glNormal3f(p_tri->v1->norm.x, p_tri->v1->norm.y, p_tri->v1->norm.z);
        glVertex3f(p_tri->v1->position.x, p_tri->v1->position.y, p_tri->v1->position.z);

        glNormal3f(p_tri->v2->norm.x, p_tri->v2->norm.y, p_tri->v2->norm.z);
        glVertex3f(p_tri->v2->position.x, p_tri->v2->position.y, p_tri->v2->position.z);

        // the other side of the triangle
        glNormal3f(-p_tri->v0->norm.x, -p_tri->v0->norm.y, -p_tri->v0->norm.z);
        glVertex3f(p_tri->v0->position.x, p_tri->v0->position.y, p_tri->v0->position.z);

        glNormal3f(-p_tri->v2->norm.x, -p_tri->v2->norm.y, -p_tri->v2->norm.z);
        glVertex3f(p_tri->v2->position.x, p_tri->v2->position.y, p_tri->v2->position.z);

        glNormal3f(-p_tri->v1->norm.x, -p_tri->v1->norm.y, -p_tri->v1->norm.z);
        glVertex3f(p_tri->v1->position.x, p_tri->v1->position.y, p_tri->v1->position.z);
    }

    glEnd();
}

void KernelMonitor::drawActiveBall()
{
    // Setting Lighting materials
    glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);

    glColor4f(0.4, 0.4, 0.4, 1.0);

    //draw a ball for setting
    glPushMatrix();
    glTranslatef(active_ball.x, active_ball.y, active_ball.z);

    GLUquadricObj *quadObj;
    quadObj = gluNewQuadric();
    gluSphere(quadObj, axis_length*0.1, 32, 32);

    glPopMatrix();
}

void KernelMonitor::drawOneParticle(Particle* p_particle, zVec4 color)
{
    // Set lighting materials
    glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);

    // Set color
    glColor4f(color.x, color.y, color.z, color.a);

    glPushMatrix();
    glTranslatef(p_particle->position[0], p_particle->position[1], 0.0);

    // Drawing particle
    GLUquadricObj *quadObj;
    quadObj = gluNewQuadric ();
    gluSphere(quadObj, p_particle->sphereRadius, 16, 16);

    glPopMatrix();
}

void KernelMonitor::drawSphere(zVec3 pos, zVec4 color)
{
    // Setting Lighting materials
    glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);

    glColor4f(color.x, color.y, color.z, color.a);
    glBegin(GL_TRIANGLES);
    int num_face = particle_mesh.triangle_list.size();
    for (int i=0; i<num_face; ++i)
    {
        Triangle *p_tri = &particle_mesh.triangle_list[i];

        zVec3 n0 = p_tri->v0->position + pos;
        zVec3 n1 = p_tri->v1->position + pos;
        zVec3 n2 = p_tri->v2->position + pos;

        //glNormal3f(p_tri->norm.x, p_tri->norm.y, p_tri->norm.z);
        glNormal3f(p_tri->v0->norm.x, p_tri->v0->norm.y, p_tri->v0->norm.z);
        glVertex3f(n0.x, n0.y, n0.z);
        glNormal3f(p_tri->v1->norm.x, p_tri->v1->norm.y, p_tri->v1->norm.z);
        glVertex3f(n1.x, n1.y, n1.z);
        glNormal3f(p_tri->v2->norm.x, p_tri->v2->norm.y, p_tri->v2->norm.z);
        glVertex3f(n2.x, n2.y, n2.z);
    }
    glEnd();
}

void KernelMonitor::buildAllKernelMesh()
{
//    buildKernelMesh(kernel_mesh_list[0], &KernelMonitor::computePoly, scale_factor_poly);
//    buildKernelMesh(kernel_mesh_list[1], &KernelMonitor::computeSpikyGradient, scale_factor_spiky);
//    buildKernelMesh(kernel_mesh_list[2], &KernelMonitor::computeViscosityLaplacian, scale_factor_viscosity);
//    buildKernelMeshHalf(kernel_mesh_list[3], &KernelMonitor::computePoly, scale_factor_poly);
//    buildKernelMeshHalf(kernel_mesh_list[4], &KernelMonitor::computeSpikyGradient, scale_factor_spiky);
//    buildKernelMeshHalf(kernel_mesh_list[5], &KernelMonitor::computeViscosityLaplacian, scale_factor_viscosity);
    buildKernelMeshCoarse(kernel_mesh_list[0], &KernelMonitor::computePoly, scale_factor_poly);
    buildKernelMeshCoarse(kernel_mesh_list[1], &KernelMonitor::computeSpikyGradient, scale_factor_spiky);
    buildKernelMeshCoarse(kernel_mesh_list[2], &KernelMonitor::computeViscosityLaplacian, scale_factor_viscosity);
    buildKernelMeshHalfCoarse(kernel_mesh_list[3], &KernelMonitor::computePoly, scale_factor_poly);
    buildKernelMeshHalfCoarse(kernel_mesh_list[4], &KernelMonitor::computeSpikyGradient, scale_factor_spiky);
    buildKernelMeshHalfCoarse(kernel_mesh_list[5], &KernelMonitor::computeViscosityLaplacian, scale_factor_viscosity);
}

void KernelMonitor::updateActiveMesh(int idx_mesh)
{
    active_mesh = kernel_mesh_list[idx_mesh];

    Node *p_last_node = active_mesh.node_list[active_mesh.node_list.size()-1];
    active_ball = p_last_node->position;
}

void KernelMonitor::updateKernelShape()
{
//    updateSupport();
    buildAllKernelMesh();

}

void KernelMonitor::restoreKernelShape()
{

}

//============================================================================
// Functions for computing kernel mesh.
//============================================================================
void KernelMonitor::updateSupport(double support_)
{
    support = support_;

    max_poly = 4.0 / (PI * support * support);
    max_spikygradient = 30.0 / (PI * support * support * support);
    max_viscositylaplacian = 20.0 / (PI * support * support * support * support);

//    zVec2 zz(0,0);
//    max_poly = Kernel::getPoly(zz);
//    zVec2 zz1 = Kernel::getSpikyGradient(zz);
//    max_spikygradient = zz1.length();
//    max_viscositylaplacian = Kernel::getViscosityLaplacian(zz);

    buildAllKernelMesh();

    updateGL();
}

void KernelMonitor::updateSupportByHeightPoly(double height_)
{
    updateSupport(sqrt(4.0 / (PI * height_)));
}

void KernelMonitor::updateSupportByHeightSpikyGradient(double height_)
{
    updateSupport(cbrt(30.0 / (PI * height_)));
}

void KernelMonitor::updateSupportByHeightViscosityLaplacian(double height_)
{
    updateSupport(sqrt(sqrt(20.0 / (PI * height_))));
}

double KernelMonitor::computePoly(zVec2 r)
{
//    double length2 = r.x*r.x + r.y*r.y;
//    double support2 = support * support;

//    if (length2 < support2)
//    {
//        double aux = 1.0 - length2 / support2;
//        return max_poly * aux * aux * aux;
//    }
//    else
//    {
//        return 0.0;
//    }
    return Kernel::getPoly(r);
}

double KernelMonitor::computeSpikyGradient(zVec2 r)
{
    double length = r.length();

    if (length < support)
    {
        double aux = 1.0 - length / support;
        return max_spikygradient * aux * aux;
    }
    else
    {
        return 0.0;
    }
//    zVec2 vv = Kernel::getSpikyGradient(r);
//    return vv.length();
}

double KernelMonitor::computeViscosityLaplacian(zVec2 r)
{
//    double length = r.length();

//    if (length < support)
//    {
//        double aux = 1.0 - length / support;
//        return max_viscositylaplacian * aux;
//    }
//    else
//    {
//        return 0.0;
//    }
    return Kernel::getViscosityLaplacian(r);
}

// This function build a triangle mesh for the poly6 kernel.
// The projection of mesh to the xy plane is a circle centered at (0,0) with radius support.
void KernelMonitor::buildKernelMesh(TriangleMesh &mesh, KernelFunction kernel_fun, double scale_factor)
{
    // compute the new support radius
    double radius_max = support;
    int num_circle = 20;
    int num_slice = 100;

    // the disk will be divided into 100 pizza slices and 10 circles
    double alpha_step = 2.0 * PI / double(num_slice);
    double radius_step = radius_max / double(num_circle);

    //------------------------------------------------------------------
    // build the nodes
    //------------------------------------------------------------------
    mesh.node_list.clear();
    int node_count = 0;
    double radius = radius_max;
    Node *p_node;
    for (int i=0; i<num_circle; ++i)
    {
        double alpha = 0;
        for (int j=0; j<num_slice; ++j)
        {
            p_node = new Node;
            p_node->id = node_count++;
            p_node->norm.clear();
            p_node->position.x = radius * cos(alpha);
            p_node->position.y = radius * sin(alpha);

            zVec2 diff(-p_node->position.x, -p_node->position.y);
            p_node->position.z = (this->*kernel_fun)(diff) * scale_factor + kernel_plane_height;

            mesh.node_list.push_back(p_node);

            alpha += alpha_step;
        }
        radius -= radius_step;
    }

    // the last node
    p_node = new Node;
    p_node->id = node_count++;
    p_node->norm.clear();
    p_node->position.x = 0;
    p_node->position.y = 0;
    p_node->position.z = (this->*kernel_fun)(zVec2(0,0)) * scale_factor + kernel_plane_height;
    mesh.node_list.push_back(p_node);

    //------------------------------------------------------------------
    // build the mesh
    //------------------------------------------------------------------
    mesh.triangle_list.clear();
    int triangle_count = 0;
    Triangle p_tri;
    for (int i=1; i<num_circle; ++i)
    {
        Node *p0, *p1, *q0, *q1;

        int offset_p = (i-1) * num_slice;
        int offset_q = i * num_slice;
        for (int j=1; j<num_slice; ++j)
        {
            p0 = mesh.node_list[offset_p + j-1];
            p1 = mesh.node_list[offset_p + j];
            q0 = mesh.node_list[offset_q + j-1];
            q1 = mesh.node_list[offset_q + j];

            p_tri.index = triangle_count++;
            p_tri.v0 = p0;
            p_tri.v1 = p1;
            p_tri.v2 = q1;
            mesh.triangle_list.push_back(p_tri);

            p_tri.index = triangle_count++;
            p_tri.v0 = p0;
            p_tri.v1 = q1;
            p_tri.v2 = q0;
            mesh.triangle_list.push_back(p_tri);
        }

        // the last two triangle of each circle
        p0 = mesh.node_list[offset_p + num_slice-1];
        p1 = mesh.node_list[offset_p + 0];
        q0 = mesh.node_list[offset_q + num_slice-1];
        q1 = mesh.node_list[offset_q + 0];

        p_tri.index = triangle_count++;
        p_tri.v0 = p0;
        p_tri.v1 = p1;
        p_tri.v2 = q1;
        mesh.triangle_list.push_back(p_tri);

        p_tri.index = triangle_count++;
        p_tri.v0 = p0;
        p_tri.v1 = q1;
        p_tri.v2 = q0;
        mesh.triangle_list.push_back(p_tri);
    }

    // the last num_slice triangles
    int lastID = mesh.node_list.size() - 1;
    int offset = lastID - num_slice;
    for (int j=1; j<num_slice; ++j)
    {
        p_tri.index = triangle_count++;
        p_tri.v0 = mesh.node_list[offset + j-1];
        p_tri.v1 = mesh.node_list[offset + j];
        p_tri.v2 = mesh.node_list[lastID];
        mesh.triangle_list.push_back(p_tri);
    }

    p_tri.index = triangle_count++;
    p_tri.v0 = mesh.node_list[offset + num_slice-1];
    p_tri.v1 = mesh.node_list[offset + 0];
    p_tri.v2 = mesh.node_list[lastID];
    mesh.triangle_list.push_back(p_tri);

    //------------------------------------------------------------------
    // compute the norm of each node
    //------------------------------------------------------------------
//    int num_tri = mesh.triangle_list.size();
//    for (int i=0; i<num_tri; ++i)
//    {
//        Triangle *p_tri = &mesh.triangle_list[i];

//        zVec3 nl = zVec3::normalOfTriangle(p_tri->v0->position,
//                                           p_tri->v1->position,
//                                           p_tri->v2->position);
//        nl = zVec3::normalize(nl);

//        p_tri->norm = nl;

//        p_tri->v0->norm += nl;
//        p_tri->v0->norm = zVec3::normalize(p_tri->v0->norm);

//        p_tri->v1->norm += nl;
//        p_tri->v1->norm = zVec3::normalize(p_tri->v1->norm);

//        p_tri->v2->norm += nl;
//        p_tri->v2->norm = zVec3::normalize(p_tri->v2->norm);
//    }

    mesh.computeNormal();
}

// The projection of mesh to the xy plane is a half circle centered at (0,0)
void KernelMonitor::buildKernelMeshHalf(TriangleMesh &mesh, KernelFunction kernel_fun, double scale_factor)
{
    // compute the new support radius
    double radius_max = support;
    int num_circle = 20;
    int num_slice = 50;

    // the half disk will be divided into 50 pizza slices and 10 circles
    double alpha_step = PI / double(num_slice-1);
    double radius_step = radius_max / double(num_circle);

    // determine the initial angle.
//    double alpha_init = atan2(rv[1], rv[0]) - PI / 2;

    //------------------------------------------------------------------
    // build the nodes
    //------------------------------------------------------------------
    mesh.node_list.clear();
    int node_count = 0;
    double radius = radius_max;
    Node *p_node;
    for (int i=0; i<num_circle; ++i)
    {
        double alpha = - PI/2.0;
        for (int j=0; j<num_slice; ++j)
        {
            p_node = new Node;

            p_node->id = node_count++;
            p_node->norm.clear();
            p_node->position.x = radius * cos(alpha);
            p_node->position.y = radius * sin(alpha);

            zVec2 diff(-p_node->position.x, -p_node->position.y);
//            p_node->position.z = (this->*kernel_fun)(diff)*2 * scale_factor + kernel_plane_height;
            p_node->position.z = (this->*kernel_fun)(diff) * scale_factor + kernel_plane_height;

            mesh.node_list.push_back(p_node);

            alpha += alpha_step;
        }
        radius -= radius_step;
    }

    // the last node
    p_node = new Node;
    p_node->id = node_count++;
    p_node->norm.clear();
    p_node->position.x = 0;
    p_node->position.y = 0;
//    p_node->position.z = (this->*kernel_fun)(zVec2(0,0))*2 * scale_factor + kernel_plane_height;
    p_node->position.z = (this->*kernel_fun)(zVec2(0,0)) * scale_factor + kernel_plane_height;
    mesh.node_list.push_back(p_node);

    //------------------------------------------------------------------
    // build the mesh
    //------------------------------------------------------------------
    mesh.triangle_list.clear();
    int triangle_count = 0;
    Triangle p_tri;
    for (int i=1; i<num_circle; ++i)
    {
        Node *p0, *p1, *q0, *q1;

        int offset_p = (i-1) * num_slice;
        int offset_q = i * num_slice;
        for (int j=1; j<num_slice; ++j)
        {
            p0 = mesh.node_list[offset_p + j-1];
            p1 = mesh.node_list[offset_p + j];
            q0 = mesh.node_list[offset_q + j-1];
            q1 = mesh.node_list[offset_q + j];

            p_tri.index = triangle_count++;
            p_tri.v0 = p0;
            p_tri.v1 = p1;
            p_tri.v2 = q1;
            mesh.triangle_list.push_back(p_tri);

            p_tri.index = triangle_count++;
            p_tri.v0 = p0;
            p_tri.v1 = q1;
            p_tri.v2 = q0;
            mesh.triangle_list.push_back(p_tri);
        }

        // the last two triangle of each circle
        p0 = mesh.node_list[offset_p + num_slice-1];
        p1 = mesh.node_list[offset_p + 0];
        q0 = mesh.node_list[offset_q + num_slice-1];
        q1 = mesh.node_list[offset_q + 0];

        p_tri.index = triangle_count++;
        p_tri.v0 = p0;
        p_tri.v1 = p1;
        p_tri.v2 = q1;
        mesh.triangle_list.push_back(p_tri);

        p_tri.index = triangle_count++;
        p_tri.v0 = p0;
        p_tri.v1 = q1;
        p_tri.v2 = q0;
        mesh.triangle_list.push_back(p_tri);
    }

    // the last num_slice triangle
    int lastID = mesh.node_list.size() - 1;
    int offset = lastID - num_slice;
    for (int j=1; j<num_slice; ++j)
    {
        p_tri.index = triangle_count++;
        p_tri.v0 = mesh.node_list[offset + j-1];
        p_tri.v1 = mesh.node_list[offset + j];
        p_tri.v2 = mesh.node_list[lastID];
        mesh.triangle_list.push_back(p_tri);
    }

    p_tri.index = triangle_count++;
    p_tri.v0 = mesh.node_list[offset + num_slice-1];
    p_tri.v1 = mesh.node_list[offset + 0];
    p_tri.v2 = mesh.node_list[lastID];
    mesh.triangle_list.push_back(p_tri);

    //------------------------------------------------------------------
    // compute the norm of each node
    //------------------------------------------------------------------
//    int num_tri = mesh.triangle_list.size();
//    for (int i=0; i<num_tri; ++i)
//    {
//        Triangle *p_tri = &mesh.triangle_list[i];

//        zVec3 nl = zVec3::normalOfTriangle(p_tri->v0->position,
//                                           p_tri->v1->position,
//                                           p_tri->v2->position);
//        nl = zVec3::normalize(nl);

//        p_tri->norm = nl;

//        p_tri->v0->norm += nl;
//        p_tri->v0->norm = zVec3::normalize(p_tri->v0->norm);

//        p_tri->v1->norm += nl;
//        p_tri->v1->norm = zVec3::normalize(p_tri->v1->norm);

//        p_tri->v2->norm += nl;
//        p_tri->v2->norm = zVec3::normalize(p_tri->v2->norm);
//    }
    mesh.computeNormal();
}

// This function build a triangle mesh for the poly6 kernel.
// The projection of mesh to the xy plane is a circle centered at (0,0) with radius support.
void KernelMonitor::buildKernelMeshCoarse(TriangleMesh &mesh, KernelFunction kernel_fun, double scale_factor)
{
    // determine the number of stripes and related number of pieces of each strips.
    int num_circle = 20; // thus 20 stripes
    int num_slice[20]; // each stripe will be divided to different number of pieces.
    num_slice[0] = 100;
    for (int i=1; i<num_circle; ++i)
    {
        num_slice[i] = num_slice[i-1] - 3;
    }

    // compute the new support radius
    double radius_max = support;
    double radius_step = radius_max / double(num_circle);
    double radius = radius_max;

    //------------------------------------------------------------------
    // build the nodes
    //------------------------------------------------------------------
    mesh.node_list.clear();
    int node_count = 0;
    Node *p_node;
    for (int i=0; i<num_circle; ++i)
    {
        double alpha_step = 2.0 * PI / double(num_slice[i]);

        // the first column of node of the stripe
        double alpha = 0;
        for (int j=0; j<num_slice[i]; ++j)
        {
            p_node = new Node;
            p_node->id = node_count++;
            p_node->norm.clear();
            p_node->position.x = radius * cos(alpha);
            p_node->position.y = radius * sin(alpha);

            zVec2 diff(-p_node->position.x, -p_node->position.y);
            p_node->position.z = (this->*kernel_fun)(diff) * scale_factor + kernel_plane_height;

            mesh.node_list.push_back(p_node);

            alpha += alpha_step;
        }
        radius -= radius_step;

        // the second column of nodes of the stripe
        alpha = 0;
        for (int j=0; j<num_slice[i]; ++j)
        {
            p_node = new Node;
            p_node->id = node_count++;
            p_node->norm.clear();
            p_node->position.x = radius * cos(alpha);
            p_node->position.y = radius * sin(alpha);

            zVec2 diff(-p_node->position.x, -p_node->position.y);
            p_node->position.z = (this->*kernel_fun)(diff) * scale_factor + kernel_plane_height;

            mesh.node_list.push_back(p_node);

            alpha += alpha_step;
        }
    }

//    // the last node
//    p_node = new Node;
//    p_node->id = node_count++;
//    p_node->norm.clear();
//    p_node->position.x = 0;
//    p_node->position.y = 0;
//    p_node->position.z = (this->*kernel_fun)(zVec2(0,0)) * scale_factor + kernel_plane_height;
//    mesh.node_list.push_back(p_node);

    //------------------------------------------------------------------
    // build the mesh
    //------------------------------------------------------------------
    mesh.triangle_list.clear();
    int triangle_count = 0;
    int offset_p = 0;
    int offset_q;
    Triangle p_tri;
    for (int i=0; i<num_circle; ++i)
    {
        offset_q = offset_p + num_slice[i];

        Node *p0, *p1, *q0, *q1;
        for (int j=1; j<num_slice[i]; ++j)
        {
            p0 = mesh.node_list[offset_p + j-1];
            p1 = mesh.node_list[offset_p + j];
            q0 = mesh.node_list[offset_q + j-1];
            q1 = mesh.node_list[offset_q + j];

            p_tri.index = triangle_count++;
            p_tri.v0 = p0;
            p_tri.v1 = p1;
            p_tri.v2 = q1;
            mesh.triangle_list.push_back(p_tri);

            p_tri.index = triangle_count++;
            p_tri.v0 = p0;
            p_tri.v1 = q1;
            p_tri.v2 = q0;
            mesh.triangle_list.push_back(p_tri);
        }

        // the last two triangle of each circle
        p0 = mesh.node_list[offset_p + num_slice[i]-1];
        p1 = mesh.node_list[offset_p + 0];
        q0 = mesh.node_list[offset_q + num_slice[i]-1];
        q1 = mesh.node_list[offset_q + 0];

        p_tri.index = triangle_count++;
        p_tri.v0 = p0;
        p_tri.v1 = p1;
        p_tri.v2 = q1;
        mesh.triangle_list.push_back(p_tri);

        p_tri.index = triangle_count++;
        p_tri.v0 = p0;
        p_tri.v1 = q1;
        p_tri.v2 = q0;
        mesh.triangle_list.push_back(p_tri);

        offset_p += 2*num_slice[i];
    }

//    // the last num_slice triangles
//    int lastID = mesh.node_list.size() - 1;
//    int offset = lastID - num_slice[num_circle-1];
//    for (int j=1; j<num_slice[num_circle-1]; ++j)
//    {
//        p_tri.index = triangle_count++;
//        p_tri.v0 = mesh.node_list[offset + j-1];
//        p_tri.v1 = mesh.node_list[offset + j];
//        p_tri.v2 = mesh.node_list[lastID];
//        mesh.triangle_list.push_back(p_tri);
//    }

//    p_tri.index = triangle_count++;
//    p_tri.v0 = mesh.node_list[offset + num_slice[num_circle-1]-1];
//    p_tri.v1 = mesh.node_list[offset + 0];
//    p_tri.v2 = mesh.node_list[lastID];
//    mesh.triangle_list.push_back(p_tri);

    //------------------------------------------------------------------
    // compute the norm of each node
    //------------------------------------------------------------------
    mesh.computeNormal();
}

// The projection of mesh to the xy plane is a half circle centered at (0,0)
void KernelMonitor::buildKernelMeshHalfCoarse(TriangleMesh &mesh, KernelFunction kernel_fun, double scale_factor)
{
    // determine the number of stripes and related number of pieces of each strips.
    int num_circle = 20; // thus 20 stripes
    int num_slice[20]; // each stripe will be divided to different number of pieces.
    num_slice[0] = 50;
    for (int i=1; i<num_circle; ++i)
    {
        num_slice[i] = num_slice[i-1] - 1;
    }

    // compute the new support radius
    double radius_max = support;
    double radius_step = radius_max / double(num_circle);
    double radius = radius_max;

    //------------------------------------------------------------------
    // build the nodes
    //------------------------------------------------------------------
    mesh.node_list.clear();
    int node_count = 0;
    Node *p_node;
    for (int i=0; i<num_circle; ++i)
    {
        double alpha_step = PI / double(num_slice[i]-1);

        // the first column of node of the stripe
        double alpha = - PI/2.0;
        for (int j=0; j<num_slice[i]; ++j)
        {
            p_node = new Node;

            p_node->id = node_count++;
            p_node->norm.clear();
            p_node->position.x = radius * cos(alpha);
            p_node->position.y = radius * sin(alpha);

            zVec2 diff(-p_node->position.x, -p_node->position.y);
//            p_node->position.z = (this->*kernel_fun)(diff)*2 * scale_factor + kernel_plane_height;
            p_node->position.z = (this->*kernel_fun)(diff) * scale_factor + kernel_plane_height;

            mesh.node_list.push_back(p_node);

            alpha += alpha_step;
        }
        radius -= radius_step;

        // the second column of node of the stripe
        alpha = - PI/2.0;
        for (int j=0; j<num_slice[i]; ++j)
        {
            p_node = new Node;

            p_node->id = node_count++;
            p_node->norm.clear();
            p_node->position.x = radius * cos(alpha);
            p_node->position.y = radius * sin(alpha);

            zVec2 diff(-p_node->position.x, -p_node->position.y);
//            p_node->position.z = (this->*kernel_fun)(diff)*2 * scale_factor + kernel_plane_height;
            p_node->position.z = (this->*kernel_fun)(diff) * scale_factor + kernel_plane_height;

            mesh.node_list.push_back(p_node);

            alpha += alpha_step;
        }
    }

//    // the last node
//    p_node = new Node;
//    p_node->id = node_count++;
//    p_node->norm.clear();
//    p_node->position.x = 0;
//    p_node->position.y = 0;
////    p_node->position.z = (this->*kernel_fun)(zVec2(0,0))*2 * scale_factor + kernel_plane_height;
//    p_node->position.z = (this->*kernel_fun)(zVec2(0,0)) * scale_factor + kernel_plane_height;
//    mesh.node_list.push_back(p_node);

    //------------------------------------------------------------------
    // build the mesh
    //------------------------------------------------------------------
    mesh.triangle_list.clear();
    int triangle_count = 0;
    int offset_p = 0;
    int offset_q;
    Triangle p_tri;
    for (int i=0; i<num_circle; ++i)
    {
        offset_q = offset_p + num_slice[i];

        Node *p0, *p1, *q0, *q1;
        for (int j=1; j<num_slice[i]; ++j)
        {
            p0 = mesh.node_list[offset_p + j-1];
            p1 = mesh.node_list[offset_p + j];
            q0 = mesh.node_list[offset_q + j-1];
            q1 = mesh.node_list[offset_q + j];

            p_tri.index = triangle_count++;
            p_tri.v0 = p0;
            p_tri.v1 = p1;
            p_tri.v2 = q1;
            mesh.triangle_list.push_back(p_tri);

            p_tri.index = triangle_count++;
            p_tri.v0 = p0;
            p_tri.v1 = q1;
            p_tri.v2 = q0;
            mesh.triangle_list.push_back(p_tri);
        }

        // the last two triangle of each circle
        p0 = mesh.node_list[offset_p + num_slice[i]-1];
        p1 = mesh.node_list[offset_p + 0];
        q0 = mesh.node_list[offset_q + num_slice[i]-1];
        q1 = mesh.node_list[offset_q + 0];

        p_tri.index = triangle_count++;
        p_tri.v0 = p0;
        p_tri.v1 = p1;
        p_tri.v2 = q1;
        mesh.triangle_list.push_back(p_tri);

        p_tri.index = triangle_count++;
        p_tri.v0 = p0;
        p_tri.v1 = q1;
        p_tri.v2 = q0;
        mesh.triangle_list.push_back(p_tri);

        offset_p += 2 * num_slice[i];
    }

//    // the last num_slice triangle
//    int lastID = mesh.node_list.size() - 1;
//    int offset = lastID - num_slice;
//    for (int j=1; j<num_slice; ++j)
//    {
//        p_tri.index = triangle_count++;
//        p_tri.v0 = mesh.node_list[offset + j-1];
//        p_tri.v1 = mesh.node_list[offset + j];
//        p_tri.v2 = mesh.node_list[lastID];
//        mesh.triangle_list.push_back(p_tri);
//    }

//    p_tri.index = triangle_count++;
//    p_tri.v0 = mesh.node_list[offset + num_slice-1];
//    p_tri.v1 = mesh.node_list[offset + 0];
//    p_tri.v2 = mesh.node_list[lastID];
//    mesh.triangle_list.push_back(p_tri);

    //------------------------------------------------------------------
    // compute the norm of each node
    //------------------------------------------------------------------
    mesh.computeNormal();
}



