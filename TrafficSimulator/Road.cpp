#include "Road.h"

//============================================================================
// Constructor
//============================================================================
Road::Road(SystemParameter *p_SysParam_, RoadLink *p_road_link_, int road_idx)
{
    p_SysParam = p_SysParam_;
    p_road_link = p_road_link_;
    index = road_idx;

    initRoadAttributes();

    initRoadGeometry();

    initRoadSimulation();

    initRoad();
}

void Road::initRoadAttributes()
{
    id = p_road_link->id;
    roadtype = p_road_link->link_type;
    roadsubtype = p_road_link->highway_type;
    num_lanes = p_road_link->lanes;
    isOneway = p_road_link->isOneway;

    speed_limit = 30;  //meter per second
    speed_constrain = CONSTRAIN_SPEED;
    isVisible = true;
    isConstrained = false;

    isInflow[0] = p_road_link->InflowSwitch[0];
    isInflow[1] = p_road_link->InflowSwitch[1];
    inflow_rate[0] = p_road_link->InflowRate[0];
    inflow_rate[1] = p_road_link->InflowRate[1];

    if (id != 0)
    {
        isInflow[0] = false;
        isInflow[1] = false;
    }
}

void Road::initRoadGeometry()
{
    road_curve.beziers.clear();
    vector<RoadNode*> roadnode_list = p_road_link->roadnode_list;
    int num_point = roadnode_list.size();

    if (p_road_link->link_type != beziers)
    {
        // convert polylines to beziers
        double tangent_factor = 0.3;
        if (num_point == 2)
        {
            // only one straight line
            RoadNode *p_roadnode_0 = roadnode_list[0];
            RoadNode *p_roadnode_1 = roadnode_list[1];

            zVec2 p0 = p_roadnode_0->position;
            zVec2 p1 = p_roadnode_1->position - p_roadnode_0->position; // vector
            zVec2 p2 = -p1; // vector
            zVec2 p3 = p_roadnode_1->position;

            double len = (p3-p0).length();
            p1 = p0 + p1 * (len * tangent_factor / p1.length());
            p2 = p3 + p2 * (len * tangent_factor / p2.length());
            road_curve.beziers.push_back(Bezier(p0,p1,p2,p3));
        }
        else if (num_point > 2)
        {
            // the first line
            RoadNode *p_roadnode_0 = roadnode_list[0];
            RoadNode *p_roadnode_1 = roadnode_list[0];
            RoadNode *p_roadnode_2 = roadnode_list[1];
            RoadNode *p_roadnode_3 = roadnode_list[2];

            zVec2 p0 = p_roadnode_1->position;
            zVec2 p1 = p_roadnode_2->position - p_roadnode_0->position; // vector
            zVec2 p2 = p_roadnode_1->position - p_roadnode_3->position; // vector
            zVec2 p3 = p_roadnode_2->position;
            double len = (p3-p0).length();
            p1 = p0 + p1 * (len * tangent_factor / p1.length());
            p2 = p3 + p2 * (len * tangent_factor / p2.length());
            road_curve.beziers.push_back(Bezier(p0,p1,p2,p3));

            // the middle lines
            for (int j=1; j<num_point-2; ++j)
            {
                p_roadnode_0 = roadnode_list[j-1];
                p_roadnode_1 = roadnode_list[j];
                p_roadnode_2 = roadnode_list[j+1];
                p_roadnode_3 = roadnode_list[j+2];

                p0 = p_roadnode_1->position;
                p1 = p_roadnode_2->position - p_roadnode_0->position; // vector
                p2 = p_roadnode_1->position - p_roadnode_3->position; // vector
                p3 = p_roadnode_2->position;
                len = (p3-p0).length();
                p1 = p0 + p1 * (len * tangent_factor / p1.length());
                p2 = p3 + p2 * (len * tangent_factor / p2.length());
                road_curve.beziers.push_back(Bezier(p0,p1,p2,p3));
            }

            // the last line
            int last_id = num_point-1;
            p_roadnode_0 = roadnode_list[last_id-2];
            p_roadnode_1 = roadnode_list[last_id-1];
            p_roadnode_2 = roadnode_list[last_id];
            p_roadnode_3 = roadnode_list[last_id];

            p0 = p_roadnode_1->position;
            p1 = p_roadnode_2->position - p_roadnode_0->position; // vector
            p2 = p_roadnode_1->position - p_roadnode_3->position; // vector
            p3 = p_roadnode_2->position;
            len = (p3-p0).length();
            p1 = p0 + p1 * (len * tangent_factor / p1.length());
            p2 = p3 + p2 * (len * tangent_factor / p2.length());
            road_curve.beziers.push_back(Bezier(p0,p1,p2,p3));
        }
    }
    else
    {
        for (int i=0; i<num_point; i+=4)
        {
            RoadNode *p_roadnode_0 = roadnode_list[i];
            RoadNode *p_roadnode_1 = roadnode_list[i+1];
            RoadNode *p_roadnode_2 = roadnode_list[i+2];
            RoadNode *p_roadnode_3 = roadnode_list[i+3];

            zVec2 p0, p1, p2, p3;
            p0 = p_roadnode_0->position;
            p1 = p_roadnode_1->position;
            p2 = p_roadnode_2->position;
            p3 = p_roadnode_3->position;

            road_curve.beziers.push_back(Bezier(p0,p1,p2,p3));
        }
    }

    road_curve.buildBezierSequence();
}

void Road::initRoadSimulation()
{
    particle_lists.resize(num_lanes);
    cleanParticles();

    car_lists.resize(num_lanes);
    cleanCars();

    lane_direction.resize(num_lanes);
    production_particle.resize(num_lanes);
    production_car.resize(num_lanes);
    for (int i=0; i<num_lanes; ++i)
    {
        lane_direction[i] = 0; // default direction
        production_particle[i] = 0;
        production_car[i] = 0;
    }

    // determine the those lane directions if this is a two-way road.
    if (!isOneway)
    {
        int half_num_lanes = num_lanes / 2;
        if (half_num_lanes >= 1)
        {
            for (int i=0; i<half_num_lanes; ++i)
            {
                lane_direction[i] = 1; // reverse direction
            }
        }
    }
}

void Road::initRoad()
{
    // determine the all length and width.
    lane_width = DEFAULT_LANE_WIDTH;
    road_width = lane_width * double(num_lanes);

    road_length = road_curve.length;
    cell_length = road_length / ceil(road_length / DEFAULT_LANE_CELL_LENGTH);

    // determine the bias distances of the lanes with respect to the road.
    bias_distances.resize(num_lanes);
    double start_lane = lane_width * double(num_lanes-1) * 0.5;
    for (int i=0; i<num_lanes; ++i)
    {
        bias_distances[i] = start_lane;
        start_lane -= lane_width;
    }

    // initialize the hash tables
    hash_tables.clear();
    for (int k=0; k<num_lanes; ++k)
    {
        HashTableX *hash_table_x = new HashTableX(road_length, p_SysParam->SupportRadius);
        hash_tables.push_back(hash_table_x);
    }

    buildVisualRoadAndLanes();
//    buildDrain();
    buildCells();

    buildLaneMeshes();

    createSensors();
}

//========================================================================================
void Road::buildVisualRoadAndLanes()
{
    //---------------------------------------------------------------------------------
    // Sample the points/normals of the bezier sequence.
    int num_samples = ceil(road_length / (cell_length*0.3));
    if (num_samples < 2)
    {
        num_samples = 2;
    }
    double step_arc_length = road_length / double(num_samples-1);

    vector<zVec2> sample_points(num_samples);
    vector<zVec2> sample_normals(num_samples);

    double arc_acc = 0.0;
    for (int i=0; i<num_samples-1; ++i)
    {
        sample_points[i] = road_curve.getPointByXPosition(arc_acc);
        sample_normals[i] = road_curve.getNormalByXPosition(arc_acc);

        arc_acc += step_arc_length;
    }
    sample_points[num_samples-1] = road_curve.getPointByXPosition(road_length);
    sample_normals[num_samples-1] = road_curve.getNormalByXPosition(road_length);

    //---------------------------------------------------------------------------------
    // Build two sides and solid lanes of the road.
    visualroad_side_left.clear();
    visualroad_side_right.clear();
    visuallane_solid.clear();

    visualroad_side_left.resize(num_samples);
    visualroad_side_right.resize(num_samples);
    visuallane_solid.resize(num_lanes+1);
    for (int k=0; k<=num_lanes; ++k)
    {
        visuallane_solid[k].clear();
        visuallane_solid[k].resize(num_samples);
    }

    // Generate points.
    for (int i=0; i<num_samples; ++i)
    {
        zVec2 vn = sample_normals[i] * (lane_width * double(num_lanes+1));
        visualroad_side_left[i] = sample_points[i] + vn * 0.5;
        visualroad_side_right[i] = sample_points[i] - vn * 0.5;

        zVec2 vm = sample_points[i] + sample_normals[i] * (lane_width * double(num_lanes) * 0.5);
        zVec2 vm_step = sample_normals[i] * lane_width;
        for (int k=0; k<=num_lanes; ++k)
        {
            PointArray *p_pa = &visuallane_solid[k];
            p_pa->at(i) = vm;
            vm -= vm_step;
        }
    }

    //---------------------------------------------------------------------------------
    // Re-Sample the points/normals of the bezier sequence.
    num_samples = ceil(road_length / (cell_length*1.0));
    if (num_samples < 2)
    {
        num_samples = 2;
    }
    step_arc_length = road_length / double(num_samples-1);
    double dash_len = step_arc_length * 0.7;
    double dash_len_gap = step_arc_length - dash_len;

    num_samples = 2*num_samples-1;
    sample_points.resize(num_samples);
    sample_normals.resize(num_samples);

    arc_acc = 0.0;
    for (int i=0; i<num_samples-1; i+=2)
    {
        sample_points[i] = road_curve.getPointByXPosition(arc_acc);
        sample_normals[i] = road_curve.getNormalByXPosition(arc_acc);
        arc_acc += dash_len;

        sample_points[i+1] = road_curve.getPointByXPosition(arc_acc);
        sample_normals[i+1] = road_curve.getNormalByXPosition(arc_acc);
        arc_acc += dash_len_gap;
    }
    sample_points[num_samples-1] = road_curve.getPointByXPosition(road_length);
    sample_normals[num_samples-1] = road_curve.getNormalByXPosition(road_length);

    //---------------------------------------------------------------------------------
    // Build dash lanes of the road.
    visuallane_dash.clear();
    visuallane_dash.resize(num_lanes+1);
    for (int k=0; k<=num_lanes; ++k)
    {
        visuallane_dash[k].clear();
        visuallane_dash[k].resize(num_samples);
    }

    // Generate points.
    for (int i=0; i<num_samples; ++i)
    {
        zVec2 vm = sample_points[i] + sample_normals[i] * (lane_width * double(num_lanes) * 0.5);
        zVec2 vm_step = sample_normals[i] * lane_width;
        for (int k=0; k<=num_lanes; ++k)
        {
            PointArray *p_pa = &visuallane_dash[k];
            p_pa->at(i) = vm;
            vm -= vm_step;
        }
    }
}

