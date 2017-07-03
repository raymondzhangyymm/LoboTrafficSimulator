#include "MainViewer.h"

// Constructor must call the base class constructor.
MainViewer::MainViewer(QWidget *parent) : QGLViewer(parent)
{
    // Colors
//    particle_color = zVec4(0.8, 0.7, 0.0, 1.0);
    particle_color = zVec4(0.0, 0.5, 0.6, 1.0);
    particle_color_ghost = zVec4(1.0, 0.0, 0.0, 0.5);
    particle_color_selected = zVec4(0.8, 0.0, 0.0, 1.0);
    particle_color_anchored = zVec4(1.0, 0.0, 0.0, 1.0);
    particle_color_active = zVec4(1.0, 0.0, 1.0, 1.0);
    particle_color_neighbor = zVec4(0.6, 0.6, 0.0, 1.0);

    color_road[0] = 0.2;
    color_road[1] = 0.2;
    color_road[2] = 0.2;
    color_road[3] = 1.0;

    color_road_light[0] = 0.5;
    color_road_light[1] = 0.5;
    color_road_light[2] = 0.5;
    color_road_light[3] = 1.0;

    color_lane[0] = 0.9;
    color_lane[1] = 0.9;
    color_lane[2] = 0.9;
    color_lane[3] = 1.0;

    color_block[0] = 1.0;
    color_block[1] = 0.0;
    color_block[2] = 0.0;
    color_block[3] = 1.0;

    color_bezier[0] = 0.0;
    color_bezier[1] = 0.5;
    color_bezier[2] = 1.0;
    color_bezier[3] = 1.0;

    color_bezier_selected[0] = 1.0;
    color_bezier_selected[1] = 0.0;
    color_bezier_selected[2] = 0.0;
    color_bezier_selected[3] = 1.0;

    //mesh_color = zVec4(0.0, 0.9, 0.9, 1.0);
//    mesh_color = zVec4(1.5, 1.5, 1.5, 1.0);
//    rest_shape_color = zVec4(0.8, 0.3, 0.3, 0.5);
//    realtime_color = zVec4(0.3, 0.8, 0.3, 1.0);

    // Matrials
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

    material_shininess[0] = 10.0;

    // Display options
    display_mode_P = 0;
    display_mode_D = 0;

    showObservations = false;
    showBlocks = false;
    showBeziers = false;

    showReferencePlanes = true;
    showRoadBoundaryCells = false;
    showRoadBoundingBox = false;
    showRoadFacilities = false;
    showParticleForce = false;
    showParticleVelocityDirection = false;
    showParticleNormalTangent = false;
    showCarSpringForce = false;
    showCarVelocityDirection = false;
    showCarTail = false;
    showCarTrajectory = false;

    // Selection options
    selection_choice = nothing;

//    QGLFormat format;
//    format.setDoubleBuffer(true);
//    format.setDepth(false);
//    format.setAlpha(false);
//    format.setSampleBuffers(true);
//    format.setSamples(16);
//    this->setFormat(format);

    traffic_system_p = NULL;
    screenshot = NULL;
    part_selected = NULL;
    fps = 0;

    init();
}
MainViewer::~MainViewer()
{
}

//=========================================================================
// Functions for the main viewer.
//=========================================================================
//void MainViewer::initializeGL()
void MainViewer::init()
{
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    glShadeModel(GL_SMOOTH);

//    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

//    glEnable(GL_LINE_SMOOTH);
//    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
//    glEnable(GL_POLYGON_SMOOTH);
//    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

    //glEnable(GL_COLOR_MATERIAL);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//    glEnable(GL_MULTISAMPLE);
//    GLint  iMultiSample = 0;
//    GLint  iNumSamples = 0;
//    glGetIntegerv(GL_SAMPLE_BUFFERS, &iMultiSample);
//    glGetIntegerv(GL_SAMPLES, &iNumSamples);
//    cout << "Sample buffers = " << iMultiSample << ", Samples = " << iNumSamples << endl;

    // set the light
    GLfloat lightAmbient[] = {0.5, 0.5, 0.5, 1};
    GLfloat lightDiffuse[] = {0.5, 0.5, 0.5, 1};
    GLfloat lightPosition[] = {1.0, -1.0, 50.0, 0};

    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);	//Setup The Ambient Light
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);	//Setup The Diffuse Light
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);	//Position The Light
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);

    glClearColor(1.0, 1.0, 1.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);

    // disable some default shortcuts
    setShortcut(STEREO, 0);  // default 'S'
    setShortcut(EXIT_VIEWER, 0);  // default 'Escape'
    setShortcut(EDIT_CAMERA, 0);  // default 'C'
}

void MainViewer::initDisplayObject(ParticleSystemTraffic *p_obj, int mode_)
{
    if (!p_obj)
    {
        return;
    }

    // make this opengl context the current one.
    this->makeCurrent();

    initTexture();

    traffic_system_p = p_obj;
    viewer_mode = mode_;

    // set up the camera
    double scene_radius = (traffic_system_p->max_corner.x - traffic_system_p->min_corner.x)/2;
    camera()->setSceneRadius(scene_radius);
    camera()->setOrientation(0,-M_PI/3.0);
    camera()->showEntireScene();
    axis_length = 0.8 * camera()->sceneRadius();

    // save the current camera for exporting files later.
//    camera()->addKeyFrameToPath(1);
    saveCamera();

    // set a xy plane
    visual_xy_plane_height = 1.5*traffic_system_p->model_parameter->ParticleRadius;

    if (viewer_mode == 0)
    {        
        int num_road = traffic_system_p->road_list.size();
        road_curves.clear();
        road_curves.resize(num_road);
        for (int i=0; i<num_road; ++i)
        {
            road_curves[i] = &traffic_system_p->road_list[i]->road_curve;
        }

        // init the block areas
        restoreBlockArea();

        buildVertexArrays();
    }
}

void MainViewer::initTexture()
{
    //------------------------------------------------------------------------
    // Initialize the texture
    //------------------------------------------------------------------------
    glEnable(GL_TEXTURE_2D);

    QImage texture_file;
    texture_file.load(QDir::currentPath() + "/image/image_rygb.png");

    texture_colormap = -1;
    glGenTextures(1, &texture_colormap);
    glBindTexture(GL_TEXTURE_2D, texture_colormap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0,
                 GL_RGB, texture_file.width(), texture_file.height(), 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, texture_file.bits());

    // init the free type library
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        cout << "Could not init freetype library\n";
        return;
    }

    // load the fonts
    FT_Face freetype_face;
    string fontfile = QDir::currentPath().toStdString() + "/font/times.ttf";
    if (FT_New_Face(ft, fontfile.c_str(), 0, &freetype_face))
    {
        cout << "Opening font file failed! " << fontfile.c_str() << endl;
        return;
    }
    initFontTexture(freetype_face, texture_charmap, char_top_max, char_top_min);

    // load the bold fonts
    fontfile = QDir::currentPath().toStdString() + "/font/timesbd.ttf";
    if (FT_New_Face(ft, fontfile.c_str(), 0, &freetype_face))
    {
        cout << "Opening font file failed! " << fontfile.c_str() << endl;
        return;
    }
    initFontTexture(freetype_face, texture_boldcharmap, boldchar_top_max, boldchar_top_min);

    FT_Done_Face(freetype_face);
    FT_Done_FreeType(ft);

    glDisable(GL_TEXTURE_2D);
}

void MainViewer::initFontTexture(FT_Face &freetype_face, vector<TextImage> &tt_charmap,
                                 double &ch_top_max, double &ch_top_min)
{
    // the size of each text character
    FT_Set_Pixel_Sizes(freetype_face, 0, 500);

    // Disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    tt_charmap.clear();
    FT_Select_Charmap(freetype_face , ft_encoding_unicode);

    // Load all character glyphs
    ch_top_max = -1;
    ch_top_min = 1;
    for (unsigned char ch=0; ch<128; ++ch)
    {
        if (FT_Load_Char(freetype_face, ch , FT_LOAD_RENDER))
        {
            cout << "Failed to load Glyph " << endl;
            continue;
        }

        FT_Bitmap &bitmap = freetype_face->glyph->bitmap;

        // Generate texture
        GLuint tex_id = -1;
        glGenTextures(1, &tex_id);
        glBindTexture(GL_TEXTURE_2D, tex_id);

        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0,
                     GL_RGBA, bitmap.width, bitmap.rows, 0,
                     GL_ALPHA, GL_UNSIGNED_BYTE, bitmap.buffer);

        // Now store character for later use
        TextImage text_image;
        text_image.texture_id = tex_id;
        text_image.ch = ch;
        text_image.size.x = bitmap.width;
        text_image.size.y = bitmap.rows;
        text_image.bearing.x = freetype_face->glyph->bitmap_left;
        text_image.bearing.y = freetype_face->glyph->bitmap_top;
        text_image.advance = freetype_face->glyph->advance.x;

        if (ch_top_max < text_image.bearing.y)
        {
            ch_top_max = text_image.bearing.y;
        }
        double temp = text_image.bearing.y - text_image.size.y;
        if (ch_top_min > temp)
        {
            ch_top_min = temp;
        }

        tt_charmap.push_back(text_image);
    }
}

void MainViewer::buildVertexArrays()
{
    // the circle
    TriangleMesh *p_circle = &traffic_system_p->model_parameter->circle_mesh;
    circle_node_number = p_circle->node_list.size();
    circle_node_p = new GLfloat[circle_node_number*2];
    for (int i=0; i<circle_node_number; ++i)
    {
        Node *p_node = p_circle->node_list[i];
        circle_node_p[2*i+0] = p_node->position.x;
        circle_node_p[2*i+1] = p_node->position.y;
    }

    // the sphere
    TriangleMesh *p_sphere = &traffic_system_p->model_parameter->sphere_mesh;
    sphere_face_number = p_sphere->triangle_list.size();

    sphere_node_p = new GLfloat[sphere_face_number*9];
    sphere_normal_p = new GLfloat[sphere_face_number*9];

    int count = 0;
    for (int i=0; i<sphere_face_number; ++i)
    {
        Triangle *p_tri = &p_sphere->triangle_list[i];

        sphere_node_p[count+0] = p_tri->v0->position[0];
        sphere_node_p[count+1] = p_tri->v0->position[1];
        sphere_node_p[count+2] = p_tri->v0->position[2];
        sphere_node_p[count+3] = p_tri->v1->position[0];
        sphere_node_p[count+4] = p_tri->v1->position[1];
        sphere_node_p[count+5] = p_tri->v1->position[2];
        sphere_node_p[count+6] = p_tri->v2->position[0];
        sphere_node_p[count+7] = p_tri->v2->position[1];
        sphere_node_p[count+8] = p_tri->v2->position[2];

        sphere_normal_p[count+0] = p_tri->v0->norm[0];
        sphere_normal_p[count+1] = p_tri->v0->norm[1];
        sphere_normal_p[count+2] = p_tri->v0->norm[2];
        sphere_normal_p[count+3] = p_tri->v1->norm[0];
        sphere_normal_p[count+4] = p_tri->v1->norm[1];
        sphere_normal_p[count+5] = p_tri->v1->norm[2];
        sphere_normal_p[count+6] = p_tri->v2->norm[0];
        sphere_normal_p[count+7] = p_tri->v2->norm[1];
        sphere_normal_p[count+8] = p_tri->v2->norm[2];

        count += 9;
    }
}

//=========================================================================
// Functions for interactive operations.
//=========================================================================
void MainViewer::mousePressEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton && e->modifiers() == Qt::ShiftModifier)
    {
        // record the line 'drawn' by the mouse
        line_on_screen = QLine(e->pos(), e->pos());

        select(e->pos());
        if (selectedName() != -1)
        {
            if (selection_choice == particle && display_mode_P == 1)
            {
                // clear the neighbor flag of all particles
                for (int i=0; i<traffic_system_p->num_particles; ++i)
                {
                    Particle *p_part = traffic_system_p->particle_list[i];
                    p_part->isNeighbor = false;
                    p_part->isSelected = false;
                }

                // set the particle selected.
                part_selected = traffic_system_p->particle_list[selectedName()];
                part_selected->isSelected = true;

                if (traffic_system_p->model_parameter->UseHalfKernel)
                {
                    int num_neigh = part_selected->neighborsHalfRegion.size();
                    for (int i=0; i<num_neigh; ++i)
                    {
                        part_selected->neighborsHalfRegion[i]->isNeighbor = true;
                    }

                    cout << "partID, x_pos, x_vel, x_acc = "
                         << part_selected->id << " "
                         << part_selected->x_position << " "
                         << part_selected->x_velocity << " "
                         << part_selected->x_acceleration << " " << endl;
                    cout << "partID, x_trafficforce, x_pressforce, x_damping, x_external = "
                         << part_selected->id << " "
                         << part_selected->x_trafficForce << " "
                         << part_selected->x_pressureForce << " "
                         << part_selected->x_dampingForce << " "
                         << part_selected->x_externalForce << " " << endl;
                }
                else
                {
                    int num_neigh = part_selected->neighbors.size();
                    for (int i=0; i<num_neigh; ++i)
                    {
                        part_selected->neighbors[i]->isNeighbor = true;
                    }

                    cout << "partID, pos, vel, acc, num_fullneigh = "
                         << part_selected->id << " "
                         << part_selected->position << " "
                         << part_selected->velocity << " "
                         << part_selected->acceleration << " "
                         << num_neigh << endl;
                    cout << "partID, pressforce, dumping, ext = "
                         << part_selected->id << " "
                         << part_selected->pressureForce << " "
                         << part_selected->dampingForce << " "
                         << part_selected->externalForce << " " << endl;
                }

                //        // Actual selection on the rectangular area.
                //        // Possibly swap left/right and top/bottom to make selection_rect valid.
                //        selection_rect = selection_rect.normalized();
                //        // Define selection window dimensions
                //        setSelectRegionWidth(selection_rect.width());
                //        setSelectRegionHeight(selection_rect.height());
                //        // Compute rectangle center and perform selection
                //        select(selection_rect.center());
                //        // Update display to show new selected objects
                updateGL();

                particlesetting.display(part_selected,
                                        traffic_system_p->model_parameter->SimulationMode);
            }

            else if (selection_choice == road)
            {
                roadsetting.display(traffic_system_p->road_list[selectedName()]);
                //            QMenu *p_menu = new QMenu("Testing popup");
                //            QAction *p_action = new QAction(tr("&New"), p_menu);
                //            p_menu->addAction(p_action);
                //            p_menu->setWindowTitle("Testing popup");
                //            p_menu->exec(QCursor::pos());
            }
        }
    }
    else
    {
        QGLViewer::mousePressEvent(e);
    }
}

