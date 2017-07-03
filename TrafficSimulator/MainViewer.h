#ifndef MAINVIEWER
#define MAINVIEWER

#include <QGLFunctions>
#include <QGLViewer/qglviewer.h>
#include <QObject>
#include <QMouseEvent>
#include <qmessagebox.h>
#include "ParticleSystemTraffic.h"
#include "roadproperty.h"
#include "particleproperty.h"
#include "carproperty.h"

#include "ft2build.h"
#include FT_FREETYPE_H
#include FT_GLYPH_H

class MainViewer : public QGLViewer
{
public :
    MainViewer(QWidget *parent);
    ~MainViewer();

    RoadProperty roadsetting;
    ParticleProperty particlesetting;
    CarProperty carsetting;

    ParticleSystemTraffic *traffic_system_p;
    SelectionGroup selection_choice;
    double fps;

    int display_mode_D; // 0:nothing, 1:density field, 2:velocity field
    int display_mode_P; // 0:nothing, 1:particle, 2:car, 3:bbox

//    int display_mode_O; // 0:nothing, 1:observation point
//    int display_mode_S; // 0:nothing/confirm, 1:spline setting
//    int display_mode_B; // 0:nothing/confirm, 1:block area setting

    bool showReferencePlanes;
    bool showRoadBoundaryCells;
    bool showRoadBoundingBox;
    bool showRoadFacilities;
    bool showParticleForce;
    bool showParticleVelocityDirection;
    bool showParticleNormalTangent;
    bool showCarSpringForce;
    bool showCarVelocityDirection;
    bool showCarTail;
    bool showCarTrajectory;

    bool showObservations;
    bool showBlocks;
    bool showBeziers;

    // For minviwers, 1:denaity, 2:velocity
    int viewer_mode;

    void initDisplayObject(ParticleSystemTraffic* p_obj, int mode_);

    // texture
    GLuint texture_colormap;
    vector<TextImage> texture_charmap;
    double char_top_max, char_top_min;
    vector<TextImage> texture_boldcharmap;
    double boldchar_top_max, boldchar_top_min;
    void initTexture();
    void initFontTexture(FT_Face &freetype_face, vector<TextImage> &tt_charmap,
                         double &ch_top_max, double &ch_top_min);

    void updateObservationPoint();
    void restoreObservationPoint();

    void updateSplines();
    void restoreSplines();
    void backupSplines();

    void addNewBlockArea();
    void updateBlockArea();
    void restoreBlockArea();

    double matrix_modelview[16], matrix_projection[16];
    void saveCamera();
    void restoreCamera();

    unsigned char *screenshot;
    void updateScreenShot();

protected :
    virtual void init();
    virtual void draw();
    virtual void drawWithNames();
    virtual void postSelection(const QPoint& point);

    virtual void mousePressEvent(QMouseEvent* e);
    virtual void mouseMoveEvent(QMouseEvent* e);
    virtual void mouseReleaseEvent(QMouseEvent* e);
    virtual void mouseDoubleClickEvent(QMouseEvent* e);
    //virtual void endSelection(const QPoint&);
    //virtual void keyPressEvent(QKeyEvent *e);
//    virtual void initializeGL();
//    virtual void paintGL();

public:
    // Particle's Lighting Materials.
    GLfloat material_ambient[4];
    GLfloat material_diffuse[4];
    GLfloat material_specular[4];
    GLfloat material_shininess[1];

    // Particle's color.
    zVec4 particle_color;
    zVec4 particle_color_ghost;
    zVec4 particle_color_selected;
    zVec4 particle_color_anchored;
    zVec4 particle_color_active;
    zVec4 particle_color_neighbor;

    zVec4 color_bezier;
    zVec4 color_bezier_selected;

    zVec4 color_road;
    zVec4 color_road_light;
    zVec4 color_lane;

    zVec4 color_block;

    // Current rectangular selection
    QRect selection_rect;
    QLine line_on_screen;

    Particle* part_selected;

    double axis_length;

    bool isModifiedSpline;

    GLfloat *circle_node_p;
    int circle_node_number;
    GLfloat *sphere_node_p, *sphere_normal_p;
    int sphere_face_number;
    void buildVertexArrays();

    void drawOneParticle(Particle* p_particle, zVec4 color_);
    void drawSphere(zVec3 pos, zVec4 color_);
    void drawParticles();
    void drawParticlesGhost();
    void drawParticlesVisualVelocity();
    void drawParticlesVisualDensity();

    void drawMesh();
    void drawFieldMeshDensity();
    void drawColormapDensity();
    void drawFieldMeshVelocity();
    void drawColormapVelocity();
    void drawReferencePlanes();

    void drawNeighbours(Particle* p_particle);

    void drawLineOnScreen();

    void drawBlockLine();

    void drawBar(GLfloat bar_verts[8][3], zVec4 color_);
    void drawSideLanes();
    void drawOneSideLane(vector<zVec2> &cl, zVec4 color_);

    void drawRealMaps();

    void drawRoads(zVec4 color_);
    void drawOneRoadPlane(double height);
    void drawDrains();
    void drawRoadSensors();
    void drawCars();
    void drawOneCar(CarParticle* p_car);
    void drawOneCarRoundPoint(CarParticle* p_car, double dd);
    void drawOneCarBbox(CarParticle *p_car);
    void drawOneCarTail(CarParticle *p_car);
    void drawOneCarHead(CarParticle *p_car);
    void drawExternalForce(zVec4 color_);

    void drawBoundaryZone();

    void drawArrowFrom(zVec3 origin, zVec3 destination, double D, zVec4 color_);

    void drawSelectionRectangle();

    void drawAxis(double len, const char* str);
    void drawColormap(int x0, int y0, int x1, int y1);
    void drawTextOnTop(const QString &str, int size);

    // freetype text
    void drawFreeTypeOnScreen(int x0, int y0, int hh, zVec4 color, const char *text);
    void drawFreeTypeBoldOnScreen(int x0, int y0, int hh, zVec4 color, const char *text);
    void drawFreeTypeOnObject(double trans_[], double rota_[], double ww, double hh,
                              zVec4 color, const char *text);

    // for spline
    vector<BezierSequence*> road_curves;
    double visual_xy_plane_height;
    void drawBezierSpline();
    void drawOneQuad(zVec2 &pos, zVec4 color_);

    void drawOneCube(zVec2 &pos, zVec4 color_);    
    void drawOneCircle(zVec2 &pos, double sc, double hh, zVec4 color_);

    // for observation points
    void drawObservationPoints();
    void drawOneObservationPoint(Node *p_node);

    // for the active block area
    vector<Cell> block_list;
    void drawBlockAreas();
    void drawOneBlockArea(Cell &cc, zVec4 color_);
    void drawOneBlockBarrier(Cell &cc);

    // check if camera is near the x axis
    bool isNearCamera();

    inline int next_p2 (int a );
};

#endif