void Road::buildDrain()
{
    double drain_height = lane_width * 1.2;
    double drain_width = lane_width * 0.6;

    //----------------------------------------------
    // build the node list of the drain
    //----------------------------------------------
    // drain 1
    drain_1.node_list.clear();
    int node_count = 0;
    int size = visualroad_side_left.size();
    for (int i=0; i<size; ++i)
    {
        zVec2 v1 = visualroad_side_left[i] - visualroad_side_right[i];
        v1 = v1 * (drain_width / v1.length());

        Node *p_node = new Node;
        p_node->id = node_count++;
        p_node->position.x = visualroad_side_left[i].x;
        p_node->position.y = visualroad_side_left[i].y;
        p_node->position.z = 0;//- drain_height;
        drain_1.node_list.push_back(p_node);

        p_node->id = node_count++;
        p_node->position.z = drain_height * 2;
        drain_1.node_list.push_back(p_node);

        p_node->id = node_count++;
        p_node->position.x += v1.x;
        p_node->position.y += v1.y;
        drain_1.node_list.push_back(p_node);

        p_node->id = node_count++;
        p_node->position.z = 0;//- drain_height;
        drain_1.node_list.push_back(p_node);
    }

//    // drain 2
//    drain_2.node_list.clear();
//    node_count = 0;
//    for (int i=0; i<size; ++i)
//    {
//        zVec2 v1 = visualroad_side_right[i] - visualroad_side_left[i];
//        v1 = v1 * (drain_width / v1.length());

//        Node p;
//        p.index = node_count++;
//        p.position.x = visualroad_side_right[i].x;
//        p.position.y = visualroad_side_right[i].y;
//        p.position.z = 0;//- drain_height;
//        drain_2.node_list.push_back(p);

//        p.index = node_count++;
//        p.position.z = drain_height * 2;
//        drain_2.node_list.push_back(p);

//        p.index = node_count++;
//        p.position.x += v1.x;
//        p.position.y += v1.y;
//        drain_2.node_list.push_back(p);

//        p.index = node_count++;
//        p.position.z = 0;//- drain_height;
//        drain_2.node_list.push_back(p);
//    }

//    //----------------------------------------------
//    // build the triangle list of the drain
//    //----------------------------------------------
//    // drain 1
//    drain_1.triangle_list.clear();
//    int triangle_count = 0;
//    size = drain_1.node_list.size() / 4 - 1;
//    for (int i=0; i<size; ++i)
//    {
//        Node *p0 = &drain_1.node_list[i*4+0];
//        Node *p1 = &drain_1.node_list[i*4+1];
//        Node *p2 = &drain_1.node_list[i*4+2];
//        Node *p3 = &drain_1.node_list[i*4+3];

//        Node *p4 = &drain_1.node_list[i*4+4];
//        Node *p5 = &drain_1.node_list[i*4+5];
//        Node *p6 = &drain_1.node_list[i*4+6];
//        Node *p7 = &drain_1.node_list[i*4+7];

//        Triangle tri;

//        tri.index = triangle_count++;
//        tri.v0 = p0;
//        tri.v1 = p5;
//        tri.v2 = p1;
//        tri.norm = zVec3::normalOfTriangle(tri.v0->position, tri.v1->position, tri.v2->position);
//        drain_1.triangle_list.push_back(tri);
//        tri.index = triangle_count++;
//        tri.v0 = p0;
//        tri.v1 = p4;
//        tri.v2 = p5;
//        tri.norm = zVec3::normalOfTriangle(tri.v0->position, tri.v1->position, tri.v2->position);
//        drain_1.triangle_list.push_back(tri);

//        tri.index = triangle_count++;
//        tri.v0 = p1;
//        tri.v1 = p5;
//        tri.v2 = p6;
//        tri.norm = zVec3::normalOfTriangle(tri.v0->position, tri.v1->position, tri.v2->position);
//        drain_1.triangle_list.push_back(tri);
//        tri.index = triangle_count++;
//        tri.v0 = p1;
//        tri.v1 = p6;
//        tri.v2 = p2;
//        tri.norm = zVec3::normalOfTriangle(tri.v0->position, tri.v1->position, tri.v2->position);
//        drain_1.triangle_list.push_back(tri);

//        tri.index = triangle_count++;
//        tri.v0 = p2;
//        tri.v1 = p6;
//        tri.v2 = p7;
//        tri.norm = zVec3::normalOfTriangle(tri.v0->position, tri.v1->position, tri.v2->position);
//        drain_1.triangle_list.push_back(tri);
//        tri.index = triangle_count++;
//        tri.v0 = p2;
//        tri.v1 = p7;
//        tri.v2 = p3;
//        tri.norm = zVec3::normalOfTriangle(tri.v0->position, tri.v1->position, tri.v2->position);
//        drain_1.triangle_list.push_back(tri);

//        tri.index = triangle_count++;
//        tri.v0 = p3;
//        tri.v1 = p7;
//        tri.v2 = p4;
//        tri.norm = zVec3::normalOfTriangle(tri.v0->position, tri.v1->position, tri.v2->position);
//        drain_1.triangle_list.push_back(tri);
//        tri.index = triangle_count++;
//        tri.v0 = p3;
//        tri.v1 = p4;
//        tri.v2 = p0;
//        tri.norm = zVec3::normalOfTriangle(tri.v0->position, tri.v1->position, tri.v2->position);
//        drain_1.triangle_list.push_back(tri);
//    }

//    // the end faces
//    Node *p0 = &drain_1.node_list[0];
//    Node *p1 = &drain_1.node_list[1];
//    Node *p2 = &drain_1.node_list[2];
//    Node *p3 = &drain_1.node_list[3];

//    Triangle tri;
//    tri.index = triangle_count++;
//    tri.v0 = p0;
//    tri.v1 = p1;
//    tri.v2 = p2;
//    tri.norm = zVec3::normalOfTriangle(tri.v0->position, tri.v1->position, tri.v2->position);
//    drain_1.triangle_list.push_back(tri);
//    tri.index = triangle_count++;
//    tri.v0 = p0;
//    tri.v1 = p2;
//    tri.v2 = p3;
//    tri.norm = zVec3::normalOfTriangle(tri.v0->position, tri.v1->position, tri.v2->position);
//    drain_1.triangle_list.push_back(tri);

//    p0 = &drain_1.node_list[size*4 + 0];
//    p1 = &drain_1.node_list[size*4 + 1];
//    p2 = &drain_1.node_list[size*4 + 2];
//    p3 = &drain_1.node_list[size*4 + 3];

//    tri.index = triangle_count++;
//    tri.v0 = p0;
//    tri.v1 = p2;
//    tri.v2 = p1;
//    tri.norm = zVec3::normalOfTriangle(tri.v0->position, tri.v1->position, tri.v2->position);
//    drain_1.triangle_list.push_back(tri);
//    tri.index = triangle_count++;
//    tri.v0 = p0;
//    tri.v1 = p3;
//    tri.v2 = p2;
//    tri.norm = zVec3::normalOfTriangle(tri.v0->position, tri.v1->position, tri.v2->position);
//    drain_1.triangle_list.push_back(tri);

//    // drain 2
//    drain_2.triangle_list.clear();
//    triangle_count = 0;
//    size = drain_2.node_list.size() / 4 - 1;
//    for (int i=0; i<size; ++i)
//    {
//        Node *p0 = &drain_2.node_list[i*4+0];
//        Node *p1 = &drain_2.node_list[i*4+1];
//        Node *p2 = &drain_2.node_list[i*4+2];
//        Node *p3 = &drain_2.node_list[i*4+3];

//        Node *p4 = &drain_2.node_list[i*4+4];
//        Node *p5 = &drain_2.node_list[i*4+5];
//        Node *p6 = &drain_2.node_list[i*4+6];
//        Node *p7 = &drain_2.node_list[i*4+7];

//        Triangle tri;

//        tri.index = triangle_count++;
//        tri.v0 = p0;
//        tri.v1 = p1;
//        tri.v2 = p5;
//        tri.norm = zVec3::normalOfTriangle(tri.v0->position, tri.v1->position, tri.v2->position);
//        drain_2.triangle_list.push_back(tri);
//        tri.index = triangle_count++;
//        tri.v0 = p0;
//        tri.v1 = p5;
//        tri.v2 = p4;
//        tri.norm = zVec3::normalOfTriangle(tri.v0->position, tri.v1->position, tri.v2->position);
//        drain_2.triangle_list.push_back(tri);

//        tri.index = triangle_count++;
//        tri.v0 = p1;
//        tri.v1 = p6;
//        tri.v2 = p5;
//        tri.norm = zVec3::normalOfTriangle(tri.v0->position, tri.v1->position, tri.v2->position);
//        drain_2.triangle_list.push_back(tri);
//        tri.index = triangle_count++;
//        tri.v0 = p1;
//        tri.v1 = p2;
//        tri.v2 = p6;
//        tri.norm = zVec3::normalOfTriangle(tri.v0->position, tri.v1->position, tri.v2->position);
//        drain_2.triangle_list.push_back(tri);

//        tri.index = triangle_count++;
//        tri.v0 = p2;
//        tri.v1 = p7;
//        tri.v2 = p6;
//        tri.norm = zVec3::normalOfTriangle(tri.v0->position, tri.v1->position, tri.v2->position);
//        drain_2.triangle_list.push_back(tri);
//        tri.index = triangle_count++;
//        tri.v0 = p2;
//        tri.v1 = p3;
//        tri.v2 = p7;
//        tri.norm = zVec3::normalOfTriangle(tri.v0->position, tri.v1->position, tri.v2->position);
//        drain_2.triangle_list.push_back(tri);

//        tri.index = triangle_count++;
//        tri.v0 = p3;
//        tri.v1 = p4;
//        tri.v2 = p7;
//        tri.norm = zVec3::normalOfTriangle(tri.v0->position, tri.v1->position, tri.v2->position);
//        drain_2.triangle_list.push_back(tri);
//        tri.index = triangle_count++;
//        tri.v0 = p3;
//        tri.v1 = p0;
//        tri.v2 = p4;
//        tri.norm = zVec3::normalOfTriangle(tri.v0->position, tri.v1->position, tri.v2->position);
//        drain_2.triangle_list.push_back(tri);
//    }

//    // the end faces
//    p0 = &drain_2.node_list[0];
//    p1 = &drain_2.node_list[1];
//    p2 = &drain_2.node_list[2];
//    p3 = &drain_2.node_list[3];

//    tri.index = triangle_count++;
//    tri.v0 = p0;
//    tri.v1 = p2;
//    tri.v2 = p1;
//    tri.norm = zVec3::normalOfTriangle(tri.v0->position, tri.v1->position, tri.v2->position);
//    drain_2.triangle_list.push_back(tri);
//    tri.index = triangle_count++;
//    tri.v0 = p0;
//    tri.v1 = p3;
//    tri.v2 = p2;
//    tri.norm = zVec3::normalOfTriangle(tri.v0->position, tri.v1->position, tri.v2->position);
//    drain_2.triangle_list.push_back(tri);

//    p0 = &drain_2.node_list[size*4 + 0];
//    p1 = &drain_2.node_list[size*4 + 1];
//    p2 = &drain_2.node_list[size*4 + 2];
//    p3 = &drain_2.node_list[size*4 + 3];

//    tri.index = triangle_count++;
//    tri.v0 = p0;
//    tri.v1 = p1;
//    tri.v2 = p2;
//    tri.norm = zVec3::normalOfTriangle(tri.v0->position, tri.v1->position, tri.v2->position);
//    drain_2.triangle_list.push_back(tri);
//    tri.index = triangle_count++;
//    tri.v0 = p0;
//    tri.v1 = p2;
//    tri.v2 = p3;
//    tri.norm = zVec3::normalOfTriangle(tri.v0->position, tri.v1->position, tri.v2->position);
//    drain_2.triangle_list.push_back(tri);
}