void MainViewer::mouseMoveEvent(QMouseEvent* e)
{
    if (e->modifiers() == Qt::ShiftModifier)
    {
//        select(e->pos());
        if (selectedName() != -1)
        {
            if (selection_choice == block && showBlocks)
            {
                qglviewer::Vec orig, dir;
                camera()->convertClickToLine(e->pos(), orig, dir);

                // get the intersection pos(x,y,0) of the line and plane z=0, i.e. pos = dir * t + orig
                // x = dir.x * t + orig.x
                // y = dir.y * t + orig.y
                // 0 = dir.z * t + orig.z
                double t = (visual_xy_plane_height - orig.z) / dir.z;
                zVec2 pos(dir.x * t + orig.x, dir.y * t + orig.y);

                Cell *p_cell = &block_list[selectedName()];
                p_cell->translate(pos);
                p_cell->align(traffic_system_p->getTangentFromRoadNet(p_cell->center_point));

                updateGL();
            }

            else if (selection_choice == spline && showBeziers)
            {
                qglviewer::Vec orig, dir;
                camera()->convertClickToLine(e->pos(), orig, dir);

                // get the intersection pos(x,y,0) of the line and plane z=0, i.e. pos = dir * t + orig
                // x = dir.x * t + orig.x
                // y = dir.y * t + orig.y
                // 0 = dir.z * t + orig.z
                double t = (visual_xy_plane_height - orig.z) / dir.z;
                zVec2 pos(dir.x * t + orig.x, dir.y * t + orig.y);

                // determine which control point it is
                int id_curve = 0;
                int id = selectedName();

                int num_curve = road_curves.size();
                if (num_curve > 0)
                {
                    for (int i=0; i<num_curve; ++i)
                    {
                        int num_cp = road_curves[i]->beziers.size()*3 + 1;
                        if (id < num_cp)
                        {
                            id_curve = i;
                            break;
                        }
                        else
                        {
                            id -= num_cp;
                        }
                    }

                    // update the control point.
                    road_curves[id_curve]->manipulateControlPoint(id, pos);
                    road_curves[id_curve]->hasModifiedBezier = true;
                    isModifiedSpline = true;

                    updateGL();
                }
            }

            else if (selection_choice == observation && showObservations)
            {
                qglviewer::Vec orig, dir;
                camera()->convertClickToLine(e->pos(), orig, dir);

                // get the intersection pos(x,y,0) of the line and plane z=0, i.e. pos = dir * t + orig
                // x = dir.x * t + orig.x
                // y = dir.y * t + orig.y
                // 0 = dir.z * t + orig.z
                double t = (visual_xy_plane_height - orig.z) / dir.z;
                zVec2 pos(dir.x * t + orig.x, dir.y * t + orig.y);

                traffic_system_p->moveObservationPoint(traffic_system_p->observation_list[selectedName()], pos);
                updateGL();
            }
        }
    }

    else
    {
        QGLViewer::mouseMoveEvent(e);
    }
}

void MainViewer::mouseReleaseEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton && e->modifiers() == Qt::ShiftModifier)
    {

    }
    else
    {
        QGLViewer::mouseReleaseEvent(e);
    }
}

void MainViewer::mouseDoubleClickEvent(QMouseEvent* e)
{
    if (showBeziers && e->button() == Qt::LeftButton)
    {
        qglviewer::Vec orig, dir;
        camera()->convertClickToLine(e->pos(), orig, dir);

        // get the intersection pos(x,y,0) of the line and plane z=0, i.e. pos = dir * t + orig
        // x = dir.x * t + orig.x
        // y = dir.y * t + orig.y
        // 0 = dir.z * t + orig.z
        double t = (visual_xy_plane_height - orig.z) / dir.z;
        zVec2 pos(dir.x * t + orig.x, dir.y * t + orig.y);

        // determine which spline/road should be splitted
        int num_curve = road_curves.size();
        if (num_curve > 0)
        {
            int id_curve = 0;
            int min_dist2 = 1e20;

            for (int i=0; i<num_curve; ++i)
            {
                zVec2 v = pos - road_curves[i]->getClosestPoint(pos);
                double d2 = v.x*v.x + v.y*v.y;
                if (d2 < min_dist2)
                {
                    min_dist2 = d2;
                    id_curve = i;
                }
            }

            cout << "Doubleclick at curve " << id_curve << "(/" << num_curve << "), ";

            int nn1 = road_curves[id_curve]->beziers.size();
            if (road_curves[id_curve]->splitBezier(pos))
            {
                int nn2 = road_curves[id_curve]->beziers.size();
                road_curves[id_curve]->hasModifiedBezier = true;
                cout << "splitting bezier succeeded. " << nn1 << " " << nn2 << endl;
                updateGL();
            }
            else
            {
                cout << "splitting bezier failed." << endl;
            }
        }
    }
//    else if ( && e->button() == Qt::LeftButton)
//    {
//        qglviewer::Vec orig, dir;
//        camera()->convertClickToLine(e->pos(), orig, dir);

//        // get the intersection pos(x,y,0) of the line and plane z=0, i.e. pos = dir * t + orig
//        // x = dir.x * t + orig.x
//        // y = dir.y * t + orig.y
//        // 0 = dir.z * t + orig.z
//        double t = (visual_xy_plane_height - orig.z) / dir.z;
//        block_point_active.x = dir.x * t + orig.x;
//        block_point_active.y = dir.y * t + orig.y;

//        block_points.push_back(block_point_active);

////        if (block_points.size() == 0)
////        {
////            // the first point must be on the side lane
////            double min_distance = 100000;
////            int idx_i = -1;  // the index to the curve
////            zVec2 closest_p;

////            Road *p_road = &traffic_system_p->road_shape->road_list[0];
////            int size = p_road->side_lane_1.size();
////            for (int i=0; i<size; ++i)
////            {
////                zVec2 v = p_road->side_lane_1[i] - block_point_active;
////                double dd = v.x*v.x + v.y*v.y;
////                if (dd < min_distance)
////                {
////                    min_distance = dd;
////                    idx_i = i;
////                    closest_p = p_road->side_lane_1[i];
////                }
////            }

////            int offset = p_road->side_lane_1.size();
////            size = p_road->side_lane_2.size();
////            for (int i=0; i<size; ++i)
////            {
////                zVec2 v = p_road->side_lane_2[i] - block_point_active;
////                double dd = v.x*v.x + v.y*v.y;
////                if (dd < min_distance)
////                {
////                    min_distance = dd;
////                    idx_i = offset + i;
////                    closest_p = p_road->side_lane_2[i];
////                }
////            }

////            if (min_distance <= 0.01)
////            {
////                block_points.push_back(closest_p);
////                block_points_zoneid.push_back(idx_i);
////                block_point_active = closest_p;
////            }
////        }
////        else
////        {
////            // determine the road zone
////            Road *p_road = &traffic_system_p->road_shape->road_list[0];
////            int idx_i = -1;
////            int size = p_road->road_zone.size();
////            for (int i=0; i<size; ++i)
////            {
////                if (p_road->isInRoad(block_point_active))
////                {
////                    idx_i = i;
////                    break;
////                }
////            }

////            if (idx_i == -1)
////            {
////                block_points.push_back(block_point_active);
////                block_points_zoneid.push_back(idx_i);
////                block_point_active = block_point_active;
////            }
////        }

//        updateGL();
//    }
    else
    {
        QGLViewer::mouseDoubleClickEvent(e);
    }
}

void MainViewer::postSelection(const QPoint& point)
{
    // Compute orig and dir, used to draw a representation of the intersecting line
    qglviewer::Vec orig, dir, selectedPoint;
    camera()->convertClickToLine(point, orig, dir);

    // Find the selectedPoint coordinates, using camera()->pointUnderPixel().
    bool found;
    selectedPoint = camera()->pointUnderPixel(point, found);
    selectedPoint -= 0.01f*dir; // Small offset to make point clearly visible.
    // Note that "found" is different from (selectedObjectId()>=0) because of the size of the select region.

//    if (selectedName() == -1)
//        QMessageBox::information(this, "No selection",
//                                 "No object selected under pixel " + QString::number(point.x()) + "," + QString::number(point.y()));
//    else
//        QMessageBox::information(this, "Selection",
//                                 "Particle number " + QString::number(selectedName()) + " selected under pixel " +
//                                 QString::number(point.x()) + "," + QString::number(point.y()));
}

//============================================================================
// Functions for Drawing.
//============================================================================
void MainViewer::draw()
{
    if (!traffic_system_p)
    {
        return;
    }

    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);

    if (traffic_system_p->model_parameter->SimulationMode == 0)
    {
        if (display_mode_P == 2 || display_mode_P == 3)
        {
            int size = traffic_system_p->car_list.size();
            for (int i=0; i<size; ++i)
            {
                //        drawOneCarBbox(traffic_system_p->car_list[i]);
                drawOneCarRoundPoint(traffic_system_p->car_list[i], 50);
            }
        }

        drawRealMaps();

        char str[1024];
        sprintf(str, "%s   %05.1f(FPS)    #Vehicles: %06i",
                traffic_system_p->simulation_timestamp, float(fps), traffic_system_p->num_cars);
        drawFreeTypeOnScreen(5, height()-32, 26, zVec4(1,0,0,1), str);
    }
    else
    {
        drawRealMaps();

        if (display_mode_P == 2)
        {
            drawRoads(color_road);
        }
        else
        {
            drawRoads(color_road_light);
        }

        if (display_mode_P == 1)
        {
            drawParticles();
        }
        if (display_mode_P == 2 || display_mode_P == 3)
        {
            drawCars();
        }

        if (display_mode_D == 1)
        {
            drawFieldMeshDensity();
            drawColormapDensity();
        }
        else if (display_mode_D == 2)
        {
            drawFieldMeshVelocity();
            drawColormapVelocity();
        }

        if (showRoadBoundaryCells)
        {
            drawBoundaryZone();
        }

        if (showObservations)
        {
            drawObservationPoints();
        }

        if (showBeziers)
        {
            drawBezierSpline();
        }

        if (showBlocks)
        {
            drawBlockAreas();
        }

        if (showRoadFacilities)
        {
            drawRoadSensors();
        }

        char str[1024];
        sprintf(str, "%07.2f(Sec.)   %05.1f(FPS)    #Particles: %07i   #Vehicles: %06i",
                traffic_system_p->simulation_time, float(fps),
                traffic_system_p->num_particles,
                traffic_system_p->num_cars);
        drawFreeTypeOnScreen(5, height()-32, 26, zVec4(1,0,0,1), str);
    }
}

void MainViewer::drawWithNames()
{
    if (!traffic_system_p)
    {
        return;
    }

    switch (selection_choice)
    {
    case nothing:
    {
        return;
    }

    case particle:
    {
        if (display_mode_P == 1)
        {
            int size = traffic_system_p->particle_list.size();
            for (int i=0; i<size; ++i)
            {
                Particle* p_particle = traffic_system_p->particle_list[i];

                glPushName(i);
                drawSphere(zVec3(p_particle->position.x, p_particle->position.y, 0), particle_color);
                glPopName();
            }
        }
        break;
    }

    case observation:
    {
        if (showObservations)
        {
            vector<Node*> *p_observation_list = &traffic_system_p->observation_list;
            int num_observation = p_observation_list->size();
            for (int i=0; i<num_observation; ++i)
            {
                glPushName(i);
                drawOneObservationPoint(p_observation_list->at(i));
                glPopName();
            }
        }
        break;
    }

    case spline:
    {
        if (showBeziers)
        {
            int count = 0;
            int num_curve = road_curves.size();
            for (int k=0; k<num_curve; ++k)
            {
                BezierSequence *p_bezs = road_curves[k];

                // The first control point
                glPushName(count++);
                drawOneQuad(p_bezs->beziers[0].control_point[0], color_bezier_selected);
                glPopName();

                // The rest control points
                int size = p_bezs->beziers.size();
                for (int i=0; i<size; ++i)
                {
                    Bezier *p_bez = &p_bezs->beziers[i];

                    for (int k=1; k<4; ++k)
                    {
                        glPushName(count++);
                        drawOneQuad(p_bez->control_point[k], color_bezier_selected);
                        glPopName();
                    }
                }
            }
        }
        break;
    }

    case block:
    {
        if (showBlocks)
        {
            int size = block_list.size();
            for (int i=0; i<size; ++i)
            {
                glPushName(i);
                drawOneBlockArea(block_list[i], color_bezier);
                glPopName();
            }
        }
        break;
    }

    case road:
    {
        glDisable(GL_CULL_FACE);
        double h_road = -0.02;
        int num_road = traffic_system_p->road_list.size();
        for (int k=0; k<num_road; ++k)
        {
            Road *p_road = traffic_system_p->road_list[k];
            if (p_road->isVisible)
            {
                glPushName(k);
                glBegin(GL_TRIANGLE_STRIP);
                int num_seg = p_road->visualroad_side_left.size();
                for (int j=0; j<num_seg; ++j)
                {
                    GLfloat n0[3], n1[3];

                    n0[0] = p_road->visualroad_side_left[j].x;
                    n0[1] = p_road->visualroad_side_left[j].y;
                    n0[2] = h_road;

                    n1[0] = p_road->visualroad_side_right[j].x;
                    n1[1] = p_road->visualroad_side_right[j].y;
                    n1[2] = h_road;

                    glVertex3fv(n0);
                    glVertex3fv(n1);
                }
                glEnd();
                glPopName();
            }
        }
        glEnable(GL_CULL_FACE);
        break;
    }
    }
}