void Road::buildBoundary()
{
    vector<Cell*> bound_cells;
    vector<Cell*> road_cells;

    bound_cells.clear();
    road_cells.clear();

    double step_arc_length = cell_length * 0.5;
    double road_area_halfwidth = 0.5 * lane_width * double(num_lanes);
    double bound_area_halfwidth = road_area_halfwidth + 0.5 * p_SysParam->SupportRadius;

    // The first cell of the bound_area is before the first point in the curve
    zVec2 v0, v1, v0_n, v1_n;
    v1 = road_curve.getPointByXPosition(0.0);
    v1_n = road_curve.getNormalByXPosition(0.0);
    v0 = v1 - road_curve.getTangentByXPosition(0.0) * step_arc_length;
    v0_n = v1_n;

    Cell *p_cell = new Cell(v0 + v0_n*bound_area_halfwidth, v0 - v0_n*bound_area_halfwidth,
                            v1 - v1_n*bound_area_halfwidth, v1 + v1_n*bound_area_halfwidth);
    p_cell->x_position = 0;
    bound_cells.push_back(p_cell);

    // the rest cells of the bound_area and all cells of road_area
    double arc_acc = step_arc_length;
    while (arc_acc <= road_length)
    {
        v0 = v1;
        v0_n = v1_n;

        v1 = road_curve.getPointByXPosition(arc_acc);
        v1_n = road_curve.getNormalByXPosition(arc_acc);

        // the cell of the bound_area
        p_cell = new Cell(v0 + v0_n*bound_area_halfwidth, v0 - v0_n*bound_area_halfwidth,
                          v1 - v1_n*bound_area_halfwidth, v1 + v1_n*bound_area_halfwidth);
        p_cell->x_position = arc_acc;
        bound_cells.push_back(p_cell);

//        // the cell of the road_area
//        p_cell = new Cell(v0 + v0_n*road_area_halfwidth, v0 - v0_n*road_area_halfwidth,
//                          v1 - v1_n*road_area_halfwidth, v1 + v1_n*road_area_halfwidth);
//        p_cell->x_position = arc_acc - step_arc_length;
//        road_cells.push_back(p_cell);

        arc_acc += step_arc_length;
    }

//    if (bound_cells.size() == 0 || road_cells.size() == 0)
//    {
//        cout << "ERROR! cells number == 0.\n";
//        return;
//    }

    // the last cell
    v0 = v1;
    v0_n = v1_n;
    v1 = road_curve.getPointByXPosition(road_length);
    v1_n = road_curve.getNormalByXPosition(road_length);
    if (fabs(arc_acc-road_length) > step_arc_length * 0.5)
    {
        // the residual is big, which means the last point is close to t=1.0
        // adjust the p1 and p2 of the last cell of the bound_area to t=1.0
        p_cell = bound_cells[bound_cells.size() - 1];
        p_cell->v[3] = v1 + v1_n*bound_area_halfwidth;
        p_cell->v[2] = v1 - v1_n*bound_area_halfwidth;

//        // adjust the p1 and p2 of the last cell of the road_area
//        p_cell = road_cells[road_cells.size() - 1];
//        p_cell->v[3] = v1 + v1_n*road_area_halfwidth;
//        p_cell->v[2] = v1 - v1_n*road_area_halfwidth;
    }
    else
    {
        // the residual is small, which means the last point t=1.0 is missed
        // add the last piont t=1.0
        // the cell of the bound_area
        p_cell = new Cell(v0 + v0_n*bound_area_halfwidth, v0 - v0_n*bound_area_halfwidth,
                          v1 - v1_n*bound_area_halfwidth, v1 + v1_n*bound_area_halfwidth);
        p_cell->x_position = arc_acc;
        bound_cells.push_back(p_cell);

//        // the cell of the road_area
//        p_cell = new Cell(v0 + v0_n*road_area_halfwidth, v0 - v0_n*road_area_halfwidth,
//                          v1 - v1_n*road_area_halfwidth, v1 + v1_n*road_area_halfwidth);
//        p_cell->x_position = arc_acc - step_arc_length;
//        road_cells.push_back(p_cell);
    }

    // The very last cell of the bound area
    v0 = v1;
    v0_n = v1_n;
    v1 = v0 + road_curve.getTangentByXPosition(road_length) * step_arc_length;
    p_cell = new Cell(v0 + v0_n*bound_area_halfwidth, v0 - v0_n*bound_area_halfwidth,
                      v1 - v1_n*bound_area_halfwidth, v1 + v1_n*bound_area_halfwidth);
    p_cell->x_position = road_length;
    bound_cells.push_back(p_cell);

    // initialize the boundary and road area by the generated cells.
//    bound_area.init(bound_cells, id, road_length+step_arc_length*2.0);
//    road_area.init(road_cells, id, road_length);

//    cout << "-------------------------------\nBound_area " << id
//         << " Cells number " << bound_cells.size() << " bbox "
//         << bound_area.min_x << " " << bound_area.max_x << " ; "
//         << bound_area.min_y << " " << bound_area.max_y << endl;
//    cout << "Road_area " << id << " Cells number " << road_cells.size() << " bbox "
//         << road_area.min_x << " " << road_area.max_x << " ; "
//         << road_area.min_y << " " << road_area.max_y << endl;
//    if (id == 39)
//    {
//        for (int i=0; i<bound_cells.size(); ++i)
//        {
//            cout << "Bound_area " << id
//                 << bound_cells[i]->v[0] << bound_cells[i]->v[1]
//                 << bound_cells[i]->v[2] << bound_cells[i]->v[3] << endl;
//        }
//        cout << "Curve 0.0 : " << road_curve.getPointByXPosition(0.0)
//             << road_curve.getNormalByXPosition(0.0)
//             << road_curve.getTangentByXPosition(0.0) << endl;
//        cout << "Curve 1.0 : " << road_curve.getPointByXPosition(1.0)
//             << road_curve.getNormalByXPosition(1.0)
//             << road_curve.getTangentByXPosition(1.0) << endl;
//    }
}

void Road::buildCells()
{
    // determine the real desired cell length and the number of cells.
//    int num_cells = ceil(road_length / (cell_length*0.5));
    int num_cells = ceil(road_length / cell_length);
    if (num_cells < 1)
    {
        num_cells = 1;
    }
    double step_arc_length = road_length / double(num_cells);

    // build road cells
    double road_halfwidth = 0.5 * lane_width * double(num_lanes);
    road_area.clearCells();

    Cell *p_cell;
    zVec2 v0, v1, v2, v3;
    zVec2 vm, vm_normal, vm_tangent;

    road_curve.getPointNormalTangentByXPosition(vm, vm_normal, vm_tangent, 0.0);
    v3 = vm + vm_normal * road_halfwidth;
    v2 = vm - vm_normal * road_halfwidth;

    double arc_acc = 0.0;
    for (int i=0; i<num_cells; ++i)
    {
        v0 = v3;
        v1 = v2;

        arc_acc += step_arc_length;
        road_curve.getPointNormalTangentByXPosition(vm, vm_normal, vm_tangent, arc_acc);
        v3 = vm + vm_normal * road_halfwidth;
        v2 = vm - vm_normal * road_halfwidth;

        p_cell = new Cell(v0,v1,v2,v3);
        road_area.cells.push_back(p_cell);
    }

    // adjust v2 and v3 of the last cell
    road_curve.getPointNormalTangentByXPosition(vm, vm_normal, vm_tangent, road_length);
    p_cell = road_area.cells[num_cells-1];
    p_cell->v[3] = vm + vm_normal * road_halfwidth;
    p_cell->v[2] = vm - vm_normal * road_halfwidth;

    road_area.init(id);

    // build boundary cells
    double bound_halfwidth = 0.5 * lane_width * double(num_lanes+2);
    bound_area.clearCells();

    // the first bound cell
    road_curve.getPointNormalTangentByXPosition(vm, vm_normal, vm_tangent, 0.0);
    v3 = vm + vm_normal * bound_halfwidth;
    v2 = vm - vm_normal * bound_halfwidth;
    v0 = v3 - vm_tangent * step_arc_length;
    v1 = v2 - vm_tangent * step_arc_length;
    p_cell = new Cell(v0,v1,v2,v3);
    bound_area.cells.push_back(p_cell);

    arc_acc = 0.0;
    for (int i=0; i<num_cells; ++i)
    {
        v0 = v3;
        v1 = v2;

        arc_acc += step_arc_length;
        road_curve.getPointNormalTangentByXPosition(vm, vm_normal, vm_tangent, arc_acc);
        v3 = vm + vm_normal * bound_halfwidth;
        v2 = vm - vm_normal * bound_halfwidth;

        p_cell = new Cell(v0,v1,v2,v3);
        bound_area.cells.push_back(p_cell);
    }

    // adjust v2 and v3 of the last second cell
    road_curve.getPointNormalTangentByXPosition(vm, vm_normal, vm_tangent, road_length);
    v3 = vm + vm_normal * bound_halfwidth;
    v2 = vm - vm_normal * bound_halfwidth;
    p_cell = bound_area.cells[num_cells];
    p_cell->v[3] = v3;
    p_cell->v[2] = v2;

    // the last cell
    v0 = v3;
    v1 = v2;
    v3 = v0 + vm_tangent * step_arc_length;
    v2 = v1 + vm_tangent * step_arc_length;
    p_cell = new Cell(v0,v1,v2,v3);
    bound_area.cells.push_back(p_cell);

    bound_area.init(id);
}

void Road::buildRandomTrajectories()
{
    int size = num_lanes * 3;
    traj_list.resize(size);
    for (int i=0; i<size; ++i)
    {
        traj_list[i] = new Trajectory(&road_curve);
    }
}

//========================================================================================
// Step functions for 1D simulation
void Road::stepX()
{
    consumeParticles();
    produceParticles();

    updateNeighborsAndDensityPressure();

    updateLaneMeshes();

    updateCarParticles();

    updateSensors();
    updateParticles();

//    handleLaneChangingParticles();
//    handleLaneChangingCarParticles();
}

void Road::stepXNOCars()
{
    consumeParticles();
    produceParticles();

    updateNeighborsAndDensityPressure();

    updateLaneMeshes();

    updateSensors();
    updateParticles();

//    handleLaneChangingParticles();
//    handleLaneChangingCarParticles();
}

void Road::stepXNOFieldMeshes()
{
    consumeParticles();
    produceParticles();

    updateNeighborsAndDensityPressure();

    updateCarParticles();

    updateSensors();
    updateParticles();

//    handleLaneChangingParticles();
//    handleLaneChangingCarParticles();
}

void Road::stepXNOFieldMeshesCars()
{
    consumeParticles();
    produceParticles();

    updateNeighborsAndDensityPressure();

    updateSensors();
    updateParticles();

//    handleLaneChangingParticles();
//    handleLaneChangingCarParticles();
}

void Road::stepXByTrajectories()
{
    consumeParticles();
    produceCarsWithTrajectory();

//    updateNeighborsAndDensityPressure();

//    updateLaneMeshes();

    updateCarsWithTrajectory();

//    updateParticles();

//    handleLaneChangingParticles();
//    handleLaneChangingCarParticles();
}

//========================================================================================
void Road::createParticles()
{
    cleanParticles();

    double step_x_pos = DEFAULT_LANE_WIDTH * 1.0;
    for (int k=0; k<num_lanes; ++k)
    {
        vector<Particle*> part_list;

        int part_count = 0;
        double x_pos = road_curve.length;
        while (x_pos > 0.0)
        {
            Particle *p_part = new Particle;
            max_particleID++;

            p_part->id = part_count++;
            p_part->road_id = id;
            p_part->lane_id = k;

            p_part->lanechanging_status = 0;

            p_part->x_position = x_pos;
            p_part->x_velocity = speed_limit;
            p_part->x_acceleration = 0;

            p_part->mass = p_SysParam->ParticleMass;
            //        p_part->cell_id = findCellId(x_pos);
//            p_part->x_mapped = x_pos / road_curve.length;

            zVec2 nl = road_curve.getNormalByXPosition(x_pos);
            p_part->position = road_curve.getPointByXPosition(x_pos) + nl * bias_distances[k];

            p_part->neighbors.clear();
            p_part->neighborsHalfRegion.clear();

            part_list.push_back(p_part);

            x_pos -= step_x_pos;
        }

        particle_lists.push_back(part_list);
    }

    num_particles = max_particleID;

    cout << "Road " << id << ", particles " << num_particles << endl;
}

void Road::createCarParticles()
{
    cleanCars();

    int num_carmesh = p_SysParam->car_mesh_list.size();
    int num_carcolor = p_SysParam->car_color_list.size();
    for (int k=0; k<num_lanes; ++k)
    {
        vector<CarParticle*> car_list;

        int car_count = 0;

        // place a car every 10 particles.
        vector<Particle*> *part_list = &particle_lists[k];
        int size = part_list->size()-10;
        for (int i=0; i<size; i+=10)
        {
            double x_pos = part_list->at(i)->x_position;

            CarParticle *p_car = new CarParticle(&road_curve, bias_distances[k]);
            max_carID++;

            // assign ids
            p_car->id = car_count++;
            p_car->road_id = id;
            p_car->lane_id = k;

            // assign car mesh and color
            p_car->carmesh_id = Randomer::generateRandomId(num_carmesh);
            p_car->carmesh = p_SysParam->car_mesh_list[p_car->carmesh_id];
            p_car->carcolor_id = Randomer::generateRandomId(num_carcolor);
            p_car->carcolor = p_SysParam->car_color_list[p_car->carcolor_id];

            // init by the x_position
            p_car->initByXPosition(x_pos - p_car->carmesh->length*0.5);

            car_list.push_back(p_car);
        }

        car_lists.push_back(car_list);
    }

    num_cars = max_carID;
}

//========================================================================================
void Road::produceParticles()
{    
    for (int k=0; k<num_lanes; ++k)
    {
        int dd = lane_direction[k];

        if (isInflow[dd])
        {
//            double inc = inflow_rate[dd] * p_SysParam->DeltaTime *
//                    (4.0 - 4.0 * Randomer::generateRandomFromZeroToOne());
            double inc = inflow_rate[dd] * p_SysParam->DeltaTime *
                    (1.2 - 0.4 * Randomer::generateRandomFromZeroToOne());            production_car[k] += inc;
            production_particle[k] += inc * 10.0;

            // produce particles
            int num_newpart = production_particle[k];
            production_particle[k] -= num_newpart;
            for (int i=0; i<num_newpart; ++i)
            {
                Particle *p_part = new Particle;
                max_particleID++;

                p_part->id = particle_lists[k].size();
                p_part->road_id = id;
                p_part->lane_id = k;

                p_part->lanechanging_status = 0;
                p_part->lanechanging_percentage = 0;

                p_part->x_position = 0;
                p_part->x_velocity = 20.0;  //initial velocity
                p_part->x_acceleration = 0;

                p_part->mass = p_SysParam->ParticleMass;
                //                p_part->x_mapped = 0;

                zVec2 nl = road_curve.getNormalByXPosition(0);
                p_part->position = road_curve.getPointByXPosition(0) + nl * bias_distances[k];

                p_part->neighbors.clear();
                p_part->neighborsHalfRegion.clear();

                particle_lists[k].push_back(p_part);
            }

            // produce cars
            int num_newcar = production_car[k];
            production_car[k] -= num_newcar;
            for (int i=0; i<num_newcar; ++i)
            {
                int num_carmesh = p_SysParam->car_mesh_list.size();
                int num_carcolor = p_SysParam->car_color_list.size();

                CarParticle *p_car = new CarParticle(&road_curve, bias_distances[k]);
                max_carID++;

                p_car->id = car_lists[k].size();
                p_car->road_id = id;
                p_car->lane_id = k;

                p_car->carmesh_id = Randomer::generateRandomId(num_carmesh);
                p_car->carmesh = p_SysParam->car_mesh_list[p_car->carmesh_id];
                p_car->carcolor_id = Randomer::generateRandomId(num_carcolor);
                p_car->carcolor = p_SysParam->car_color_list[p_car->carcolor_id];

                p_car->initByXPosition(0.0);

                int size = car_lists[k].size();
                if (size == 0)
                {
                    car_lists[k].push_back(p_car);
                }
                else
                {
                    // check with its previous car.
                    // discard it if can't place it.
                    CarParticle *p_car_pre = car_lists[k].at(size-1);
                    if (p_car->length > p_car_pre->x_position - p_car_pre->length*0.5)
                    {
                        delete p_car;
                    }
                    else
                    {
                        car_lists[k].push_back(p_car);
                    }
                }
            }

        }
    }
}

void Road::consumeParticles()
{
    for (int k=0; k<num_lanes; ++k)
    {
        //------------------------------------------------------------------------------------
        // remove the particles beyond the end point of the lane or having changed out.
        vector<Particle*> *part_list = &particle_lists[k];
        for (vector<Particle*>::iterator it = part_list->begin();
                                         it != part_list->end(); ++it)
        {
            if ((*it)->x_position < 0.0)
            {
                cout << "Warning! Negative x_pos " << (*it)->x_position << " "
                     << (*it)->id << endl;
            }


            if ((*it)->x_position < 0.0 ||
                (*it)->x_position >= road_length || (*it)->lanechanging_status == 3)
            {
                if (*it == part_list->back())
                {
                    part_list->pop_back();
                    num_particles--;
                    break;
                }
                else
                {
                    swap(*it, part_list->back());
                    part_list->pop_back();
                    num_particles--;
                }
            }
        }

        // re-assign the id for each particle
        int size = part_list->size();
        for (int i=0; i<size; ++i)
        {
            part_list->at(i)->id = i;
        }
    }

    for (int k=0; k<num_lanes; ++k)
    {
        //------------------------------------------------------------------------------------
        // remove the vehicles beyond the end point of the lane or having changed out.
        // make a copy then clear
        vector<CarParticle*> *car_list = &car_lists[k];
        vector<CarParticle*> temp_car_list = *car_list;
        num_cars -= temp_car_list.size();
        car_list->clear();

        // determine the first car to be reserved.
        vector<CarParticle*>::iterator ss = temp_car_list.begin();
        while (ss != temp_car_list.end())
        {
           if ((*ss)->x_position < road_length && (*ss)->lanechanging_status != 3)
           {
               break;
           }
           ss++;
        }

        if (ss != temp_car_list.end())
        {
            vector<CarParticle*>::iterator ee = ss+1;
            while (ee != temp_car_list.end())
            {
                if ((*ee)->x_position >= road_length || (*ee)->lanechanging_status == 3)
                {
                    car_list->insert(car_list->end(), ss, ee-1);
                    ss = ee + 1;
                    while (ss != temp_car_list.end())
                    {
                       if ((*ss)->x_position < road_length && (*ss)->lanechanging_status != 3)
                       {
                           break;
                       }
                       ss++;
                    }
                    ee = ss;
                }
                else
                {
                    ee++;
                }
            }

            car_list->insert(car_list->end(), ss, ee);
            num_cars += car_list->size();
        }

//        // remove the vehicles beyond the end point of the lane or having changed out.
//        vector<CarParticle*> *car_list = &car_lists[k];
//        size = car_list->size();

//        vector<CarParticle*> temp_car_list = *car_list;
//        car_list->clear();
//        for (int i=0; i<size; ++i)
//        {
//            CarParticle *p_car = temp_car_list[i];
//            if (p_car->x_position >= road_length || p_car->lanechanging_status == 3)
//            {
//                delete p_car;
//            }
//            else
//            {
//                car_list->push_back(p_car);
//                num_cars++;
//            }
//        }

        // re-assign the id for each particle
        int size = car_list->size();
        for (int i=0; i<size; ++i)
        {
            car_list->at(i)->id = i;
        }
    }

//    num_particles = 0;
//    num_cars = 0;
//    for (int k=0; k<num_lanes; ++k)
//    {
//        // remove the particles beyond the end point of the lane or having changed out.
//        vector<Particle*> *part_list = &particle_lists[k];
//        int size = part_list->size();

//        vector<Particle*> temp_list = *part_list;
//        part_list->clear();
//        for (int i=0; i<size; ++i)
//        {
//            Particle *p_part = temp_list[i];

//            if (p_part->x_position >= road_length || p_part->lanechanging_status == 3)
//            {
//                delete p_part;
//            }
//            else
//            {
//                part_list->push_back(p_part);
//                num_particles++;
//            }
//        }

//        // re-assign the id for each particle
//        size = part_list->size();
//        for (int i=0; i<size; ++i)
//        {
//            part_list->at(i)->id = i;
//        }

//        // remove the vehicles beyond the end point of the lane or having changed out.
//        vector<CarParticle*> *car_list = &car_lists[k];
//        size = car_list->size();

//        vector<CarParticle*> temp_car_list = *car_list;
//        car_list->clear();
//        for (int i=0; i<size; ++i)
//        {
//            CarParticle *p_car = temp_car_list[i];
//            if (p_car->x_position >= road_length || p_car->lanechanging_status == 3)
//            {
//                delete p_car;
//            }
//            else
//            {
//                car_list->push_back(p_car);
//                num_cars++;
//            }
//        }

//        // re-assign the id for each particle
//        size = car_list->size();
//        for (int i=0; i<size; ++i)
//        {
//            car_list->at(i)->id = i;
//        }
//    }
}