//-----------------------------------------------------------------------------------
// Common components
void MainViewer::drawAxis(double len, const char* text)
{
    // Setting Lighting materials
    glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);

    // the arrows
    zVec3 p0(0, 0, 0);
    zVec3 p1(len, 0, 0);
    zVec4 color(1,0,1,1);
    double radius = DEFAULT_LANE_WIDTH / 3.0;
    drawArrowFrom(p0,p1,radius,color);

    p1.x = 0;
    p1.y = len;
    p1.z = 0;
    drawArrowFrom(p0,p1,radius,color);

    p1.x = 0;
    p1.y = 0;
    p1.z = len;
    drawArrowFrom(p0,p1,radius,color);

    // label "X".
    char str[1024];
    sprintf(str, "X");
    double hh = radius * 10;
    double ww = hh * 20;
    double translation[3] = {len+hh/4, 0, 0};
    double rotation[3] = {90, 0, 90}; // rotation around x/y/z axis
    drawFreeTypeOnObject(translation, rotation, ww, hh, color, str);

    rotation[2] = -90;
    drawFreeTypeOnObject(translation, rotation, ww, hh, color, str);

    // label "Y".
    sprintf(str, "Y");
    translation[0] = 0;
    translation[1] = len+hh/4;
    translation[2] = 0;
    rotation[0] = 90;
    rotation[1] = 0;
    rotation[2] = 180;
    drawFreeTypeOnObject(translation, rotation, ww, hh, color, str);

    rotation[2] = 0;
    drawFreeTypeOnObject(translation, rotation, ww, hh, color, str);

    // label the text.
    translation[0] = 0;
    translation[1] = 0;
    translation[2] = len+hh/4;
    rotation[0] = 0;
    rotation[1] = 0;
    rotation[2] = 0;
    drawFreeTypeOnObject(translation, rotation, ww, hh, color, text);

    rotation[1] = 180;
    drawFreeTypeOnObject(translation, rotation, ww, hh, color, text);
}

void MainViewer::drawTextOnTop(const QString &str, int size)
{
    // the labels of the axes.
    glDisable(GL_LIGHTING);

    // draw the rectangle with the texture
    startScreenCoordinatesSystem(true);

    QPainterPath path;
    path.addText(QPointF(0, 0), QFont("Arial", size), str);
    QList<QPolygonF> poly = path.toSubpathPolygons();
//    QList<QPolygonF> poly = path.toFillPolygons();

    int xx = 10, yy = height() - 30;

    glColor4f(0,0,0,1);
    glLineWidth(1.3);
    for (QList<QPolygonF>::iterator i = poly.begin(); i != poly.end(); i++){
        glBegin(GL_LINE_LOOP);
        //glBegin(GL_POLYGON);
        for (QPolygonF::iterator p = (*i).begin(); p != i->end(); p++)
            glVertex2f(p->rx()+xx, -p->ry()+yy);
        glEnd();
    }

    stopScreenCoordinatesSystem();

    glEnable(GL_LIGHTING);
}

void MainViewer::drawFreeTypeOnScreen(int x0, int y0, int hh, zVec4 color, const char *text)
{
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);

    double scale = hh / (char_top_max - char_top_min);
    double pen_x = x0;
    double pen_y = y0 - char_top_min * scale;

    startScreenCoordinatesSystem(true);
    glColor4f(color.x, color.y, color.z, color.a);
    for (const char *p = text; *p; p++)
    {
        TextImage *p_textchar = &texture_charmap[*p];
        glBindTexture(GL_TEXTURE_2D, p_textchar->texture_id);

        float w = p_textchar->size.x * scale;
        float h = p_textchar->size.y * scale;
        float x1 = pen_x + p_textchar->bearing.x * scale;
        float y1 = pen_y + (p_textchar->bearing.y * scale - h) ;

        glBegin(GL_QUADS);
        glTexCoord2f(0.0, 1.0);        glVertex2f(x1, y1);
        glTexCoord2f(1.0, 1.0);        glVertex2f(x1+w, y1);
        glTexCoord2f(1.0, 0.0);        glVertex2f(x1+w, y1+h);
        glTexCoord2f(0.0, 0.0);        glVertex2f(x1, y1+h);
        glEnd();

        pen_x += (p_textchar->advance >> 6) * scale;
    }

    stopScreenCoordinatesSystem();

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
}

void MainViewer::drawFreeTypeBoldOnScreen(int x0, int y0, int hh, zVec4 color, const char *text)
{
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);

    double scale = hh / (boldchar_top_max - boldchar_top_min);
    double pen_x = x0;
    double pen_y = y0 - boldchar_top_min * scale;

    startScreenCoordinatesSystem(true);
    glColor4f(color.x, color.y, color.z, color.a);
    for (const char *p = text; *p; p++)
    {
        TextImage *p_textchar = &texture_boldcharmap[*p];
        glBindTexture(GL_TEXTURE_2D, p_textchar->texture_id);

        float w = p_textchar->size.x * scale;
        float h = p_textchar->size.y * scale;
        float x1 = pen_x + p_textchar->bearing.x * scale;
        float y1 = pen_y + (p_textchar->bearing.y * scale - h) ;

        glBegin(GL_QUADS);
        glTexCoord2f(0.0, 1.0);        glVertex2f(x1, y1);
        glTexCoord2f(1.0, 1.0);        glVertex2f(x1+w, y1);
        glTexCoord2f(1.0, 0.0);        glVertex2f(x1+w, y1+h);
        glTexCoord2f(0.0, 0.0);        glVertex2f(x1, y1+h);
        glEnd();

        pen_x += (p_textchar->advance >> 6) * scale;
    }

    stopScreenCoordinatesSystem();

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
}

void MainViewer::drawFreeTypeOnObject(double trans_[], double rota_[], double ww, double hh,
                                      zVec4 color, const char *text)
{
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);

    // find the width of the text string
    double str_ww = 0;
    for (const char *p = text; *p; p++)
    {
        TextImage *p_textchar = &texture_charmap[*p];
        str_ww += (p_textchar->advance >> 6);
    }
    double str_hh = char_top_max - char_top_min;

    // determine the scale and final width and height
    double scale;
    if (str_ww * hh < ww * str_hh)
    {
        scale = hh / str_hh;
    }
    else
    {
        scale = ww / str_ww;
    }
    ww = str_ww * scale;
    hh = str_hh * scale;
    double pen_x = - ww / 2.0;
    double pen_y = - hh / 2.0 - char_top_min * scale;

    glPushMatrix();
    glTranslated(trans_[0], trans_[1], trans_[2]);
    glRotated(rota_[2], 0, 0, 1);
    glRotated(rota_[1], 0, 1, 0);
    glRotated(rota_[0], 1, 0, 0);

    glColor4f(color.x, color.y, color.z, color.a);
    for (const char *p = text; *p; p++)
    {
        TextImage *p_textchar = &texture_charmap[*p];
        glBindTexture(GL_TEXTURE_2D, p_textchar->texture_id);

        float w = p_textchar->size.x * scale;
        float h = p_textchar->size.y * scale;
        float x1 = pen_x + p_textchar->bearing.x * scale;
        float y1 = pen_y + (p_textchar->bearing.y * scale - h) ;

        glBegin(GL_QUADS);
        glTexCoord2f(0.0, 1.0);        glVertex3f(x1, y1, 0);
        glTexCoord2f(1.0, 1.0);        glVertex3f(x1+w, y1, 0);
        glTexCoord2f(1.0, 0.0);        glVertex3f(x1+w, y1+h, 0);
        glTexCoord2f(0.0, 0.0);        glVertex3f(x1, y1+h, 0);
        glEnd();

        pen_x += (p_textchar->advance >> 6) * scale;
    }

    glPopMatrix();

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
}

void MainViewer::drawColormap(int x0, int y0, int x1, int y1)
{
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    glBindTexture(GL_TEXTURE_2D, texture_colormap);

    startScreenCoordinatesSystem(true);

    glColor4f(0,0,0,1);
    glNormal3f(0.0, 0.0, 1.0);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0);
    glVertex2i(x0, y0);
    glTexCoord2f(1.0, 0.0);
    glVertex2i(x1, y0);
    glTexCoord2f(1.0, 1.0);
    glVertex2i(x1, y1);
    glTexCoord2f(0.0, 1.0);
    glVertex2i(x0, y1);
    glEnd();

    stopScreenCoordinatesSystem();

    // Depth clear is not absolutely needed. An other option would have been to draw the
    // QUAD with a 0.999 z value (z ranges in [0, 1[ with startScreenCoordinatesSystem()).
    //glClear(GL_DEPTH_BUFFER_BIT);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
}

void MainViewer::drawBar(GLfloat bar_verts[8][3], zVec4 color)
{
    if (!traffic_system_p)
    {
        return;
    }

    glColor4f(color[0], color[1], color[2], color[3]);

    // Setting Lighting materials
//    glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
//    glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
//    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
//    glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);

    glBegin(GL_QUADS);
    glVertex3fv(bar_verts[0]);
    glVertex3fv(bar_verts[3]);
    glVertex3fv(bar_verts[2]);
    glVertex3fv(bar_verts[1]);
    glEnd();

    glBegin(GL_QUADS);
    glVertex3fv(bar_verts[0]);
    glVertex3fv(bar_verts[1]);
    glVertex3fv(bar_verts[5]);
    glVertex3fv(bar_verts[4]);
    glEnd();

    glBegin(GL_QUADS);
    glVertex3fv(bar_verts[0]);
    glVertex3fv(bar_verts[4]);
    glVertex3fv(bar_verts[7]);
    glVertex3fv(bar_verts[3]);
    glEnd();

    glBegin(GL_QUADS);
    glVertex3fv(bar_verts[7]);
    glVertex3fv(bar_verts[6]);
    glVertex3fv(bar_verts[2]);
    glVertex3fv(bar_verts[3]);
    glEnd();

    glBegin(GL_QUADS);
    glVertex3fv(bar_verts[6]);
    glVertex3fv(bar_verts[5]);
    glVertex3fv(bar_verts[1]);
    glVertex3fv(bar_verts[2]);
    glEnd();

    glBegin(GL_QUADS);
    glVertex3fv(bar_verts[4]);
    glVertex3fv(bar_verts[5]);
    glVertex3fv(bar_verts[6]);
    glVertex3fv(bar_verts[7]);
    glEnd();
}

void MainViewer::drawArrowFrom(zVec3 origin, zVec3 destination, double D, zVec4 color_)
{
    // Setting Lighting materials
    glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);

    GLdouble x1 = origin.x;
    GLdouble y1 = origin.y;
    GLdouble z1 = origin.z;
    GLdouble x2 = destination.x;
    GLdouble y2 = destination.y;
    GLdouble z2 = destination.z;
    //GLdouble D  = 0.01;  // the size of the arrow

    double x=x2-x1;
    double y=y2-y1;
    double z=z2-z1;
    double L=sqrt(x*x+y*y+z*z);

    glColor4f(color_.x, color_.y, color_.z, color_.a);

    GLUquadricObj *quadObj;

    glPushMatrix();

    glTranslated((x1+x2)*0.5, (y1+y2)*0.5, (z1+z2)*0.5);

    double RADPERDEG = 0.0174533;
    if (x != 0.0 || y != 0.0)
    {
        glRotated(atan2(y,x)/RADPERDEG, 0., 0., 1.);
        glRotated(atan2(sqrt(x*x+y*y),z)/RADPERDEG,0.,1.,0.);
    }
    else if (z<0)
    {
        glRotated(180,1.,0.,0.);
    }

    // the stick
    glTranslatef(0,0,-L/2);

    quadObj = gluNewQuadric ();
    gluQuadricDrawStyle(quadObj, GLU_FILL);
    gluQuadricNormals(quadObj, GLU_SMOOTH);
    gluCylinder(quadObj, D, D, L-4*D, 32, 1);
    gluDeleteQuadric(quadObj);

    quadObj = gluNewQuadric ();
    gluQuadricDrawStyle(quadObj, GLU_FILL);
    gluQuadricNormals(quadObj, GLU_SMOOTH);
    gluQuadricOrientation(quadObj, GLU_INSIDE);
    gluDisk(quadObj, 0.0, D, 32, 1);
    gluDeleteQuadric(quadObj);

    // the head
    glTranslatef(0,0,L-4*D);

    quadObj = gluNewQuadric ();
    gluQuadricDrawStyle(quadObj, GLU_FILL);
    gluQuadricNormals(quadObj, GLU_SMOOTH);
    gluCylinder(quadObj, 2.3*D, 0.0, 4*D, 32, 1);
    gluDeleteQuadric(quadObj);

    quadObj = gluNewQuadric ();
    gluQuadricDrawStyle(quadObj, GLU_FILL);
    gluQuadricNormals(quadObj, GLU_SMOOTH);
    gluQuadricOrientation(quadObj, GLU_INSIDE);
    gluDisk(quadObj, 0.0, 2.3*D, 32, 1);
    gluDeleteQuadric(quadObj);

    glPopMatrix();
}

void MainViewer::drawSelectionRectangle()
{
    startScreenCoordinatesSystem();
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);

    glColor4f(0.0, 0.0, 0.3f, 0.3f);
    glBegin(GL_QUADS);
    glVertex2i(selection_rect.left(),  selection_rect.top());
    glVertex2i(selection_rect.right(), selection_rect.top());
    glVertex2i(selection_rect.right(), selection_rect.bottom());
    glVertex2i(selection_rect.left(),  selection_rect.bottom());
    glEnd();

    glLineWidth(2.0);
    glColor4f(0.4f, 0.4f, 0.5f, 0.5f);
    glBegin(GL_LINE_LOOP);
    glVertex2i(selection_rect.left(),  selection_rect.top());
    glVertex2i(selection_rect.right(), selection_rect.top());
    glVertex2i(selection_rect.right(), selection_rect.bottom());
    glVertex2i(selection_rect.left(),  selection_rect.bottom());
    glEnd();

    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
    stopScreenCoordinatesSystem();
}

void MainViewer::drawLineOnScreen()
{
    startScreenCoordinatesSystem();
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);

    glLineWidth(2.0);
    glColor4f(particle_color_selected[0],
              particle_color_selected[1],
              particle_color_selected[2],
              particle_color_selected[3]);
    glBegin(GL_LINES);
    glVertex2i(line_on_screen.x1(),  line_on_screen.y1());
    glVertex2i(line_on_screen.x2(),  line_on_screen.y2());
    glEnd();

    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
    stopScreenCoordinatesSystem();
}

void MainViewer::drawOneQuad(zVec2 &pos, zVec4 color)
{
    glColor4f(color.x, color.y, color.z, color.a);

    GLfloat hd = DEFAULT_LANE_WIDTH * 0.5;
    GLfloat n0[3], n1[3], n2[3], n3[3];

    n0[0] = pos.x - hd;
    n0[1] = pos.y - hd;
    n0[2] = visual_xy_plane_height;

    n1[0] = pos.x + hd;
    n1[1] = pos.y - hd;
    n1[2] = visual_xy_plane_height;

    n2[0] = pos.x + hd;
    n2[1] = pos.y + hd;
    n2[2] = visual_xy_plane_height;

    n3[0] = pos.x - hd;
    n3[1] = pos.y + hd;
    n3[2] = visual_xy_plane_height;

    glBegin(GL_QUADS);
    {
        glNormal3f(0, 0, -1);
        glVertex3fv(n0);
        glVertex3fv(n3);
        glVertex3fv(n2);
        glVertex3fv(n1);

        glNormal3f(0, 0, 1);
        glVertex3fv(n0);
        glVertex3fv(n1);
        glVertex3fv(n2);
        glVertex3fv(n3);
    }
    glEnd();
}

void MainViewer::drawOneCube(zVec2 &pos, zVec4 color)
{
    glColor4f(color.x, color.y, color.z, color.a);

    // Setting Lighting materials
//    glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
//    glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
//    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
//    glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);

    GLfloat hd = 0.05; // the half dimension of the cube.+
    GLfloat n0[3], n1[3], n2[3], n3[3];
    GLfloat n4[3], n5[3], n6[3], n7[3];

    n0[0] = pos.x - hd;
    n0[1] = pos.y - hd;
    n0[2] = - hd;

    n1[0] = pos.x + hd;
    n1[1] = pos.y - hd;
    n1[2] = - hd;

    n2[0] = pos.x + hd;
    n2[1] = pos.y + hd;
    n2[2] = - hd;

    n3[0] = pos.x - hd;
    n3[1] = pos.y + hd;
    n3[2] = - hd;

    n4[0] = pos.x - hd;
    n4[1] = pos.y - hd;
    n4[2] = hd;

    n5[0] = pos.x + hd;
    n5[1] = pos.y - hd;
    n5[2] = hd;

    n6[0] = pos.x + hd;
    n6[1] = pos.y + hd;
    n6[2] = hd;

    n7[0] = pos.x - hd;
    n7[1] = pos.y + hd;
    n7[2] = hd;

    glBegin(GL_QUADS);
    {
        glNormal3f(0, 0, -1);
        glVertex3fv(n0);
        glVertex3fv(n3);
        glVertex3fv(n2);
        glVertex3fv(n1);

        glNormal3f(0, -1, 0);
        glVertex3fv(n0);
        glVertex3fv(n1);
        glVertex3fv(n5);
        glVertex3fv(n4);

        glNormal3f(-1, 0, 0);
        glVertex3fv(n0);
        glVertex3fv(n4);
        glVertex3fv(n7);
        glVertex3fv(n3);

        glNormal3f(0, 1, 0);
        glVertex3fv(n7);
        glVertex3fv(n6);
        glVertex3fv(n2);
        glVertex3fv(n3);

        glNormal3f(1, 0, 0);
        glVertex3fv(n6);
        glVertex3fv(n5);
        glVertex3fv(n1);
        glVertex3fv(n2);

        glNormal3f(0, 0, 1);
        glVertex3fv(n4);
        glVertex3fv(n5);
        glVertex3fv(n6);
        glVertex3fv(n7);
    }
    glEnd();
}

void MainViewer::drawOneCircle(zVec2 &pos, double sc, double hh, zVec4 color)
{
    glColor4f(color.x, color.y, color.z, color.a);
    glBegin(GL_TRIANGLE_FAN);
    int size = circle_node_number * 2;
    for (int i=0; i<size; i+=2)
    {
        GLfloat n0[3];
        n0[0] = circle_node_p[i]*sc + pos[0];
        n0[1] = circle_node_p[i+1]*sc + pos[1];
        n0[2] = hh;

        glNormal3f(0,0,1);
        glVertex3fv(n0);
    }
    glEnd();

//    GLUquadricObj* quadratic;
//    quadratic = gluNewQuadric();
//    gluQuadricNormals(quadratic, GLU_SMOOTH);
//    glPushMatrix();
//    glTranslatef(pos.x, pos.y, hh);
//    gluDisk(quadratic,0,DEFAULT_LANE_WIDTH * 0.2,32,32);
//    glPopMatrix();
}

//-----------------------------------------------------------------------------------
// Observations
void MainViewer::drawObservationPoints()
{
    vector<Node*> *p_observation_list = &traffic_system_p->observation_list;
    int num_observation = p_observation_list->size();

    for (int i=0; i<num_observation; ++i)
    {
        drawOneObservationPoint(p_observation_list->at(i));
    }
}

void MainViewer::drawOneObservationPoint(Node *p_node)
{
    zVec3 pos = p_node->position;
    zVec3 p0(pos.x, pos.y, visual_xy_plane_height*10.0);
    zVec3 p1(pos.x, pos.y, visual_xy_plane_height);

    double radius = traffic_system_p->model_parameter->ParticleRadius;
    drawArrowFrom(p0,p1,radius,p_node->color);

    // the id
    double hh = radius * 6;
    double ww = hh * 2;
    double translation[3] = {p0.x, p0.y, p0.z + hh/2};
    double rotation[3] = {90, 0, 0}; // rotation around x/y/z axis
    char str[1024];
    sprintf(str, "%i", p_node->id);
    drawFreeTypeOnObject(translation, rotation, ww, hh, p_node->color, str);

    rotation[2] = 180;
    drawFreeTypeOnObject(translation, rotation, ww, hh, p_node->color, str);
}

//-----------------------------------------------------------------------------------
// Particles
void MainViewer::drawOneParticle(Particle* p_part, zVec4 color)
{
    // Set color
    glColor4f(color.x, color.y, color.z, color.a);

    glPushMatrix();
    glTranslatef(p_part->position[0], p_part->position[1], 0.0);

    // Drawing particle
    //glutSolidSphere(sphereRadius, 12, 12);
    GLUquadricObj *quadObj;
    quadObj = gluNewQuadric ();
    gluSphere(quadObj, p_part->sphereRadius, 16, 16);

    glPopMatrix();
}

void MainViewer::drawSphere(zVec3 pos, zVec4 color)
{
    GLfloat n0[3], n1[3], n2[3];
    glColor4f(color.x, color.y, color.z, color.a);
    glBegin(GL_TRIANGLES);
    int count = 0;
    for (int i=0; i<sphere_face_number; ++i)
    {
        n0[0] = sphere_node_p[count+0] + pos[0];
        n0[1] = sphere_node_p[count+1] + pos[1];
        n0[2] = sphere_node_p[count+2] + pos[2];
        n1[0] = sphere_node_p[count+3] + pos[0];
        n1[1] = sphere_node_p[count+4] + pos[1];
        n1[2] = sphere_node_p[count+5] + pos[2];
        n2[0] = sphere_node_p[count+6] + pos[0];
        n2[1] = sphere_node_p[count+7] + pos[1];
        n2[2] = sphere_node_p[count+8] + pos[2];

        glNormal3fv(sphere_normal_p+count);
        glVertex3fv(n0);
        glNormal3fv(sphere_normal_p+count+3);
        glVertex3fv(n1);
        glNormal3fv(sphere_normal_p+count+6);
        glVertex3fv(n2);

//        glNormal3fv(sphere_normal_p+count);
//        glVertex3fv(n0);
//        glNormal3fv(sphere_normal_p+count+6);
//        glVertex3fv(n2);
//        glNormal3fv(sphere_normal_p+count+3);
//        glVertex3fv(n1);

        count += 9;
    }
    glEnd();

//    TriangleMesh *p_sphere = &traffic_system_p->model_parameter->sphere_mesh;
//    glColor4f(color.x, color.y, color.z, color.a);
//    glBegin(GL_TRIANGLES);
//    int num_face = p_sphere->triangle_list.size();
//    for (int i=0; i<num_face; ++i)
//    {
//        Triangle *p_tri = &p_sphere->triangle_list[i];

//        zVec3 n0 = p_tri->v0->position + pos;
//        zVec3 n1 = p_tri->v1->position + pos;
//        zVec3 n2 = p_tri->v2->position + pos;

//        glNormal3f(p_tri->v0->norm.x, p_tri->v0->norm.y, p_tri->v0->norm.z);
//        glVertex3f(n0.x, n0.y, n0.z);
//        glNormal3f(p_tri->v1->norm.x, p_tri->v1->norm.y, p_tri->v1->norm.z);
//        glVertex3f(n1.x, n1.y, n1.z);
//        glNormal3f(p_tri->v2->norm.x, p_tri->v2->norm.y, p_tri->v2->norm.z);
//        glVertex3f(n2.x, n2.y, n2.z);
//    }
//    glEnd();
}

void MainViewer::drawParticles()
{
    glShadeModel(GL_FLAT);

    // Setting Lighting materials
    glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);

    int size = traffic_system_p->particle_list.size();
    for (int i=0; i<size; ++i)
    {
        Particle* p_particle = traffic_system_p->particle_list[i];

        // Choose the color for the particle.
        zVec4 color;
        if (p_particle->isSelected)
        {
            color = particle_color_selected;
        }
        else if (p_particle->isAnchored)
        {
            color = particle_color_anchored;
        }
        else if (p_particle->isActive)
        {
            color = particle_color_active;
        }
        else if (p_particle->isNeighbor)
        {
            color = particle_color_neighbor;
        }
        else
        {
            color = particle_color;
        }

        //        drawOneParticle(p_particle, color);
        if (p_particle->isInRoad)
        {
            drawSphere(zVec3(p_particle->position.x, p_particle->position.y, 0), color);
        }
        else
        {
            color.x += 0.3;
            color.z -= 0.3;
            drawSphere(zVec3(p_particle->position.x, p_particle->position.y, 0), color);
        }
    }

    if (traffic_system_p->model_parameter->SimulationMode == 2)
    {
        // draw the direction of force
        if (showParticleForce)
        {
            glLineWidth(3.0);
            glColor4f(0, 0, 1, 1);
            glBegin(GL_LINES);
            for (int i=0; i<size; ++i)
            {
                Particle* p_part = traffic_system_p->particle_list[i];

                GLfloat n0[3],n1[3];
                n0[0] = p_part->position.x;
                n0[1] = p_part->position.y;
                n0[2] = 0.0;

                zVec2 vv = p_part->position + p_part->trafficForce * 0.05;
                n1[0] = vv[0];
                n1[1] = vv[1];
                n1[2] = 0.0;

                glVertex3fv(n0);
                glVertex3fv(n1);
            }
            glEnd();
        }

        // draw the direction of force
        if (showParticleVelocityDirection)
        {
            glLineWidth(3.0);
            glColor4f(0, 0, 1, 1);
            glBegin(GL_LINES);
            for (int i=0; i<size; ++i)
            {
                Particle* p_part = traffic_system_p->particle_list[i];

                GLfloat n0[3],n1[3];
                n0[0] = p_part->position.x;
                n0[1] = p_part->position.y;
                n0[2] = 0.0;

                zVec2 vv = p_part->position + p_part->velocity * 0.05;
                n1[0] = vv[0];
                n1[1] = vv[1];
                n1[2] = 0.0;

                glVertex3fv(n0);
                glVertex3fv(n1);
            }
            glEnd();
        }

        // draw the normal and tangent of particles
        if (showParticleNormalTangent)
        {
            glLineWidth(3.0);
            glColor4f(0.5, 1, 0, 1);
            double rr = traffic_system_p->model_parameter->ParticleRadius;
            double len = DEFAULT_LANE_WIDTH * 0.3;
            glBegin(GL_LINES);
            for (int i=0; i<size; ++i)
            {
                Particle* p_part = traffic_system_p->particle_list[i];

                GLfloat n0[3],n1[3];
                n0[0] = p_part->position.x;
                n0[1] = p_part->position.y;
                n0[2] = rr;

                zVec2 vv = p_part->position + p_part->tangent * len;
                n1[0] = vv[0];
                n1[1] = vv[1];
                n1[2] = rr;

                glVertex3fv(n0);
                glVertex3fv(n1);
            }
            glEnd();

            glColor4f(1, 0, 0, 1);
            glBegin(GL_LINES);
            for (int i=0; i<size; ++i)
            {
                Particle* p_part = traffic_system_p->particle_list[i];

                GLfloat n0[3],n1[3];
                n0[0] = p_part->position.x;
                n0[1] = p_part->position.y;
                n0[2] = rr;

                zVec2 vv = p_part->position + p_part->normal * len;
                n1[0] = vv[0];
                n1[1] = vv[1];
                n1[2] = rr;

                glVertex3fv(n0);
                glVertex3fv(n1);
            }
            glEnd();
        }
    }
//    else
//    {
//        int num_roads = traffic_system_p->road_list.size();
//        for (int i=0; i<num_roads; ++i)
//        {
//            Road *p_road = &traffic_system_p->road_list[i];
//            for (int j=0; j<p_road->num_lanes; ++j)
//            {
//                vector<Particle*> *p_list = &p_road->particle_lists[j];
//                int size = p_list->size();
//                for (int k=0; k<size; ++k)
//                {
//                    Particle* p_particle = p_list->at(k);

//                    // Choose the color for the particle.
//                    zVec4 color;
//                    if (p_particle->isSelected)
//                    {
//                        color = particle_color_selected;
//                    }
//                    else if (p_particle->isAnchored)
//                    {
//                        color = particle_color_anchored;
//                    }
//                    else if (p_particle->isActive)
//                    {
//                        color = particle_color_active;
//                    }
//                    else if (p_particle->isNeighbor)
//                    {
//                        color = particle_color_neighbor;
//                    }
//                    else if (p_particle->lanechanging_status == 1)
//                    {
//                        color = zVec4(1,0,0,p_particle->mass/traffic_system_p->model_parameter->ParticleMass);
//                    }
//                    else if (p_particle->lanechanging_status == 2)
//                    {
//                        color = zVec4(0,1,0,p_particle->mass/traffic_system_p->model_parameter->ParticleMass);
//                    }
//                    else
//                    {
//                        color = particle_color;
//                    }