void Road::updateCarParticles()
{
    // update the regular cars, i.e. lanechanging status == 0
    for (int k=0; k<num_lanes; ++k)
    {
        vector<CarParticle*> *car_list = &car_lists[k];
        for (int i=0; i<car_list->size(); ++i)
        {
            CarParticle *p_car = car_list->at(i);

            p_car->isAnchored = false;
            if (p_SysParam->AddConstrain)
            {
                int num_block = block_list->size();
                for (int j=0; j<num_block; ++j)
                {
                    if (block_list->at(j).isInZone(p_car->position))
                    {
                        p_car->isAnchored = true;
                        p_car->x_velocity = 0;
                        break;
                    }
                }
            }

            if (!p_car->isAnchored)
            {
                double pos = p_car->x_position;
//                p_car->density = getDensityByLaneMesh(k, pos);
//                p_car->x_velocity = getVelocityByLaneMesh(k, pos);
                getDensityVelocityByInterpolation(k, pos, p_car->density, p_car->x_velocity);

                if (i == 0)
                {
                    p_car->stepX(p_SysParam->DeltaTime);
                }
                else
                {
                    p_car->stepX(p_SysParam->DeltaTime, car_list->at(i-1));
                }

                p_car->updatePositionByXPos();
                p_car->updateTail();
            }
        }
    }

    return;

    // determine the real position of the lanechanging out cars.
    double lanechanging_smoothness = 0.02;
    for (int k=0; k<num_lanes; ++k)
    {
        vector<CarParticle*> *car_list = &car_lists[k];
        for (int i=0; i<car_list->size(); ++i)
        {
            CarParticle *p_car = car_list->at(i);
            if (p_car->lanechanging_status == 1)
            {
                CarParticle *p_car_ghost = p_car->ghost_neighbor;

                // determine the real x_position
                double delta_pos = p_car_ghost->x_position - p_car->x_position;
                double pos = p_car->x_position + p_car->lanechanging_percentage * delta_pos;

                // determine the real visual position
                p_car->orientation = road_curve.getTangentByXPosition(pos);
                p_car->normal = road_curve.getNormalByXPosition(pos);
                double offset = bias_distances[k] + DEFAULT_LANE_WIDTH *
                                (-p_car->lanechanging_percentage * p_car->lanechanging_direction);
                p_car->position = road_curve.getPointByXPosition(pos) + p_car->normal * offset;

                if (p_car->lanechanging_percentage >= 1.0)
                {
                    p_car->lanechanging_status = 3;
                    p_car_ghost->lanechanging_status = 0;
                    p_car_ghost->lanechanging_percentage = 0;
                }
                else
                {
                    p_car->lanechanging_percentage += lanechanging_smoothness;
                }
            }
        }
    }
}

void Road::produceCarsWithTrajectory()
{
    for (int k=0; k<num_lanes; ++k)
    {
        double inc = inflow_rate[0] * p_SysParam->DeltaTime *
                (1.2 - 0.4 * Randomer::generateRandomFromZeroToOne());
        production_car[k] += inc;

        int num_newcar = production_car[k];
        production_car[k] -= num_newcar;
        for (int i=0; i<num_newcar; ++i)
        {
            int num_carmesh = p_SysParam->car_mesh_list.size();
            int num_carcolor = p_SysParam->car_color_list.size();
            int num_traj = traj_list.size();

            CarParticle *p_car = new CarParticle(&road_curve, bias_distances[k]);
            max_carID++;

            // indices
            p_car->id = car_lists[k].size();
            p_car->road_id = id;
            p_car->lane_id = k;

            // car mesh/color
            p_car->carmesh_id = Randomer::generateRandomId(num_carmesh);
            p_car->carmesh = p_SysParam->car_mesh_list[p_car->carmesh_id];
            p_car->carcolor_id = Randomer::generateRandomId(num_carcolor);
            p_car->carcolor = p_SysParam->car_color_list[p_car->carcolor_id];

            // assign trajectory
            p_car->trajectory = traj_list[Randomer::generateRandomId(num_traj)];
            p_car->head_length = 20;

            p_car->initByXPosition(0.0);

            p_car->length = p_SysParam->ParticleRadius * 2;

            int size = car_lists[k].size();
            if (size == 0)
            {
                car_lists[k].push_back(p_car);
            }
            else
            {
                // check with its previous car.
                // discard it if can't place it.
                CarParticle *p_car_pre = car_lists[k].at(size-1);
                if (p_car->length > p_car_pre->x_position - p_car_pre->length*0.5)
                {
                    delete p_car;
                }
                else
                {
                    car_lists[k].push_back(p_car);
                }
            }
        }
    }
}

void Road::updateCarsWithTrajectory()
{
    for (int k=0; k<num_lanes; ++k)
    {
        vector<CarParticle*> *car_list = &car_lists[k];
        for (int i=0; i<car_list->size(); ++i)
        {
            CarParticle *p_car = car_list->at(i);

            // determine the x_velocity
            p_car->computeXVelocityByTrajectory();

            if (i == 0)
            {
                p_car->stepX(p_SysParam->DeltaTime);
            }
            else
            {
                p_car->stepX(p_SysParam->DeltaTime, car_list->at(i-1));
            }

            p_car->updatePositionByXPos();
            p_car->updateTail();
//            p_car->updateHead();
        }
    }
}

double Road::getDensityByLaneMesh(double lane_id, double x_pos)
{
    TriangleMesh *p_mesh = lane_density_meshes[lane_id];
    if (x_pos <= 0)
    {
        return p_mesh->node_list[0]->density;
    }
    else if (x_pos >= road_length)
    {
        int idx = p_mesh->node_list.size() - 2;
        return p_mesh->node_list[idx]->density;
    }
    else
    {
        // the road is actually divided by num_seg segments.
        int num_seg = p_mesh->node_list.size()/2 - 1;
        int idx = (num_seg * (x_pos / road_length)) * 2;
        return (p_mesh->node_list[idx]->density + p_mesh->node_list[idx+2]->density) * 0.5;
    }
}

double Road::getVelocityByLaneMesh(double lane_id, double x_pos)
{
    TriangleMesh *p_mesh = lane_velocity_meshes[lane_id];
    if (x_pos <= 0)
    {
        return p_mesh->node_list[0]->velocity;
    }
    else if (x_pos >= road_length)
    {
        int idx = p_mesh->node_list.size() - 2;
        return p_mesh->node_list[idx]->velocity;
    }
    else
    {
        // the road is actually divided by num_seg segments.
        int num_seg = p_mesh->node_list.size()/2 - 1;
        int idx = (num_seg * (x_pos / road_length)) * 2;
        return (p_mesh->node_list[idx]->velocity + p_mesh->node_list[idx+2]->velocity) * 0.5;
    }
}

void Road::getDensityVelocityByInterpolation(double lane_id, double x_pos, double &dd, double &vv)
{
    // find neighbours by the position
    vector<Particle*> neighbors;
    hash_tables[lane_id]->getNeighborsByXPosition(x_pos, neighbors);

    // interpolate the density and velocity
    dd = 0;
    vv = 0;
    int num_neigh = neighbors.size();
    for (int j=0; j<num_neigh; ++j)
    {
        Particle* p_neigh = neighbors[j];
        double k = Kernel::getPoly1D(x_pos - p_neigh->x_position);

        dd += p_neigh->mass * k;
        vv += p_neigh->x_velocity * (p_neigh->mass * k / p_neigh->density);
    }
}

void Road::buildLaneMeshes()
{
    lane_density_meshes.clear();
    lane_velocity_meshes.clear();

    // Determine the visual space
    field_mesh_top = DEFAULT_LANE_WIDTH * 4.0 * 2.5;
    field_mesh_bottom = DEFAULT_LANE_WIDTH * 4.0 * 0.8;

    double x_step_length = lane_width * 0.5;

    for (int i=0; i<num_lanes; ++i)
    {
        TriangleMesh *lane_density_mesh = new TriangleMesh;
        TriangleMesh *lane_velocity_mesh = new TriangleMesh;

        double x_pos = 0.0;
        while (x_pos < road_curve.length)
        {
            zVec2 nl = road_curve.getNormalByXPosition(x_pos);
            zVec2 center_point = road_curve.getPointByXPosition(x_pos) + nl * bias_distances[i];
            zVec2 left_point = center_point + nl * (lane_width*0.5);
            zVec2 right_point = center_point - nl * (lane_width*0.5);

            // nodes for density
            Node *p_node = new Node;

            p_node->original_position[0] = left_point[0];
            p_node->original_position[1] = left_point[1];
            p_node->original_position[2] = field_mesh_bottom;
            p_node->position = p_node->original_position;
            p_node->x_position = x_pos;
            p_node->x_mapped = x_pos / road_curve.length;
            lane_density_mesh->node_list.push_back(p_node);

            p_node = new Node;
            p_node->original_position[0] = right_point[0];
            p_node->original_position[1] = right_point[1];
            p_node->original_position[2] = field_mesh_bottom;
            p_node->position = p_node->original_position;
            p_node->x_position = x_pos;
            p_node->x_mapped = x_pos / road_curve.length;
            lane_density_mesh->node_list.push_back(p_node);

            // nodes for velocity
            p_node = new Node;
            p_node->original_position[0] = left_point[0];
            p_node->original_position[1] = left_point[1];
            p_node->original_position[2] = field_mesh_bottom;
            p_node->position = p_node->original_position;
            p_node->x_position = x_pos;
            p_node->x_mapped = x_pos / road_curve.length;
            lane_velocity_mesh->node_list.push_back(p_node);

            p_node = new Node;
            p_node->original_position[0] = right_point[0];
            p_node->original_position[1] = right_point[1];
            p_node->original_position[2] = field_mesh_bottom;
            p_node->position = p_node->original_position;
            p_node->x_position = x_pos;
            p_node->x_mapped = x_pos / road_curve.length;
            lane_velocity_mesh->node_list.push_back(p_node);

            x_pos += x_step_length;
        }

        lane_density_mesh->buildMeshTwoNodesPerGroup();
        lane_density_mesh->computeNormal();
        lane_density_meshes.push_back(lane_density_mesh);

        lane_velocity_mesh->buildMeshTwoNodesPerGroup();
        lane_velocity_mesh->computeNormal();
        lane_velocity_meshes.push_back(lane_velocity_mesh);
    }
}