//                    //        drawOneParticle(p_particle, color);
//                    zVec3 pos(p_particle->position.x, p_particle->position.y, 0);
//                    drawSphere(pos, color);
//                }
//            }
//        }
//    }

    // draw arrow of the active particle
//    if (part_selected && part_selected->isActive)
//    {
//        glm::vec3 p0(0,0,0), p1(0,0,0);
//        p0.x = part_selected->position.x;
//        p0.y = part_selected->position.y;

//        double len = 5 * part_selected->external_force.length();
//        p1.x = p0.x + part_selected->external_force.x / len;
//        p1.y = p0.y + part_selected->external_force.y / len;

//        //drawArrowFrom(p0, p1);
//    }

    glShadeModel(GL_SMOOTH);
}

//-----------------------------------------------------------------------------------
// Field meshes
void MainViewer::drawDrains()
{
//    glEnable (GL_BLEND);
//    glDisable (GL_DEPTH_TEST);

    // Setting Lighting materials
    glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);

    glColor4f(0, 0, 1, 0.5);
    glBegin(GL_TRIANGLES);
    int num_road = traffic_system_p->road_list.size();
    for (int k=0; k<num_road; ++k)
    {
        Road *p_road = traffic_system_p->road_list[k];

        int num_face = p_road->drain_1.triangle_list.size();
        for (int i=0; i<num_face; ++i)
        {
            Triangle* p_tri = &p_road->drain_1.triangle_list[i];

            // one side of the triangle
            glNormal3f(p_tri->v0->norm.x, p_tri->v0->norm.y, p_tri->v0->norm.z);
            glVertex3f(p_tri->v0->position.x, p_tri->v0->position.y, p_tri->v0->position.z);

            glNormal3f(p_tri->v1->norm.x, p_tri->v1->norm.y, p_tri->v1->norm.z);
            glVertex3f(p_tri->v1->position.x, p_tri->v1->position.y, p_tri->v1->position.z);

            glNormal3f(p_tri->v2->norm.x, p_tri->v2->norm.y, p_tri->v2->norm.z);
            glVertex3f(p_tri->v2->position.x, p_tri->v2->position.y, p_tri->v2->position.z);
        }

        num_face = p_road->drain_2.triangle_list.size();
        for (int i=0; i<num_face; ++i)
        {
            Triangle* p_tri = &p_road->drain_2.triangle_list[i];

            // one side of the triangle
            glNormal3f(p_tri->v0->norm.x, p_tri->v0->norm.y, p_tri->v0->norm.z);
            glVertex3f(p_tri->v0->position.x, p_tri->v0->position.y, p_tri->v0->position.z);

            glNormal3f(p_tri->v1->norm.x, p_tri->v1->norm.y, p_tri->v1->norm.z);
            glVertex3f(p_tri->v1->position.x, p_tri->v1->position.y, p_tri->v1->position.z);

            glNormal3f(p_tri->v2->norm.x, p_tri->v2->norm.y, p_tri->v2->norm.z);
            glVertex3f(p_tri->v2->position.x, p_tri->v2->position.y, p_tri->v2->position.z);
        }
    }
    glEnd();

//    glDisable(GL_BLEND);
//    glEnable(GL_DEPTH_TEST);
}

void MainViewer::drawMesh()
{
    // Setting Lighting materials
    glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);

    TriangleMesh *p_mesh = &traffic_system_p->field_mesh_density;
    glColor4f(0.0, 0.7, 0.7, 0.6);
    glBegin(GL_TRIANGLES);
    int num_face = p_mesh->triangle_list.size();
    for (int i=0; i<num_face; ++i)
    {
        Triangle *f = &p_mesh->triangle_list[i];
        zVec3 n0, n1, n2;

        n0[0] = f->v0->position.x;
        n0[1] = f->v0->position.y;
        n0[2] = f->v0->density * axis_length* 0.4;

        n1[0] = f->v1->position.x;
        n1[1] = f->v1->position.y;
        n1[2] = f->v1->density * axis_length* 0.4;

        n2[0] = f->v2->position.x;
        n2[1] = f->v2->position.y;
        n2[2] = f->v2->density * axis_length* 0.4;

        glNormal3f(0, 1, 0);
        glVertex3f(n0.x, n0.y, n0.z);
        glVertex3f(n1.x, n1.y, n1.z);
        glVertex3f(n2.x, n2.y, n2.z);

        glNormal3f(0, -1, 0);
        glVertex3f(n0.x, n0.y, n0.z);
        glVertex3f(n2.x, n2.y, n2.z);
        glVertex3f(n1.x, n1.y, n1.z);
    }
    glEnd();
}

void MainViewer::drawFieldMeshDensity()
{
    glDisable(GL_CULL_FACE);     // let both sides visible
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture_colormap);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    // Setting Lighting materials
    glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);

    if (traffic_system_p->model_parameter->SimulationMode == 2 ||
       (traffic_system_p->model_parameter->SimulationMode == 9 && traffic_system_p->isTwoDimension))
    {
        TriangleMesh *p_mesh = &traffic_system_p->field_mesh_density;
        GLfloat n0[3], n1[3], n2[3], norm0[3], norm1[3], norm2[3];
        glColor4f(1.0, 1.0, 1.0, 1.0);
        glBegin(GL_TRIANGLES);
        int num_face = p_mesh->triangle_list.size();
        for (int i=0; i<num_face; ++i)
        {
            Triangle *f = &p_mesh->triangle_list[i];

            Node *p_node0 = f->v0;
            n0[0] = p_node0->position.x;
            n0[1] = p_node0->position.y;
            n0[2] = p_node0->position.z;
            norm0[0] = p_node0->norm.x;
            norm0[1] = p_node0->norm.y;
            norm0[2] = p_node0->norm.z;

            Node *p_node1 = f->v1;
            n1[0] = p_node1->position.x;
            n1[1] = p_node1->position.y;
            n1[2] = p_node1->position.z;
            norm1[0] = p_node1->norm.x;
            norm1[1] = p_node1->norm.y;
            norm1[2] = p_node1->norm.z;

            Node *p_node2 = f->v2;
            n2[0] = p_node2->position.x;
            n2[1] = p_node2->position.y;
            n2[2] = p_node2->position.z;
            norm2[0] = p_node2->norm.x;
            norm2[1] = p_node2->norm.y;
            norm2[2] = p_node2->norm.z;

            // one side of the triangle
            glNormal3fv(norm0);
            glTexCoord2f(0.5, p_node0->normalized_density);
            glVertex3fv(n0);

            glNormal3fv(norm1);
            glTexCoord2f(0.5, p_node1->normalized_density);
            glVertex3fv(n1);

            glNormal3fv(norm2);
            glTexCoord2f(0.5, p_node2->normalized_density);
            glVertex3fv(n2);

            // the other side of the triangle
            glNormal3f(-norm0[0], -norm0[1], -norm0[2]);
            glTexCoord2f(0.5, p_node0->normalized_density);
            glVertex3fv(n0);

            glNormal3f(-norm2[0], -norm2[1], -norm2[2]);
            glTexCoord2f(0.5, p_node2->normalized_density);
            glVertex3fv(n2);

            glNormal3f(-norm1[0], -norm1[1], -norm1[2]);
            glTexCoord2f(0.5, p_node1->normalized_density);
            glVertex3fv(n1);
        }
        glEnd();
    }

    else if (traffic_system_p->model_parameter->SimulationMode == 1 ||
            (traffic_system_p->model_parameter->SimulationMode == 9 && !traffic_system_p->isTwoDimension))
    {
        glColor4f(1.0, 1.0, 1.0, 1.0);
        int num_road = traffic_system_p->road_list.size();
        for (int k=0; k<num_road; ++k)
        {
            vector<TriangleMesh*> *p_mesh_list = &traffic_system_p->road_list[k]->lane_density_meshes;
            for (int j=0; j<p_mesh_list->size(); j+=1)
            {
                TriangleMesh *p_mesh = p_mesh_list->at(j);

                glBegin(GL_TRIANGLE_STRIP);
                int num_node = p_mesh->node_list.size();
                for (int i=0; i<num_node; ++i)
                {
                    Node *p_node0 = p_mesh->node_list[i];

                    GLfloat n0[3], norm0[3];

                    n0[0] = p_node0->position.x;
                    n0[1] = p_node0->position.y;
                    n0[2] = p_node0->position.z;
                    norm0[0] = p_node0->norm.x;
                    norm0[1] = p_node0->norm.y;
                    norm0[2] = p_node0->norm.z;

                    // one side of the triangle
                    glNormal3fv(norm0);
                    glTexCoord2f(0.5, p_node0->normalized_density);
                    glVertex3fv(n0);
                }
                glEnd();
            }
        }

//        glBegin(GL_TRIANGLES);
//        int num_road = traffic_system_p->road_list.size();
//        for (int k=0; k<num_road; ++k)
//        {
//            vector<TriangleMesh*> *p_mesh_list = &traffic_system_p->road_list[k].lane_density_meshes;
//            for (int j=0; j<p_mesh_list->size(); j+=1)
//            {
//                TriangleMesh *p_mesh = p_mesh_list->at(j);
//                int num_face = p_mesh->triangle_list.size();
//                for (int i=0; i<num_face; ++i)
//                {
//                    Triangle *f = &p_mesh->triangle_list[i];

//                    Node *p_node0 = f->v0;
//                    n0[0] = p_node0->position.x;
//                    n0[1] = p_node0->position.y;
//                    n0[2] = p_node0->position.z;
//                    norm0[0] = p_node0->norm.x;
//                    norm0[1] = p_node0->norm.y;
//                    norm0[2] = p_node0->norm.z;

//                    Node *p_node1 = f->v1;
//                    n1[0] = p_node1->position.x;
//                    n1[1] = p_node1->position.y;
//                    n1[2] = p_node1->position.z;
//                    norm1[0] = p_node1->norm.x;
//                    norm1[1] = p_node1->norm.y;
//                    norm1[2] = p_node1->norm.z;

//                    Node *p_node2 = f->v2;
//                    n2[0] = p_node2->position.x;
//                    n2[1] = p_node2->position.y;
//                    n2[2] = p_node2->position.z;
//                    norm2[0] = p_node2->norm.x;
//                    norm2[1] = p_node2->norm.y;
//                    norm2[2] = p_node2->norm.z;

//                    // one side of the triangle
//                    glNormal3fv(norm0);
//                    glTexCoord2f(0.5, p_node0->normalized_density);
//                    glVertex3fv(n0);

//                    glNormal3fv(norm1);
//                    glTexCoord2f(0.5, p_node1->normalized_density);
//                    glVertex3fv(n1);

//                    glNormal3fv(norm2);
//                    glTexCoord2f(0.5, p_node2->normalized_density);
//                    glVertex3fv(n2);

//                    // the other side of the triangle
//                    glNormal3f(-norm0[0], -norm0[1], -norm0[2]);
//                    glTexCoord2f(0.5, p_node0->normalized_density);
//                    glVertex3fv(n0);

//                    glNormal3f(-norm2[0], -norm2[1], -norm2[2]);
//                    glTexCoord2f(0.5, p_node2->normalized_density);
//                    glVertex3fv(n2);

//                    glNormal3f(-norm1[0], -norm1[1], -norm1[2]);
//                    glTexCoord2f(0.5, p_node1->normalized_density);
//                    glVertex3fv(n1);
//                }
//            }
//        }
//        glEnd();
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);

    // The reference planes.
    if (showReferencePlanes)
    {
        drawReferencePlanes();
    }
}

void MainViewer::drawColormapDensity()
{
    drawColormap(0, 0, 20, 80);
    QString str;
    str.sprintf("%03.1f", traffic_system_p->model_parameter->MaxDensity);
    drawFreeTypeBoldOnScreen(22, 70, 16, zVec4(0,0,0,1), str.toLatin1());
    str.sprintf("%03.1f", 0.0);
    drawFreeTypeBoldOnScreen(22, 0, 16, zVec4(0,0,0,1), str.toLatin1());
}

void MainViewer::drawFieldMeshVelocity()
{
    glDisable(GL_CULL_FACE);     // let both sides visible
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture_colormap);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    // Setting Lighting materials
    glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);

    if (traffic_system_p->model_parameter->SimulationMode == 2 ||
       (traffic_system_p->model_parameter->SimulationMode == 9 && traffic_system_p->isTwoDimension))
    {
        TriangleMesh *p_mesh = &traffic_system_p->field_mesh_velocity;
        GLfloat n0[3], n1[3], n2[3], norm0[3], norm1[3], norm2[3];
        glColor4f(1.0, 1.0, 1.0, 1.0);
        glBegin(GL_TRIANGLES);
        int num_face = p_mesh->triangle_list.size();
        for (int i=0; i<num_face; ++i)
        {
            Triangle *f = &p_mesh->triangle_list[i];

            Node *p_node0 = f->v0;
            n0[0] = p_node0->position.x;
            n0[1] = p_node0->position.y;
            n0[2] = p_node0->position.z;
            norm0[0] = p_node0->norm.x;
            norm0[1] = p_node0->norm.y;
            norm0[2] = p_node0->norm.z;

            Node *p_node1 = f->v1;
            n1[0] = p_node1->position.x;
            n1[1] = p_node1->position.y;
            n1[2] = p_node1->position.z;
            norm1[0] = p_node1->norm.x;
            norm1[1] = p_node1->norm.y;
            norm1[2] = p_node1->norm.z;

            Node *p_node2 = f->v2;
            n2[0] = p_node2->position.x;
            n2[1] = p_node2->position.y;
            n2[2] = p_node2->position.z;
            norm2[0] = p_node2->norm.x;
            norm2[1] = p_node2->norm.y;
            norm2[2] = p_node2->norm.z;

            // one side of the triangle
            glNormal3fv(norm0);
            glTexCoord2f(0.5, p_node0->normalized_velocity);
            glVertex3fv(n0);

            glNormal3fv(norm1);
            glTexCoord2f(0.5, p_node1->normalized_velocity);
            glVertex3fv(n1);

            glNormal3fv(norm2);
            glTexCoord2f(0.5, p_node2->normalized_velocity);
            glVertex3fv(n2);

            // the other side of the triangle
            glNormal3f(-norm0[0], -norm0[1], -norm0[2]);
            glTexCoord2f(0.5, p_node0->normalized_velocity);
            glVertex3fv(n0);

            glNormal3f(-norm2[0], -norm2[1], -norm2[2]);
            glTexCoord2f(0.5, p_node2->normalized_velocity);
            glVertex3fv(n2);

            glNormal3f(-norm1[0], -norm1[1], -norm1[2]);
            glTexCoord2f(0.5, p_node1->normalized_velocity);
            glVertex3fv(n1);
        }
        glEnd();
    }

    else if (traffic_system_p->model_parameter->SimulationMode == 1 ||
            (traffic_system_p->model_parameter->SimulationMode == 9 && !traffic_system_p->isTwoDimension))
    {
        glColor4f(1.0, 1.0, 1.0, 1.0);
        int num_road = traffic_system_p->road_list.size();
        for (int k=0; k<num_road; ++k)
        {
            vector<TriangleMesh*> *p_mesh_list = &traffic_system_p->road_list[k]->lane_velocity_meshes;
            for (int j=0; j<p_mesh_list->size(); j+=1)
            {
                TriangleMesh *p_mesh = p_mesh_list->at(j);

                glBegin(GL_TRIANGLE_STRIP);
                int num_node = p_mesh->node_list.size();
                for (int i=0; i<num_node; ++i)
                {
                    Node *p_node0 = p_mesh->node_list[i];

                    GLfloat n0[3], norm0[3];

                    n0[0] = p_node0->position.x;
                    n0[1] = p_node0->position.y;
                    n0[2] = p_node0->position.z;
                    norm0[0] = p_node0->norm.x;
                    norm0[1] = p_node0->norm.y;
                    norm0[2] = p_node0->norm.z;

                    glNormal3fv(norm0);
                    glTexCoord2f(0.5, p_node0->normalized_velocity);
                    glVertex3fv(n0);
                }
                glEnd();
            }
        }

//        GLfloat n0[3], n1[3], n2[3], norm0[3], norm1[3], norm2[3];
//        glColor4f(1.0, 1.0, 1.0, 1.0);
//        glBegin(GL_TRIANGLES);
//        int num_road = traffic_system_p->road_list.size();
//        for (int k=0; k<num_road; ++k)
//        {
//            vector<TriangleMesh*> *p_mesh_list = &traffic_system_p->road_list[k].lane_velocity_meshes;
//            for (int j=0; j<p_mesh_list->size(); j+=1)
//            {
//                TriangleMesh *p_mesh = p_mesh_list->at(j);
//                int num_face = p_mesh->triangle_list.size();
//                for (int i=0; i<num_face; ++i)
//                {
//                    Triangle *f = &p_mesh->triangle_list[i];

//                    Node *p_node0 = f->v0;
//                    n0[0] = p_node0->position.x;
//                    n0[1] = p_node0->position.y;
//                    n0[2] = p_node0->position.z;
//                    norm0[0] = p_node0->norm.x;
//                    norm0[1] = p_node0->norm.y;
//                    norm0[2] = p_node0->norm.z;

//                    Node *p_node1 = f->v1;
//                    n1[0] = p_node1->position.x;
//                    n1[1] = p_node1->position.y;
//                    n1[2] = p_node1->position.z;
//                    norm1[0] = p_node1->norm.x;
//                    norm1[1] = p_node1->norm.y;
//                    norm1[2] = p_node1->norm.z;

//                    Node *p_node2 = f->v2;
//                    n2[0] = p_node2->position.x;
//                    n2[1] = p_node2->position.y;
//                    n2[2] = p_node2->position.z;
//                    norm2[0] = p_node2->norm.x;
//                    norm2[1] = p_node2->norm.y;
//                    norm2[2] = p_node2->norm.z;

//                    // one side of the triangle
//                    glNormal3fv(norm0);
//                    glTexCoord2f(0.5, p_node0->normalized_velocity);
//                    glVertex3fv(n0);

//                    glNormal3fv(norm1);
//                    glTexCoord2f(0.5, p_node1->normalized_velocity);
//                    glVertex3fv(n1);

//                    glNormal3fv(norm2);
//                    glTexCoord2f(0.5, p_node2->normalized_velocity);
//                    glVertex3fv(n2);

//                    // the other side of the triangle
//                    glNormal3f(-norm0[0], -norm0[1], -norm0[2]);
//                    glTexCoord2f(0.5, p_node0->normalized_velocity);
//                    glVertex3fv(n0);

//                    glNormal3f(-norm2[0], -norm2[1], -norm2[2]);
//                    glTexCoord2f(0.5, p_node2->normalized_velocity);
//                    glVertex3fv(n2);

//                    glNormal3f(-norm1[0], -norm1[1], -norm1[2]);
//                    glTexCoord2f(0.5, p_node1->normalized_velocity);
//                    glVertex3fv(n1);
//                }
//            }
//        }
//        glEnd();
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);

    // The reference planes.
    if (showReferencePlanes)
    {
        drawReferencePlanes();
    }
}

void MainViewer::drawColormapVelocity()
{
    drawColormap(0, 0, 20, 80);
    QString str;
    str.sprintf("%03.1f", traffic_system_p->model_parameter->MaxVelocity);
    drawFreeTypeBoldOnScreen(22, 70, 16, zVec4(0,0,0,1), str.toLatin1());
    str.sprintf("%03.1f", 0.0);
    drawFreeTypeBoldOnScreen(22, 0, 16, zVec4(0,0,0,1), str.toLatin1());
}

void MainViewer::drawReferencePlanes()
{
    double tt = traffic_system_p->field_mesh_top;
    double bb = traffic_system_p->field_mesh_bottom;
    double dd = (tt-bb)/3.0;

    glDisable(GL_CULL_FACE);
//    glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_BLEND);
    glBindTexture(GL_TEXTURE_2D, texture_colormap);

    drawOneRoadPlane(bb);
    drawOneRoadPlane(bb+dd);
    drawOneRoadPlane(bb+2*dd);
    drawOneRoadPlane(bb+3*dd);

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}

void MainViewer::drawOneRoadPlane(double height)
{
    double tt = traffic_system_p->field_mesh_top;
    double bb = traffic_system_p->field_mesh_bottom;
    double height_normalized = (height - bb) / (tt - bb);

    // Set desired texture color.
    glTexCoord2f(0.5, height_normalized);

    // Draw the road plane.
    glEnable(GL_TEXTURE_2D);
    glColor4f(1.0, 1.0, 1.0, 0.2);
    int num_road = traffic_system_p->road_list.size();
    for (int k=0; k<num_road; ++k)
    {
        Road *p_road = traffic_system_p->road_list[k];
        glBegin(GL_TRIANGLE_STRIP);
        int num_seg = p_road->visualroad_side_left.size();
        for (int j=0; j<num_seg; ++j)
        {
            GLfloat n0[3], n1[3];

            n0[0] = p_road->visualroad_side_left[j].x;
            n0[1] = p_road->visualroad_side_left[j].y;
            n0[2] = height;

            n1[0] = p_road->visualroad_side_right[j].x;
            n1[1] = p_road->visualroad_side_right[j].y;
            n1[2] = height;

            glVertex3fv(n0);
            glVertex3fv(n1);
        }
        glEnd();
    }

    // Draw the contour of the road plane.
    glDisable(GL_TEXTURE_2D);
    glColor4f(0.8, 0.8, 0.8, 1);
    for (int k=0; k<num_road; ++k)
    {
        Road *p_road = traffic_system_p->road_list[k];

        glBegin(GL_LINE_LOOP);
        int num_seg = p_road->visualroad_side_left.size();
        for (int j=0; j<num_seg; ++j)
        {
            GLfloat n0[3];

            n0[0] = p_road->visualroad_side_left[j].x;
            n0[1] = p_road->visualroad_side_left[j].y;
            n0[2] = height;

            glVertex3fv(n0);
        }
        num_seg = p_road->visualroad_side_right.size();
        for (int j=num_seg-1; j>=0; --j)
        {
            GLfloat n0[3];

            n0[0] = p_road->visualroad_side_right[j].x;
            n0[1] = p_road->visualroad_side_right[j].y;
            n0[2] = height;

            glVertex3fv(n0);
        }
        glEnd();
    }
}

//-----------------------------------------------------------------------------------
// Real maps
void MainViewer::drawRealMaps()
{
    // Do not need lighting
    glDisable(GL_LIGHTING);

    vector<RoadLink*> *p_roadlink_list =
            &traffic_system_p->model_parameter->real_map->roadlink_list;
    int num_roadlink = p_roadlink_list->size();

    // Draw polylines of the roads.
    for (int k=0; k<num_roadlink; ++k)
    {
        RoadLink *p_roadlink = p_roadlink_list->at(k);
        if (p_roadlink->isVisible && p_roadlink->link_type != beziers)
        {
            double hh = 0;
            if (p_roadlink->link_type == highway)
            {
                glLineWidth(2.5);
                glColor4f(color_road[0], color_road[1],
                          color_road[2], color_road[3]);
                hh = -0.1;
            }
            else if (p_roadlink->link_type == building)
            {
                glLineWidth(1.5);
                glColor4f(0, 0.8, 0, 1);
                hh = -0.2;
            }
            else if (p_roadlink->link_type == railway)
            {
                glLineWidth(3.5);
                glColor4f(0.8, 0, 0.8, 1);
                hh = -0.3;
            }
            else // otherhighways
            {
                glLineWidth(2.5);
                glColor4f(0.7,0.7,0,1);
                hh = -0.4;
            }

            glBegin(GL_LINE_STRIP);
            vector<RoadNode*> *p_pa = &p_roadlink->roadnode_list;
            int num_seg = p_pa->size();
            for (int j=0; j<num_seg; ++j)
            {
                GLfloat n0[3];

                n0[0] = (*p_pa)[j]->position.x;
                n0[1] = (*p_pa)[j]->position.y;
                n0[2] = hh;

                glVertex3fv(n0);
            }
            glEnd();
        }
    }

    glEnable(GL_LIGHTING);
}

//-----------------------------------------------------------------------------------
// Road networks
void MainViewer::drawRoads(zVec4 color)
{
    // Do not need lighting
    glDisable(GL_LIGHTING);

    double h_road = -0.02;
    double h_lane = -0.01;
    double h_bound = -0.005;

    int num_road = traffic_system_p->road_list.size();

    // Draw the road.
    glColor4f(color.x, color.y, color.z, color.a);
    for (int k=0; k<num_road; ++k)
    {
        Road *p_road = traffic_system_p->road_list[k];
        if (p_road->isVisible)
        {
            glBegin(GL_TRIANGLE_STRIP);
            int num_seg = p_road->visualroad_side_left.size();
            for (int j=0; j<num_seg; ++j)
            {
                GLfloat n0[3], n1[3];

                n0[0] = p_road->visualroad_side_left[j].x;
                n0[1] = p_road->visualroad_side_left[j].y;
                n0[2] = h_road;

                n1[0] = p_road->visualroad_side_right[j].x;
                n1[1] = p_road->visualroad_side_right[j].y;
                n1[2] = h_road;

                glVertex3fv(n0);
                glVertex3fv(n1);
            }
            glEnd();
        }
    }

    // Draw the solid and dash lanes.
    glLineWidth(2.5);
    glColor4f(color_lane.x, color_lane.y, color_lane.z, color_lane.a);
    for (int k=0; k<num_road; ++k)
    {
        Road *p_road = traffic_system_p->road_list[k];
        if (p_road->isVisible)
        {
            glBegin(GL_LINE_STRIP);
            PointArray *p_pa = &p_road->visuallane_solid[0];
            int num_seg = p_pa->size();
            for (int j=0; j<num_seg; ++j)
            {
                GLfloat n0[3];

                n0[0] = (*p_pa)[j].x;
                n0[1] = (*p_pa)[j].y;
                n0[2] = h_lane;

                glVertex3fv(n0);
            }
            glEnd();

            glBegin(GL_LINE_STRIP);
            int num_lanes = p_road->num_lanes;
            p_pa = &p_road->visuallane_solid[num_lanes];
            num_seg = p_pa->size();
            for (int j=0; j<num_seg; ++j)
            {
                GLfloat n0[3];

                n0[0] = (*p_pa)[j].x;
                n0[1] = (*p_pa)[j].y;
                n0[2] = h_lane;

                glVertex3fv(n0);
            }
            glEnd();

            glBegin(GL_LINES);
            for (int i=1; i<num_lanes; ++i)
            {
                p_pa = &p_road->visuallane_dash[i];

                num_seg = p_pa->size();
                for (int j=1; j<num_seg; j += 2)
                {
                    GLfloat n0[3], n1[3];

                    n0[0] = (*p_pa)[j-1].x;
                    n0[1] = (*p_pa)[j-1].y;
                    n0[2] = h_lane;

                    n1[0] = (*p_pa)[j].x;
                    n1[1] = (*p_pa)[j].y;
                    n1[2] = h_lane;

                    //glNormal3f(0, 0, -1);
                    glVertex3fv(n0);
                    glVertex3fv(n1);
                }
            }
            glEnd();
        }
    }

    // Draw source and destination area.
    glLineWidth(2.0);
    glBegin(GL_QUADS);
    for (int k=0; k<num_road; ++k)
    {
        if (traffic_system_p->road_list[k]->isVisible)
        {
            CellSequence *p_bound = &traffic_system_p->road_list[k]->bound_area;
            if (p_bound->cells.size() > 0)
            {
                Cell *p_ele = p_bound->cells[0];
                glColor4f(0.0, 0.0, 0.8, 0.8);
                glVertex3f(p_ele->v[0].x, p_ele->v[0].y, h_lane);
                glVertex3f(p_ele->v[1].x, p_ele->v[1].y, h_lane);

                glVertex3f(p_ele->v[1].x, p_ele->v[1].y, h_lane);
                glVertex3f(p_ele->v[2].x, p_ele->v[2].y, h_lane);

                glVertex3f(p_ele->v[2].x, p_ele->v[2].y, h_lane);
                glVertex3f(p_ele->v[3].x, p_ele->v[3].y, h_lane);

                glVertex3f(p_ele->v[3].x, p_ele->v[3].y, h_lane);
                glVertex3f(p_ele->v[0].x, p_ele->v[0].y, h_lane);

                int last_id = p_bound->cells.size()-1;
                p_ele = p_bound->cells[last_id];
                glColor4f(0.8, 0.0, 0.0, 0.8);
                glVertex3f(p_ele->v[0].x, p_ele->v[0].y, h_lane);
                glVertex3f(p_ele->v[1].x, p_ele->v[1].y, h_lane);

                glVertex3f(p_ele->v[1].x, p_ele->v[1].y, h_lane);
                glVertex3f(p_ele->v[2].x, p_ele->v[2].y, h_lane);

                glVertex3f(p_ele->v[2].x, p_ele->v[2].y, h_lane);
                glVertex3f(p_ele->v[3].x, p_ele->v[3].y, h_lane);

                glVertex3f(p_ele->v[3].x, p_ele->v[3].y, h_lane);
                glVertex3f(p_ele->v[0].x, p_ele->v[0].y, h_lane);
            }
        }
    }
    glEnd();

    // Draw big bounding box of the road net.
    if (showRoadBoundingBox)
    {
        glLineWidth(2.0);
        glColor4f(1.0, 0.0, 0.0, 1.0);
        glBegin(GL_LINE_LOOP);

        zVec2 min_corner = traffic_system_p->min_corner;
        zVec2 max_corner = traffic_system_p->max_corner;

        glVertex3f(min_corner.x, min_corner.y, h_bound);
        glVertex3f(max_corner.x, min_corner.y, h_bound);
        glVertex3f(max_corner.x, max_corner.y, h_bound);
        glVertex3f(min_corner.x, max_corner.y, h_bound);

        glEnd();
    }

    glEnable(GL_LIGHTING);
}