void Road::updateLaneMeshes()
{
    for (int k=0; k<num_lanes; ++k)
    {
        HashTableX *hash_table_x = hash_tables[k];
        TriangleMesh *p_density_mesh = lane_density_meshes[k];
        TriangleMesh *p_velocity_mesh = lane_velocity_meshes[k];

        int num_nodes = p_density_mesh->node_list.size() / 2;
        for (int i=0; i<num_nodes; ++i)
        {
            int i0 = 2*i, i1 = i0+1;
            Node *p_node = p_density_mesh->node_list[i0];
            Node *p_node_1 = p_density_mesh->node_list[i1];

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

            // for density nodes i and i+1
            p_node->density = density; // real density
            p_node->normalized_density = density / p_SysParam->MaxDensity; // normalized density
            p_node->position.z = field_mesh_bottom +
                    (field_mesh_top - field_mesh_bottom) * p_node->normalized_density;
            if (p_node->position.z > field_mesh_top)
            {
                p_node->position.z = field_mesh_top;
            }
            p_node_1->density = p_node->density;
            p_node_1->normalized_density = p_node->normalized_density;
            p_node_1->position.z = p_node->position.z;

            // for velocity nodes i and i+1
            p_node = p_velocity_mesh->node_list[i0];
            p_node_1 = p_velocity_mesh->node_list[i1];

            p_node->velocity = velocity; // real velocity
            p_node->normalized_velocity = velocity / p_SysParam->MaxVelocity;  // normalized velocity
            p_node->position.z = field_mesh_bottom +
                    (field_mesh_top - field_mesh_bottom) * p_node->normalized_velocity;
            if (p_node->position.z > field_mesh_top)
            {
                p_node->position.z = field_mesh_top;
            }
            p_node_1->velocity = p_node->velocity;
            p_node_1->normalized_velocity = p_node->normalized_velocity;
            p_node_1->position.z = p_node->position.z;
        }

        p_density_mesh->computeNormal();
        p_velocity_mesh->computeNormal();
    }
}

void Road::createSensors()
{
    sensor_lists.clear();
    sensor_lists.resize(num_lanes);

    // Determine the number of sensors along each lane.
    int num_sensors = 20;
//    int num_sensors = 200;
    if (p_SysParam->SimulationMode == 2)
    {
        num_sensors = 60;
    }

    // Determine the step length along x and y directions.
    double step_y = lane_width;
    double step_x = road_length / (num_sensors-1);

    double x_pos = 0.0;
    for (int i=0; i<num_sensors; ++i)
    {
        zVec2 nl = road_curve.getNormalByXPosition(x_pos);
        zVec2 pos = road_curve.getPointByXPosition(x_pos);

        zVec2 moving_pos = pos + nl*(step_y*(double(num_lanes)*0.5-0.5));
        for (int j=0; j<num_lanes; ++j)
        {
            Node nnode;
            nnode.original_position.x = moving_pos.x;
            nnode.original_position.y = moving_pos.y;
            nnode.original_position.z = field_mesh_bottom;

            nnode.position = nnode.original_position;
            nnode.density = 0.0;
            nnode.velocity = 0.0;
            nnode.normalized_density = 0.0;
            nnode.normalized_velocity = 0.0;

            nnode.x_position = x_pos;

            sensor_lists[j].push_back(nnode);

            moving_pos -= nl*step_y;
        }

        x_pos += step_x;
    }
}

void Road::updateSensors()
{
    for (int k=0; k<num_lanes; ++k)
    {
        HashTableX *hash_table_x = hash_tables[k];
        vector<Node> *p_sensor_list = &sensor_lists[k];
        int num_sensors = p_sensor_list->size();
        for (int i=0; i<num_sensors; ++i)
        {
            Node *p_node = &p_sensor_list->at(i);

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

            p_node->density = density; // real density
            p_node->normalized_density = density / p_SysParam->MaxDensity; // normalized density

            p_node->velocity = velocity; // real velocity
            p_node->normalized_velocity = velocity / p_SysParam->MaxVelocity;  // normalized velocity
        }
    }
}

void Road::updateNeighborsAndDensityPressure()
{
    // Find neighbors and compute density and pressure.
    for (int k=0; k<num_lanes; ++k)
    {
        // Get the particle list of the lane
        // Re-build the hash table and determine the neighbour list of each particle.
        vector<Particle*> *p_list = &particle_lists[k];
        HashTableX *hash_table_x = hash_tables[k];
        hash_table_x->buildHashTableX(p_list, p_SysParam->SupportRadius);
        int num_part = p_list->size();
        for (int i=0; i<num_part; ++i)
        {
            Particle *p_part = p_list->at(i);
            hash_table_x->getTwoNeighborListsByXPosition(p_part->x_position,
                                                         p_part->neighbors,
                                                         p_part->neighborsHalfRegion);
//            hash_table_x->getNeighborsByXPosition(p_part->x_position, p_part->neighbors);
//            hash_table_x->getNeighborsHalfRegionByXPosition(p_part->x_position,
//                                                            p_part->neighborsHalfRegion);
        }

        // Compute the density and the pressure of each particle.
        for (int i=0; i<num_part; ++i)
        {
            Particle *p_part = p_list->at(i);

            // determine the density
            double density = 0;
            int num_neigh = p_part->neighbors.size();
            for (int j=0; j<num_neigh; ++j)
            {
                Particle *p_neigh = p_part->neighbors[j];
                density += p_neigh->mass * Kernel::getPoly1D(p_part->x_position - p_neigh->x_position);
            }
            // Note: computing the density should take itself into account
//            density += p_part->mass * Kernel::getPoly1D(0.0);
            p_part->density = density;

            // determine the pressure
            //    p_part->pressure = p_SysParam->GasStiffness * density * 0.15 ;//1.0
            //    p_part->pressure = p_SysParam->GasStiffness * density * sqrt(density) * 0.4;//1.5
            p_part->pressure = p_SysParam->GasStiffness * density * density;//2.0
            //    p_part->pressure = p_SysParam->GasStiffness * density * density * sqrt(density) *1.7;//2.5
        }
    }
}

void Road::updateParticles()
{
    computeXTrafficForces();

    for (int k=0; k<num_lanes; ++k)
    {
        vector<Particle*> *p_list = &particle_lists[k];
        int num_part = p_list->size();
        for (int i=0; i<num_part; ++i)
        {
            Particle *p_part = p_list->at(i);
            p_part->isAnchored = false;

            if (p_SysParam->AddConstrain)
            {
                int num_block = block_list->size();
                for (int j=0; j<num_block; ++j)
                {
                    if (block_list->at(j).isInZone(p_part->position))
                    {
                        p_part->isAnchored = true;
                        break;
                    }
                }
            }

            if (p_part->isAnchored)
            {
                // Constrain x_velocity to zero
//                p_part->x_velocity = p_SysParam->MaxVelocity * 0.1;
//                p_part->x_position += p_part->x_velocity * p_SysParam->DeltaTime;

                // Update the position map
//                if (p_part->x_position < 0)
//                {
//                    //p_part->x_position = 0;
//                    p_part->isBad = true;
//                }
//                p_part->x_mapped = p_part->x_position / road_length;

                p_part->x_velocity = speed_constrain;
                p_part->x_position += p_part->x_velocity * p_SysParam->DeltaTime;

                // update the visual position
                zVec2 nl = road_curve.getNormalByXPosition(p_part->x_position);
                p_part->position = road_curve.getPointByXPosition(p_part->x_position) + nl * bias_distances[k];
            }
            else
            {
                p_part->x_acceleration = p_part->x_trafficForce / p_part->density;

                p_part->stepXPosition(p_SysParam->DeltaTime);

                // Update the position map
//                if (p_part->x_position < 0)
//                {
//                    p_part->x_position = 0;
//                }
//                p_part->x_mapped = p_part->x_position / road_length;

                // update the visual position
                zVec2 nl = road_curve.getNormalByXPosition(p_part->x_position);
                p_part->position = road_curve.getPointByXPosition(p_part->x_position) + nl * bias_distances[k];

//                if (p_part->isSelected)
//                {
//                    cout << "id x_pos position = " << p_part->id
//                         << " " << p_part->x_position
//                         << " " << p_part->position << endl;
//                }
            }
        }
    }

    return;

    // deal with the lanechanging out particles, i.e. lanechanging status == 1.
    double lanechanging_smoothness = 0.01;
    for (int k=0; k<num_lanes; ++k)
    {
        vector<Particle*> *p_list = &particle_lists[k];
        int num_part = p_list->size();
        for (int i=0; i<num_part; ++i)
        {
            Particle *p_part = p_list->at(i);

            if (p_part->lanechanging_status == 1)
            {
                Particle *p_part_target = p_part->lanechanging_target_part;

                // transfer the mass to the target particle.
                p_part->mass = p_SysParam->ParticleMass * (1.0 - p_part->lanechanging_percentage);
                if (p_part->mass <= 0.0)
                {
                    // set the status to end changing out and will be removed soon.
                    p_part->lanechanging_status = 3;
                    p_part->lanechanging_percentage = 0;
                    p_part->mass = 0;

                    // set the target part to regular status, i.e. lanechanging == 0.
                    p_part_target->lanechanging_status = 0;
                    p_part_target->lanechanging_percentage = 0;
                    p_part_target->mass = p_SysParam->ParticleMass;
                }
                else
                {
                    p_part_target->mass = p_SysParam->ParticleMass - p_part->mass;
                }

                p_part->lanechanging_percentage += lanechanging_smoothness;
            }
        }
    }

//    // Update the particles that are changing out, i.e. transferring the mass.
//    double lanechanging_smoothness = 0.01;
//    num_part = lanechanging_particle_list.size();
//    for (int i=0; i<num_part; ++i)
//    {
//        Particle *p_part = lanechanging_particle_list[i];

//        // transfer the mass to the target particle.
//        p_part->lanechanging_percentage += lanechanging_smoothness;
//        p_part->mass = p_SysParam->ParticleMass * (1.0 - p_part->lanechanging_percentage);
//        p_part->lanechanging_target_part->mass = p_SysParam->ParticleMass - p_part->mass;
//        if (p_part->mass <= 0.0)
//        {
//            // set the status to end changing out and will be removed soon.
//            p_part->lanechanging_status = 3;
//            p_part->mass = 0;

//            // set the target part to no lane changing.
//            p_part->lanechanging_target_part->mass = p_SysParam->ParticleMass;
//            p_part->lanechanging_target_part->lanechanging_status = 0;

//            lanechanging_particle_list[i] = NULL;
//        }
//    }

//    // clean up the lanechanging_particle_list
//    vector<Particle*> temp_list = lanechanging_particle_list;
//    lanechanging_particle_list.clear();
//    num_part = temp_list.size();
//    for (int i=0; i<num_part; ++i)
//    {
//        if (temp_list[i])
//        {
//            lanechanging_particle_list.push_back(temp_list[i]);
//        }
//    }
}

void Road::computeXTrafficForces()
{
    computeXDampingForces();

    if (p_SysParam->UseExternalForce)
    {
        computeXExternalForces();
    }

    if (p_SysParam->UsePressureForce)
    {
        if (p_SysParam->UseHalfKernel)
        {
            computeXPressureForcesHalfRegion();
        }
        else
        {
            computeXPressureForces();
        }
    }

    if (p_SysParam->UseViscosityForce)
    {
        if (p_SysParam->UseHalfKernel)
        {
            computeXViscosityForcesHalfRegion();
        }
        else
        {
            computeXViscosityForces();
        }
    }

    // Sum to traffic forces.
    for (int k=0; k<num_lanes; ++k)
    {
        vector<Particle*> *p_list = &particle_lists[k];
        int num_part = p_list->size();
        for (int i=0; i<num_part; ++i)
        {
            Particle *p_part = p_list->at(i);
            p_part->x_trafficForce = p_part->x_dampingForce +
                                     p_part->x_externalForce +
                                     p_part->x_pressureForce +
                                     p_part->x_viscosityForce;
        }
    }
}