void MainViewer::drawBoundaryZone()
{
    glDisable(GL_LIGHTING);

    double hh = visual_xy_plane_height;

    glLineWidth(2.0);
    glColor4f(1, 0, 0, 0.8);
    int num_road = traffic_system_p->road_list.size();
    for (int k=0; k<num_road; ++k)
    {
        vector<Cell*> *p_cells = &traffic_system_p->road_list[k]->bound_area.cells;

        glBegin(GL_LINE_STRIP);
        int num_zone = p_cells->size();
        for (int j=0; j<num_zone; ++j)
        {
            Cell *p_ele = p_cells->at(j);
            glVertex3f(p_ele->v[0].x, p_ele->v[0].y, hh);
            glVertex3f(p_ele->v[1].x, p_ele->v[1].y, hh);
            glVertex3f(p_ele->v[2].x, p_ele->v[2].y, hh);
            glVertex3f(p_ele->v[3].x, p_ele->v[3].y, hh);
            glVertex3f(p_ele->v[0].x, p_ele->v[0].y, hh);
        }
        glEnd();
    }

    // highlight the overlappong cells.
    glLineWidth(2.0);
    glColor4f(0.2, 0.5, 0, 0.5);
    glBegin(GL_QUADS);
    for (int k=0; k<num_road; ++k)
    {
        vector<Cell*> *p_cells = &traffic_system_p->road_list[k]->bound_area.cells;

        int num_zone = p_cells->size();
        for (int j=0; j<num_zone; ++j)
        {
            Cell *p_ele = p_cells->at(j);
            if (p_ele->isOverlappingCell)
            {
                glVertex3f(p_ele->v[0].x, p_ele->v[0].y, visual_xy_plane_height);
                glVertex3f(p_ele->v[1].x, p_ele->v[1].y, visual_xy_plane_height);

                glVertex3f(p_ele->v[1].x, p_ele->v[1].y, visual_xy_plane_height);
                glVertex3f(p_ele->v[2].x, p_ele->v[2].y, visual_xy_plane_height);

                glVertex3f(p_ele->v[2].x, p_ele->v[2].y, visual_xy_plane_height);
                glVertex3f(p_ele->v[3].x, p_ele->v[3].y, visual_xy_plane_height);

                glVertex3f(p_ele->v[3].x, p_ele->v[3].y, visual_xy_plane_height);
                glVertex3f(p_ele->v[0].x, p_ele->v[0].y, visual_xy_plane_height);
            }
        }
    }
    glEnd();

    glEnable(GL_LIGHTING);
}

void MainViewer::drawRoadSensors()
{
    int num_sensor_lists = traffic_system_p->sensor_lists.size();
    for (int k=0; k<num_sensor_lists; ++k)
    {
        vector<Node> *p_sensor_list = &traffic_system_p->sensor_lists[k];
        int num_sensors = p_sensor_list->size();
        for (int i=0; i<num_sensors; ++i)
        {
            Node *p_node = &p_sensor_list->at(i);

            zVec3 pos = p_node->position;
            double sc = 2.0;
            double hh = 0.001;
            glColor4f(1.0, 0.0, 0, 1);
            glBegin(GL_TRIANGLE_FAN);
            int size = circle_node_number * 2;
            for (int i=0; i<size; i+=2)
            {
                GLfloat n0[3];
                n0[0] = circle_node_p[i]*sc + pos[0];
                n0[1] = circle_node_p[i+1]*sc + pos[1];
                n0[2] = hh;

                glNormal3f(0,0,1);
                glVertex3fv(n0);
            }
            glEnd();
        }
    }
}

//-----------------------------------------------------------------------------------
// Cars
void MainViewer::drawCars()
{
    // Setting Lighting materials
    glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);

    int size = traffic_system_p->car_list.size();
    if (display_mode_P == 2)
    {
        for (int i=0; i<size; ++i)
        {
            drawOneCar(traffic_system_p->car_list[i]);
            drawOneCarBbox(traffic_system_p->car_list[i]);
        }
    }
    else if (display_mode_P == 3)
    {
        for (int i=0; i<size; ++i)
        {
//            drawOneCar(traffic_system_p->car_list[i]);
            drawOneCarBbox(traffic_system_p->car_list[i]);
//            drawOneCarRoundPoint(traffic_system_p->car_list[i], 3);
        }
    }

    if (showCarTail)
    {
        glDisable(GL_CULL_FACE);
        glEnable(GL_TEXTURE_2D);
//        glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_BLEND);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        glBindTexture(GL_TEXTURE_2D, texture_colormap);

        for (int i=0; i<size; ++i)
        {
            drawOneCarTail(traffic_system_p->car_list[i]);
        }

        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
        glEnable(GL_CULL_FACE);

//        for (int i=0; i<size; ++i)
//        {
//            drawOneCarHead(traffic_system_p->car_list[i]);
//        }

        drawColormapVelocity();
    }

    if (traffic_system_p->model_parameter->SimulationMode == 2)
    {
        // draw the direction of force
        if (showCarSpringForce)
        {
            glLineWidth(3.0);
            glColor4f(1, 0, 0, 1);
            glBegin(GL_LINES);
            for (int i=0; i<size; ++i)
            {
                CarParticle* p_car = traffic_system_p->car_list[i];

                GLfloat n0[3],n1[3];
                n0[0] = p_car->position.x;
                n0[1] = p_car->position.y;
                n0[2] = 0.0;

                zVec2 vv = p_car->position + p_car->spring_displacement;
                n1[0] = vv[0];
                n1[1] = vv[1];
                n1[2] = 0.0;

                glVertex3fv(n0);
                glVertex3fv(n1);
            }
            glEnd();
        }

        // draw the direction of velocity
        if (showCarVelocityDirection)
        {
            glLineWidth(3.0);
            glColor4f(0, 1, 0.8, 1);
            glBegin(GL_LINES);
            for (int i=0; i<size; ++i)
            {
                CarParticle* p_car = traffic_system_p->car_list[i];

                GLfloat n0[3],n1[3];
                n0[0] = p_car->position.x;
                n0[1] = p_car->position.y;
                n0[2] = 0.0;

                zVec2 vv = p_car->position + p_car->velocity * 0.1;
                n1[0] = vv[0];
                n1[1] = vv[1];
                n1[2] = 0.0;

                glVertex3fv(n0);
                glVertex3fv(n1);
            }
            glEnd();
        }
    }

    else if (traffic_system_p->model_parameter->SimulationMode == 0)
    {
        if (showCarTrajectory)
        {
            double hh = DEFAULT_LANE_WIDTH * 0.1;
            for (int i=0; i<size; ++i)
            {
                CarParticle *p_car = traffic_system_p->car_list[i];
                if (p_car->trajectory)
                {
                    int num_points = p_car->trajectory->input_points.size();
                    for (int j=0; j<num_points; ++j)
                    {
                        drawOneCircle(p_car->trajectory->input_points[j],
                                      1.0, hh, color_bezier_selected);
                    }

                    // connect the trajectory points
                    glLineWidth(2.0);
                    glColor4f(color_bezier.x, color_bezier.y, color_bezier.z, color_bezier.a);
                    glBegin(GL_LINE_STRIP);
                    for (int j=0; j<num_points; ++j)
                    {
                        zVec2 pos_0 = p_car->trajectory->input_points[j];
                        glVertex3f(pos_0.x, pos_0.y, hh);
                    }
                    glEnd();

                    // connect to the projected points
                    glColor4f(0, 1, 0, 1);
                    glBegin(GL_LINES);
                    for (int j=0; j<num_points; ++j)
                    {
                        zVec2 pos_0 = p_car->trajectory->input_points[j];
                        zVec2 pos_1 = p_car->trajectory->projected_point[j];
                        glVertex3f(pos_0.x, pos_0.y, hh);
                        glVertex3f(pos_1.x, pos_1.y, hh);
                    }
                    glEnd();
                }
            }
        }
    }
//        // draw the real blocks.
//        size = block_list.size();
//        for (int i=0; i<size; ++i)
//        {
//            drawOneBlockBarrier(block_list[i]);
//        }

//    else
//    {
//        int num_roads = traffic_system_p->road_list.size();
//        for (int i=0; i<num_roads; ++i)
//        {
//            Road *p_road = &traffic_system_p->road_list[i];
//            for (int j=0; j<p_road->num_lanes; ++j)
//            {
//                vector<CarParticle*> *p_list = &p_road->car_lists[j];
//                int size = p_list->size();
//                for (int k=0; k<size; ++k)
//                {
////                    drawOneCar(p_list->at(k));
//                    drawOneCarBbox(p_list->at(k));
//                }
//            }
//        }
//    }
}

//void MainViewer::drawOneCar(CarParticle *p_car)
//{
//    glPushMatrix();
//    glTranslated(p_car->position.x, p_car->position.y, 0.0);
//    glRotatef(atan2(p_car->orientation.y, p_car->orientation.x)*180.0/PI, 0, 0, 1);

//    glColor4f(p_car->color.x, p_car->color.y, p_car->color.z, p_car->color.a);
//    glBegin(GL_TRIANGLES);
//    int num_face = p_car->carmesh->triangle_list.size();
//    for (int i=0; i<num_face; ++i)
//    {
//        Triangle *p_tri = &p_car->carmesh->triangle_list[i];
//        zVec3 norm = p_tri->norm;

//        // determine the new vertices
//        zVec3 n0 = p_tri->v0->original_position;
//        zVec3 n1 = p_tri->v1->original_position;
//        zVec3 n2 = p_tri->v2->original_position;

//        glNormal3f(norm.x, norm.y, norm.z);
//        glVertex3f(n0.x, n0.y, n0.z);
//        glVertex3f(n1.x, n1.y, n1.z);
//        glVertex3f(n2.x, n2.y, n2.z);

//        glNormal3f(-norm.x, -norm.y, -norm.z);
//        glVertex3f(n0.x, n0.y, n0.z);
//        glVertex3f(n2.x, n2.y, n2.z);
//        glVertex3f(n1.x, n1.y, n1.z);
//    }
//    glEnd();
//    glPopMatrix();
//}

void MainViewer::drawOneCar(CarParticle *p_car)
{
    glPushMatrix();
    glTranslated(p_car->position.x, p_car->position.y, 0.0);
    glRotatef(atan2(p_car->orientation.y, p_car->orientation.x)*180.0/PI, 0, 0, 1);

    glColor4f(p_car->carcolor.x, p_car->carcolor.y, p_car->carcolor.z, p_car->carcolor.a);
    if (p_car->lanechanging_status == 1 ||
        (p_car->lanechanging_status == 0 && p_car->needLaneChanging))
    {
        glColor4f(1,1,1,1);
    }
    else if (p_car->lanechanging_status == 2)
    {
        glColor4f(1,1,1,0.5);
    }
    glBegin(GL_TRIANGLES);
    int num_face = p_car->carmesh->triangle_list.size();
    for (int i=0; i<num_face; ++i)
    {
        Triangle *p_tri = &p_car->carmesh->triangle_list[i];
        zVec3 norm = p_tri->norm;

        // determine the new vertices
        zVec3 n0 = p_tri->v0->original_position;
        zVec3 n1 = p_tri->v1->original_position;
        zVec3 n2 = p_tri->v2->original_position;

        glNormal3f(norm.x, norm.y, norm.z);
        glVertex3f(n0.x, n0.y, n0.z);
        glVertex3f(n1.x, n1.y, n1.z);
        glVertex3f(n2.x, n2.y, n2.z);

//        glNormal3f(-norm.x, -norm.y, -norm.z);
//        glVertex3f(n0.x, n0.y, n0.z);
//        glVertex3f(n2.x, n2.y, n2.z);
//        glVertex3f(n1.x, n1.y, n1.z);
    }
    glEnd();
    glPopMatrix();
}

//void MainViewer::drawOneCar(CarParticle *p_car)
//{
//    GLfloat* nodes_p = car_list_triangle_nodes[p_car->carmesh_id];
//    GLfloat* normals_p = car_list_triangle_normals[p_car->carmesh_id];
//    int num_face = car_list_face_number[p_car->carmesh_id];

//    glPushMatrix();
//    glTranslated(p_car->visual_position.x, p_car->visual_position.y, 0.0);
//    glRotatef(atan2(p_car->orientation.y, p_car->orientation.x)*180.0/PI, 0, 0, 1);

//    glColor4f(p_car->carcolor.x, p_car->carcolor.y, p_car->carcolor.z, p_car->carcolor.a);
//    glBegin(GL_TRIANGLES);
//    int count = 0, count_normal = 0;
//    for (int i=0; i<num_face; ++i)
//    {
//        GLfloat norm[3];

//        norm[0] = - normals_p[count_normal+0];
//        norm[1] = - normals_p[count_normal+1];
//        norm[2] = - normals_p[count_normal+2];

//        glNormal3fv(norm);
//        glVertex3fv(nodes_p+count);
//        glVertex3fv(nodes_p+count+6);
//        glVertex3fv(nodes_p+count+3);

//        count += 9;
//        count_normal += 3;
//    }
//    glEnd();
//    glPopMatrix();
//}

void MainViewer::drawOneCarRoundPoint(CarParticle *p_car, double dd)
{
    zVec2 pos = p_car->position;
    zVec4 col = p_car->carcolor;
    double hh = 0.1;
    glColor4f(col.x, col.y, col.z, col.a);
    glBegin(GL_TRIANGLE_FAN);
    int size = circle_node_number * 2;
    for (int i=0; i<size; i+=2)
    {
        GLfloat n0[3];
        n0[0] = circle_node_p[i] * dd + pos[0];
        n0[1] = circle_node_p[i+1] * dd + pos[1];
        n0[2] = hh;

        glNormal3f(0,0,1);
        glVertex3fv(n0);
    }
    glEnd();
}

void MainViewer::drawOneCarBbox(CarParticle *p_car)
{
    zVec2 bbox[4];
    p_car->getBbox(bbox);
    GLfloat bbox_h = 0.02;

    if (p_car->lanechanging_status == 1 || p_car->lanechanging_status == 2)
    {
        glColor4f(1, 0, 0, 1);
    }
    else if (p_car->lanechanging_status == 3)
    {
        glColor4f(0, 1, 0, 1);
    }
    else
    {
        glColor4f(1, 0, 1, 1);
    }

    glBegin(GL_LINE_LOOP);
    glVertex3f(bbox[0].x, bbox[0].y, bbox_h);
    glVertex3f(bbox[1].x, bbox[1].y, bbox_h);
    glVertex3f(bbox[2].x, bbox[2].y, bbox_h);
    glVertex3f(bbox[3].x, bbox[3].y, bbox_h);
    glEnd();
}

void MainViewer::drawOneCarTail(CarParticle *p_car)
{
//    double hh = p_car->height * 0.3;
    double hh = 0.0;

    // Set desired texture color.
    glTexCoord2f(0.5, p_car->x_velocity / traffic_system_p->model_parameter->MaxVelocity);

    glColor4f(1.0, 1.0, 1.0, 1.0);
    glBegin(GL_TRIANGLE_STRIP);
    for (int j=0; j<p_car->tail_length; ++j)
    {
        GLfloat n0[3], n1[3];

        n0[0] = p_car->tail_left[j].x;
        n0[1] = p_car->tail_left[j].y;
        n0[2] = hh;

        n1[0] = p_car->tail_right[j].x;
        n1[1] = p_car->tail_right[j].y;
        n1[2] = hh;

        glVertex3fv(n0);
        glVertex3fv(n1);
    }
    glEnd();
}

void MainViewer::drawOneCarHead(CarParticle *p_car)
{
//    double hh = p_car->height * 0.3;
    double hh = 0.0;

    glColor4f(0.0, 1.0, 0.0, 0.4);
    glBegin(GL_TRIANGLE_STRIP);
    for (int j=0; j<p_car->head_length; ++j)
    {
        GLfloat n0[3], n1[3];

        n0[0] = p_car->head_left[j].x;
        n0[1] = p_car->head_left[j].y;
        n0[2] = hh;

        n1[0] = p_car->head_right[j].x;
        n1[1] = p_car->head_right[j].y;
        n1[2] = hh;

        glVertex3fv(n0);
        glVertex3fv(n1);
    }
    glEnd();

    hh += 0.001;
    zVec4 col(0.0, 0.6, 0.5, 0.8);
    int size = p_car->head_visual_trajpoints.size();
    for (int j=0; j<size; ++j)
    {
        drawOneCircle(p_car->head_visual_trajpoints[j], 1.0, hh, col);
    }
}

//-----------------------------------------------------------------------------------
// Blocks
void MainViewer::drawBlockAreas()
{
    // Do not need lighting
    glDisable(GL_LIGHTING);

    int size = block_list.size();
    for (int i=0; i<size; ++i)
    {
        drawOneBlockArea(block_list[i], color_block);
    }

    glEnable(GL_LIGHTING);
}

void MainViewer::drawOneBlockArea(Cell &cc, zVec4 color)
{
    // draw the block cell
    double h_lane = traffic_system_p->model_parameter->ParticleRadius;
    glLineWidth(5.0);
    glColor4f(color.x, color.y, color.z, color.a);
    glBegin(GL_QUADS);
    {
        // Important! Must be double sided for selection.
        glNormal3f(0,0,1);
        glVertex3f(cc.v[0].x, cc.v[0].y, h_lane);
        glVertex3f(cc.v[1].x, cc.v[1].y, h_lane);
        glVertex3f(cc.v[2].x, cc.v[2].y, h_lane);
        glVertex3f(cc.v[3].x, cc.v[3].y, h_lane);

        glNormal3f(0,0,-1);
        glVertex3f(cc.v[0].x, cc.v[0].y, h_lane);
        glVertex3f(cc.v[3].x, cc.v[3].y, h_lane);
        glVertex3f(cc.v[2].x, cc.v[2].y, h_lane);
        glVertex3f(cc.v[1].x, cc.v[1].y, h_lane);
    }
    glEnd();

//    // draw the block barrier
//    TriangleMesh *p_mesh = &traffic_system_p->model_parameter->block_mesh;
//    glPushMatrix();
//    glTranslated(cc.center_point.x, cc.center_point.y, 0.0);
//    glRotatef(atan2(cc.center_tangent.y, cc.center_tangent.x)*180.0/PI, 0, 0, 1);

//    glColor4f(1,1,1,1);
//    glBegin(GL_TRIANGLES);
//    int num_face = p_mesh->triangle_list.size();
//    for (int i=0; i<num_face; ++i)
//    {
//        Triangle *p_tri = &p_mesh->triangle_list[i];
//        zVec3 norm = p_tri->norm;

//        // determine the new vertices
//        zVec3 n0 = p_tri->v0->original_position;
//        zVec3 n1 = p_tri->v1->original_position;
//        zVec3 n2 = p_tri->v2->original_position;

//        glNormal3f(norm.x, norm.y, norm.z);
//        glVertex3f(n0.x, n0.y, n0.z);
//        glVertex3f(n1.x, n1.y, n1.z);
//        glVertex3f(n2.x, n2.y, n2.z);

////        glNormal3f(-norm.x, -norm.y, -norm.z);
////        glVertex3f(n0.x, n0.y, n0.z);
////        glVertex3f(n2.x, n2.y, n2.z);
////        glVertex3f(n1.x, n1.y, n1.z);
//    }
//    glEnd();
//    glPopMatrix();
}

void MainViewer::drawOneBlockBarrier(Cell &cc)
{
    // draw the block barrier
    TriangleMesh *p_mesh = &traffic_system_p->model_parameter->block_mesh;
    glPushMatrix();
    glTranslated(cc.center_point.x, cc.center_point.y, 0.0);
    glRotatef(atan2(cc.center_tangent.y, cc.center_tangent.x)*180.0/PI, 0, 0, 1);

    glColor4f(1,1,1,1);
    glBegin(GL_TRIANGLES);
    int num_face = p_mesh->triangle_list.size();
    for (int i=0; i<num_face; ++i)
    {
        Triangle *p_tri = &p_mesh->triangle_list[i];
        zVec3 norm = p_tri->norm;

        // determine the new vertices
        zVec3 n0 = p_tri->v0->original_position;
        zVec3 n1 = p_tri->v1->original_position;
        zVec3 n2 = p_tri->v2->original_position;

        glNormal3f(norm.x, norm.y, norm.z);
        glVertex3f(n0.x, n0.y, n0.z);
        glVertex3f(n1.x, n1.y, n1.z);
        glVertex3f(n2.x, n2.y, n2.z);

//        glNormal3f(-norm.x, -norm.y, -norm.z);
//        glVertex3f(n0.x, n0.y, n0.z);
//        glVertex3f(n2.x, n2.y, n2.z);
//        glVertex3f(n1.x, n1.y, n1.z);
    }
    glEnd();
    glPopMatrix();
}

//-----------------------------------------------------------------------------------
// Splines
void MainViewer::drawBezierSpline()
{
    // Do not need lighting
    glDisable(GL_LIGHTING);

    double hh_1 = visual_xy_plane_height * 1.2;
    double hh_2 = visual_xy_plane_height * 1.5;
    int num_curve = road_curves.size();
    for (int k=0; k<num_curve; ++k)
    {
        BezierSequence *p_bezs = road_curves[k];

        int size = p_bezs->beziers.size();

        // draw the road shape using the smooth points
        glLineWidth(5.0);
        glColor4f(color_bezier.x, color_bezier.y, color_bezier.z, color_bezier.a);
        for (int i=0; i<size; ++i)
        {
            Bezier *p_bez = &p_bezs->beziers[i];

            glBegin(GL_LINE_STRIP);
            int size_points = p_bez->sequ_point.size();
            for (int j=0; j<size_points; ++j)
            {
                glNormal3f(0,0,1);
                glVertex3f(p_bez->sequ_point[j].x, p_bez->sequ_point[j].y, hh_1);
            }
            glEnd();
        }


        // draw the control points and connecting lines
        glLineWidth(2.0);
        drawOneQuad(p_bezs->beziers[0].control_point[0], color_bezier_selected);
        for (int i=0; i<size; ++i)
        {
            Bezier *p_bez = &p_bezs->beziers[i];
            for (int k=1; k<4; ++k)
            {
                drawOneQuad(p_bez->control_point[k], color_bezier_selected);
            }

            // connect the control points
            glBegin(GL_LINES);
            glNormal3f(0,0,1);
            glVertex3f(p_bez->control_point[0].x, p_bez->control_point[0].y, hh_2);
            glVertex3f(p_bez->control_point[1].x, p_bez->control_point[1].y, hh_2);

            glNormal3f(0,0,1);
            glVertex3f(p_bez->control_point[2].x, p_bez->control_point[2].y, hh_2);
            glVertex3f(p_bez->control_point[3].x, p_bez->control_point[3].y, hh_2);
            glEnd();
        }
    }

    glEnable(GL_LIGHTING);
}

//============================================================================
// Associatie Functions
//============================================================================
void MainViewer::updateSplines()
{
    int num_road = road_curves.size();
    for (int i=0; i<num_road; ++i)
    {
        BezierSequence *p_road_curve = road_curves[i];

        if (p_road_curve->hasModifiedBezier)
        {
            p_road_curve->buildBezierSequence();
            traffic_system_p->road_list[i]->initRoad();
        }
    }
}

void MainViewer::restoreSplines()
{
    int num_road = road_curves.size();
    for (int i=0; i<num_road; ++i)
    {
        BezierSequence *p_road_curve = road_curves[i];
        if (p_road_curve->hasModifiedBezier)
        {
            int num_bez = p_road_curve->beziers.size();
            for (int j=0; j<num_bez; ++j)
            {
                p_road_curve->beziers[j].restoreControlPoints();
                p_road_curve->beziers[j].init();
            }
            p_road_curve->hasModifiedBezier = false;
        }
    }
}

void MainViewer::backupSplines()
{
    int num_road = road_curves.size();
    for (int i=0; i<num_road; ++i)
    {
        BezierSequence *p_road_curve = road_curves[i];
        int num_bez = p_road_curve->beziers.size();
        for (int j=0; j<num_bez; ++j)
        {
            p_road_curve->beziers[j].backupControlPoints();
        }
    }
}

void MainViewer::addNewBlockArea()
{
    // the block is a non-symmetric rectangle
    double block_halflen_x0 = DEFAULT_LANE_WIDTH / 3;
    double block_halflen_x1 = DEFAULT_LANE_WIDTH * 2.0;
    double block_halflen_y = DEFAULT_LANE_WIDTH * 1.3 / 2;

    zVec2 p0,p1,p2,p3;
    p0.x = - block_halflen_x0;
    p0.y =   block_halflen_y;
    p1.x = - block_halflen_x0;
    p1.y = - block_halflen_y;
    p2.x =   block_halflen_x1;
    p2.y = - block_halflen_y;
    p3.x =   block_halflen_x1;
    p3.y =   block_halflen_y;

    Cell cc(p0,p1,p2,p3);
    cc.setCenterToPoint(zVec2(0,0));
    cc.align(traffic_system_p->getTangentFromRoadNet(cc.center_point));
    block_list.push_back(cc);

    updateGL();
}

void MainViewer::updateBlockArea()
{
    traffic_system_p->block_list = block_list;
}

void MainViewer::restoreBlockArea()
{
    block_list = traffic_system_p->block_list;
}

bool MainViewer::isNearCamera()
{
    //double cx = camera()->position().x;
    double cy = camera()->position().y;
    double cz = camera()->position().z;

    if (cy*cy + cz*cz <= 1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void MainViewer::updateScreenShot()
{
    if (screenshot)
    {
        delete screenshot;
    }

    screenshot = new unsigned char[4 * width() * height()];
    glReadPixels(0, 0, width(), height(), GL_RGBA, GL_UNSIGNED_BYTE, screenshot);
}

void MainViewer::saveCamera()
{
    camera()->getModelViewMatrix(matrix_modelview);
    camera()->getProjectionMatrix(matrix_projection);
}

void MainViewer::restoreCamera()
{
    camera()->setFromModelViewMatrix(matrix_modelview);
//        camera()->setFieldOfView(2.0 * atan(1.0/matrix_projection[5]));
    updateGL();
}