void Road::computeXDampingForces()
{
    for (int k=0; k<num_lanes; ++k)
    {
        vector<Particle*> *p_list = &particle_lists[k];
        int num_part = p_list->size();
        for (int i=0; i<num_part; ++i)
        {
            Particle *p_part = p_list->at(i);
            p_part->x_dampingForce = p_part->x_velocity *
                                     (- p_SysParam->CoeffDampingForce * p_part->density);
        }
    }
}

void Road::computeXExternalForces()
{
    for (int k=0; k<num_lanes; ++k)
    {
        vector<Particle*> *p_list = &particle_lists[k];
        int num_part = p_list->size();
        for (int i=0; i<num_part; ++i)
        {
            Particle *p_part = p_list->at(i);
            p_part->x_externalForce = p_SysParam->CoeffFieldForce * p_part->density;
        }
    }
}

void Road::computeXPressureForces()
{
    for (int k=0; k<num_lanes; ++k)
    {
        vector<Particle*> *p_list = &particle_lists[k];
        int num_part = p_list->size();
        for (int i=0; i<num_part; ++i)
        {
            Particle *p_part = p_list->at(i);

            // compute pressure force from the neighbour list
            double pressureForce = 0.0;
            vector<Particle*>* p_neigh_list = &p_part->neighbors;
            int num_neigh = p_neigh_list->size();
            for (int j=0; j<num_neigh; ++j)
            {
                Particle* p_neigh = p_neigh_list->at(j);
                if (p_part != p_neigh)
                {
                    double coef = (p_part->pressure + p_neigh->pressure) *
                                  p_neigh->mass / (2.0 * p_neigh->density);
                    pressureForce += Kernel::getSpikyGradient1D(p_part->x_position - p_neigh->x_position)
                                     * coef;
                }
            }

            p_part->x_pressureForce = pressureForce * (-1.0);
        }
    }
}

void Road::computeXPressureForcesHalfRegion()
{
    for (int k=0; k<num_lanes; ++k)
    {
        vector<Particle*> *p_list = &particle_lists[k];
        int num_part = p_list->size();
        for (int i=0; i<num_part; ++i)
        {
            Particle *p_part = p_list->at(i);

            // compute pressure force from the neighbour list
            double pressureForce = 0.0;
            vector<Particle*>* p_neigh_list = &p_part->neighborsHalfRegion;
            int num_neigh = p_neigh_list->size();
            for (int j=0; j<num_neigh; ++j)
            {
                Particle* p_neigh = p_neigh_list->at(j);
                if (p_part != p_neigh)
                {
                    double coef = (p_part->pressure + p_neigh->pressure) *
                                   p_neigh->mass / (2.0 * p_neigh->density);
                    pressureForce += Kernel::getSpikyGradient1DHK(p_part->x_position - p_neigh->x_position)
                                     * coef;
                }
            }

            p_part->x_pressureForce = pressureForce * (-1.0);
        }
    }
}

void Road::computeXViscosityForces()
{
    for (int k=0; k<num_lanes; ++k)
    {
        vector<Particle*> *p_list = &particle_lists[k];
        int num_part = p_list->size();
        for (int i=0; i<num_part; ++i)
        {
            Particle *p_part = p_list->at(i);

            // compute viscosity force from the neighbour list
            double viscosityForce = 0.0;
            vector<Particle*>* p_neigh_list = &p_part->neighbors;
            int num_neigh = p_neigh_list->size();
            for (int j=0; j<num_neigh; ++j)
            {
                Particle* p_neigh = p_neigh_list->at(j);

                if (p_part != p_neigh)
                {
                    double coef = Kernel::getSpikyLaplacian1D(p_neigh->x_position - p_part->x_position)
                                  * p_neigh->mass / p_neigh->density;
                    viscosityForce += (p_neigh->x_velocity - p_part->x_velocity) * coef;
                }
            }

            p_part->x_viscosityForce = p_SysParam->Viscosity * viscosityForce;
        }
    }
}

void Road::computeXViscosityForcesHalfRegion()
{
    for (int k=0; k<num_lanes; ++k)
    {
        vector<Particle*> *p_list = &particle_lists[k];
        int num_part = p_list->size();
        for (int i=0; i<num_part; ++i)
        {
            Particle *p_part = p_list->at(i);

            // compute viscosity force from the neighbour list
            double viscosityForce = 0.0;
            vector<Particle*>* p_neigh_list = &p_part->neighborsHalfRegion;
            int num_neigh = p_neigh_list->size();
            for (int j=0; j<num_neigh; ++j)
            {
                Particle* p_neigh = p_neigh_list->at(j);

                if (p_part != p_neigh)
                {
                    double coef = Kernel::getSpikyLaplacian1DHK(p_neigh->x_position - p_part->x_position)
                                  * p_neigh->mass / p_neigh->density;
                    viscosityForce += (p_neigh->x_velocity - p_part->x_velocity) * coef;
                }
            }

            p_part->x_viscosityForce = p_SysParam->Viscosity * viscosityForce;
        }
    }
}

void Road::handleLaneChangingParticles()
{
    if (num_lanes > 1)
    {
        // Check with the particles lane by lane and determine the set needs to apply lanechanging.
        // The leftmost lane.
        vector<Particle*> *p_list = &particle_lists[0];
        int num_part = p_list->size();
        for (int i=0; i<num_part; ++i)
        {
            Particle *p_part = p_list->at(i);
            if (p_part->lanechanging_status == 0)
            {
                // check with the density difference
                double right_diff = p_part->density - getDensityByLaneMesh(1, p_part->x_position);
                if (right_diff > p_SysParam->LaneChangingDensityGap)
                {
                    duplicateParticleForLaneChanging(1, p_part);
                }
            }
        }

        // The rightmost lane.
        p_list = &particle_lists[num_lanes-1];
        num_part = p_list->size();
        for (int i=0; i<num_part; ++i)
        {
            Particle *p_part = p_list->at(i);
            if (p_part->lanechanging_status == 0)
            {
                // check with the density difference
                double left_diff = p_part->density -
                                   getDensityByLaneMesh(num_lanes-2, p_part->x_position);
                if (left_diff > p_SysParam->LaneChangingDensityGap)
                {
                    duplicateParticleForLaneChanging(num_lanes-2, p_part);
                }
            }
        }

        // The middle lanes
        for (int k=1; k<num_lanes-1; ++k)
        {
            p_list = &particle_lists[k];
            int num_part = p_list->size();
            for (int i=0; i<num_part; ++i)
            {
                Particle *p_part = p_list->at(i);
                if (p_part->lanechanging_status == 0)
                {
                    double left_diff = p_part->density -
                                       getDensityByLaneMesh(k-1, p_part->x_position);
                    double right_diff = p_part->density -
                                        getDensityByLaneMesh(k+1, p_part->x_position);

                    // determine to lanechanging with bigger density gap.
                    if (left_diff >= right_diff && left_diff > p_SysParam->LaneChangingDensityGap)
                    {
                        duplicateParticleForLaneChanging(k-1, p_part);
                    }
                    else if (left_diff < right_diff && right_diff > p_SysParam->LaneChangingDensityGap)
                    {
                        duplicateParticleForLaneChanging(k+1, p_part);
                    }
                }
            }
        }
    }
}

void Road::duplicateParticleForLaneChanging(int lane_id, Particle *p_part)
{
    vector<Particle*> *p_list = &particle_lists[lane_id];

    // create the lanechanging target particle
    Particle *p_part_new = p_part->duplicate();
    p_part_new->id = max_particleID++;
    p_part_new->road_id = id;
    p_part_new->lane_id = lane_id;
    p_part_new->mass = 0;
    p_part_new->lanechanging_status = 2;  // this is lanechanging in.
    p_part_new->lanechanging_percentage = 0;

    // push the new part to the target lane
    p_list->push_back(p_part_new);

    // update the current part
    p_part->lanechanging_target_part = p_part_new;
    p_part->lanechanging_status = 1;  // this is lanechanging out.
    p_part->lanechanging_percentage = 0;

//    // push the part to the lanechanging list
//    lanechanging_particle_list.push_back(p_part);
}

void Road::handleLaneChangingCarParticles()
{
    if (num_lanes > 1)
    {
        // Check with the cars lane by lane and determine the set needs to apply lanechanging.
        // The leftmost lane.
        vector<CarParticle*> *p_list = &car_lists[0];
        int num_part = p_list->size();
        for (int i=0; i<num_part; ++i)
        {
            CarParticle *p_car = p_list->at(i);
            if (p_car->lanechanging_status == 0)
            {
                // check with the density difference
                double right_diff = p_car->density - getDensityByLaneMesh(1, p_car->x_position);
                if (right_diff > p_SysParam->LaneChangingDensityGap)
                {
                    duplicateCarParticleForLaneChanging(1, p_car, 1);
                    p_car->needLaneChanging = true;
                }
                else
                {
                    p_car->needLaneChanging = false;
                }
            }
        }

        // The rightmost lane.
        p_list = &car_lists[num_lanes-1];
        num_part = p_list->size();
        for (int i=0; i<num_part; ++i)
        {
            CarParticle *p_car = p_list->at(i);
            if (p_car->lanechanging_status == 0)
            {
                // check with the density difference
                double left_diff = p_car->density -
                                   getDensityByLaneMesh(num_lanes-2, p_car->x_position);
                if (left_diff > p_SysParam->LaneChangingDensityGap)
                {
                    duplicateCarParticleForLaneChanging(num_lanes-2, p_car, -1);
                    p_car->needLaneChanging = true;
                }
                else
                {
                    p_car->needLaneChanging = false;
                }
            }
        }

        // The middle lanes
        for (int k=1; k<num_lanes-1; ++k)
        {
            p_list = &car_lists[k];
            int num_part = p_list->size();
            for (int i=0; i<num_part; ++i)
            {
                CarParticle *p_car = p_list->at(i);
                if (p_car->lanechanging_status == 0)
                {
                    double left_diff = p_car->density -
                                       getDensityByLaneMesh(k-1, p_car->x_position);
                    double right_diff = p_car->density -
                                        getDensityByLaneMesh(k+1, p_car->x_position);

                    // determine to lanechanging with bigger density gap.
                    if (left_diff >= right_diff && left_diff > p_SysParam->LaneChangingDensityGap)
                    {
                        duplicateCarParticleForLaneChanging(k-1, p_car, -1);
                        p_car->needLaneChanging = true;
                    }
                    else if (left_diff < right_diff && right_diff > p_SysParam->LaneChangingDensityGap)
                    {
                        duplicateCarParticleForLaneChanging(k+1, p_car, 1);
                        p_car->needLaneChanging = true;
                    }
                    else
                    {
                        p_car->needLaneChanging = false;
                    }
                }
            }
        }
    }


//    double delta_normal_move = DEFAULT_LANE_WIDTH * 0.02;

//    if (left_lane && left_lane->isRunning && right_lane && right_lane->isRunning)
//    {
//        for (int i=0; i<num_cars; ++i)
//        {
//            CarParticle *p_car = car_list[i];

//            if (p_car->lanechanging_state == 0)
//            {
//                double left_diff = p_car->density - left_lane->getDensityAtPosition(p_car->position);
//                double right_diff = p_car->density - right_lane->getDensityAtPosition(p_car->position);

//                if (left_diff <= right_diff && right_diff > p_SysParam->LaneChangingDensity)
//                {
//                    p_car->ghost_neighbor = right_lane->duplicateOneCarParticle(p_car);

//                    // should check if the duplication succeeded.
//                    if (p_car->ghost_neighbor)
//                    {
//                        p_car->ghost_neighbor->lanechanging_state = 3;
//                        p_car->lanechanging_state = 2;
//                        p_car->normal_move = 0;
//                    }
//                }
//                else if (left_diff >= right_diff && left_diff > p_SysParam->LaneChangingDensity)
//                {
//                    p_car->ghost_neighbor = left_lane->duplicateOneCarParticle(p_car);

//                    // should check if the duplication succeeded.
//                    if (p_car->ghost_neighbor)
//                    {
//                        p_car->ghost_neighbor->lanechanging_state = 3;
//                        p_car->lanechanging_state = 1;
//                        p_car->normal_move = 0;
//                    }
//                }
//            }
//            else if (p_car->lanechanging_state == 1)
//            {
//                p_car->normal_move += delta_normal_move;

//                double pos = p_car->ghost_neighbor->position;
//                p_car->orientation = p_BezSequ->getTangentByXPosition(pos);
//                p_car->normal = p_BezSequ->getNormalByXPosition(pos);
//                p_car->visual_position = p_BezSequ->getPointByXPosition(pos) +
//                                         p_car->normal * (distance_bias + p_car->normal_move);

//                if (p_car->normal_move >= DEFAULT_LANE_WIDTH)
//                {
//                    p_car->ghost_neighbor->lanechanging_state = 0;

//                    delete p_car;
//                    car_list[i] = NULL;
//                }
//            }
//            else if (p_car->lanechanging_state == 2)
//            {
//                p_car->normal_move += delta_normal_move;

//                double pos = p_car->ghost_neighbor->position;
//                p_car->orientation = p_BezSequ->getTangentByXPosition(pos);
//                p_car->normal = p_BezSequ->getNormalByXPosition(pos);
//                p_car->visual_position = p_BezSequ->getPointByXPosition(pos) +
//                                         p_car->normal * (distance_bias - p_car->normal_move);

//                if (p_car->normal_move >= DEFAULT_LANE_WIDTH)
//                {
//                    p_car->ghost_neighbor->lanechanging_state = 0;

//                    delete p_car;
//                    car_list[i] = NULL;
//                }
//            }
//        }
//    }
//    else if (left_lane && left_lane->isRunning)
//    {
//        for (int i=0; i<num_cars; ++i)
//        {
//            CarParticle *p_car = car_list[i];

//            if (p_car->lanechanging_state == 0)
//            {
////                double left_velocity = left_lane->getSpeedAtPosition(p_car->position);
////                if (left_velocity - p_car->velocity > p_SysParam->LaneChangingVelocity)
//                double left_diff = p_car->density - left_lane->getDensityAtPosition(p_car->position);
//                if (left_diff > p_SysParam->LaneChangingDensity)
//                {
//                    p_car->ghost_neighbor = left_lane->duplicateOneCarParticle(p_car);

//                    // should check if the duplication succeeded.
//                    if (p_car->ghost_neighbor)
//                    {
//                        p_car->ghost_neighbor->lanechanging_state = 3;
//                        p_car->lanechanging_state = 1;
//                        p_car->normal_move = 0;
//                    }
//                }
//            }
//            else if (p_car->lanechanging_state == 1)
//            {
//                p_car->normal_move += delta_normal_move;

////                double pos = (p_car->position + p_car->ghost_neighbor->position)*0.5;
//                double pos = p_car->ghost_neighbor->position;
//                p_car->orientation = p_BezSequ->getTangentByXPosition(pos);
//                p_car->normal = p_BezSequ->getNormalByXPosition(pos);
//                p_car->visual_position = p_BezSequ->getPointByXPosition(pos) +
//                                         p_car->normal * (distance_bias + p_car->normal_move);

//                if (p_car->normal_move >= DEFAULT_LANE_WIDTH)
//                {
//                    p_car->ghost_neighbor->lanechanging_state = 0;

//                    delete p_car;
//                    car_list[i] = NULL;
//                }
//            }
//        }
//    }
//    else if (right_lane && right_lane->isRunning)
//    {
//        for (int i=0; i<num_cars; ++i)
//        {
//            CarParticle *p_car = car_list[i];

//            if (p_car->lanechanging_state == 0)
//            {
//                double right_diff = p_car->density - right_lane->getDensityAtPosition(p_car->position);
//                if (right_diff > p_SysParam->LaneChangingDensity)
//                {
//                    p_car->ghost_neighbor = right_lane->duplicateOneCarParticle(p_car);

//                    // should check if the duplication succeeded.
//                    if (p_car->ghost_neighbor)
//                    {
//                        p_car->ghost_neighbor->lanechanging_state = 3;
//                        p_car->lanechanging_state = 2;
//                        p_car->normal_move = 0;
//                    }
//                }
//            }
//            else if (p_car->lanechanging_state == 2)
//            {
//                p_car->normal_move += delta_normal_move;

//                double pos = p_car->ghost_neighbor->position;
//                p_car->orientation = p_BezSequ->getTangentByXPosition(pos);
//                p_car->normal = p_BezSequ->getNormalByXPosition(pos);
//                p_car->visual_position = p_BezSequ->getPointByXPosition(pos) +
//                                         p_car->normal * (distance_bias - p_car->normal_move);

//                if (p_car->normal_move >= DEFAULT_LANE_WIDTH)
//                {
//                    p_car->ghost_neighbor->lanechanging_state = 0;

//                    delete p_car;
//                    car_list[i] = NULL;
//                }
//            }
//        }
//    }

//    // remove the NULL car particles from the list
//    vector<CarParticle*> temp_list = car_list;
//    car_list.clear();
//    for (int i=0; i<num_cars; ++i)
//    {
//        CarParticle *p_car = temp_list[i];

//        if (p_car)
//        {
//            car_list.push_back(p_car);
//        }
//    }
//    num_cars = car_list.size();
}

void Road::duplicateCarParticleForLaneChanging(int lane_id, CarParticle *p_car, int dd)
{
    vector<CarParticle*> *car_list = &car_lists[lane_id];
    int num_car = car_list->size();
    if (num_car == 0)
    {
        // create the lanechanging target car
        CarParticle *p_car_new = p_car->duplicate();
        p_car_new->id = max_carID++;
        p_car_new->road_id = id;
        p_car_new->lane_id = lane_id;
        p_car_new->lanechanging_status = 2;  // this is lanechanging in.
        p_car_new->lanechanging_percentage = 0;
        p_car_new->position = road_curve.getPointByXPosition(p_car_new->x_position) +
                              p_car_new->normal * bias_distances[lane_id];

        // push the new car to the target lane
        car_list->push_back(p_car_new);

        // update the current car
        p_car->ghost_neighbor = p_car_new;
        p_car->lanechanging_status = 1;  // this is lanechanging out.
        p_car->lanechanging_percentage = 0;
        p_car->lanechanging_direction = dd;

        return;
    }
    else
    {
        CarParticle *p_car_0 = car_list->at(0);  // the first car in the list
        CarParticle *p_car_1 = car_list->at(num_car-1);  // the last car in the list

        if (p_car->x_position >= p_car_0->x_position && p_car->isSafeToNextCar(p_car_0))
        {
            // create the lanechanging target car
            CarParticle *p_car_new = p_car->duplicate();
            p_car_new->id = max_carID++;
            p_car_new->road_id = id;
            p_car_new->lane_id = lane_id;
            p_car_new->lanechanging_status = 2;  // this is lanechanging in.
            p_car_new->lanechanging_percentage = 0;
            p_car_new->position = road_curve.getPointByXPosition(p_car_new->x_position) +
                                  p_car_new->normal * bias_distances[lane_id];

            // insert a new car to the head of the car list
            car_list->push_back(NULL);
            for (int i=num_car; i>0; i--)
            {
                car_list->at(i) = car_list->at(i-1);
            }
            car_list->at(0) = p_car_new;

            // update the current car
            p_car->ghost_neighbor = p_car_new;
            p_car->lanechanging_status = 1;  // this is lanechanging out.
            p_car->lanechanging_percentage = 0;
            p_car->lanechanging_direction = dd;

            return;
        }

        else if (p_car->x_position <= p_car_1->x_position && p_car->isSafeToPreviousCar(p_car_1))
        {
            // create the lanechanging target car
            CarParticle *p_car_new = p_car->duplicate();
            p_car_new->id = max_carID++;
            p_car_new->road_id = id;
            p_car_new->lane_id = lane_id;
            p_car_new->lanechanging_status = 2;  // this is lanechanging in.
            p_car_new->lanechanging_percentage = 0;
            p_car_new->position = road_curve.getPointByXPosition(p_car_new->x_position) +
                                  p_car_new->normal * bias_distances[lane_id];

            // push the new car to the tail of the car list
            car_list->push_back(p_car_new);

            // update the current car
            p_car->ghost_neighbor = p_car_new;
            p_car->lanechanging_status = 1;  // this is lanechanging out.
            p_car->lanechanging_percentage = 0;
            p_car->lanechanging_direction = dd;

            return;
        }

        else
        {
            for (int i=1; i<num_car; ++i)
            {
                p_car_0 = car_list->at(i-1);
                p_car_1 = car_list->at(i);

                if (p_car->x_position <= p_car_0->x_position &&
                    p_car->x_position >= p_car_1->x_position &&
                    p_car->isSafeToPreviousCar(p_car_0) && p_car->isSafeToNextCar(p_car_1))
                {
                    // create the lanechanging target car
                    CarParticle *p_car_new = p_car->duplicate();
                    p_car_new->id = max_carID++;
                    p_car_new->road_id = id;
                    p_car_new->lane_id = lane_id;
                    p_car_new->lanechanging_status = 2;  // this is lanechanging in.
                    p_car_new->lanechanging_percentage = 0;
                    p_car_new->position = road_curve.getPointByXPosition(p_car_new->x_position) +
                            p_car_new->normal * bias_distances[lane_id];

                    // insert the new car to the proper location of the car list
                    car_list->push_back(NULL);
                    for (int k=num_car; k>i; k--)
                    {
                        car_list->at(k) = car_list->at(k-1);
                    }
                    car_list->at(i) = p_car_new;

                    // update the current car
                    p_car->ghost_neighbor = p_car_new;
                    p_car->lanechanging_status = 1;  // this is lanechanging out.
                    p_car->lanechanging_percentage = 0;
                    p_car->lanechanging_direction = dd;

                    return;
                }
            }
        }
    }
}

void Road::cleanParticles()
{
    for (int k=0; k<num_lanes; ++k)
    {
        vector<Particle*> *part_list = &particle_lists[k];
        part_list->clear();
    }
    num_particles = 0;
    max_particleID = 0;
}

void Road::cleanCars()
{
    for (int k=0; k<num_lanes; ++k)
    {
        vector<CarParticle*> *car_list = &car_lists[k];
        car_list->clear();
    }
    num_cars = 0;
    max_carID = 0;
}

