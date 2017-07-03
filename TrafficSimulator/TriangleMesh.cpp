#include "TriangleMesh.h"

TriangleMesh::TriangleMesh()
{

}

bool TriangleMesh::loadMeshFile(const char *filename)
{
    // open the obj file.
    ifstream ifs(filename);
    if (ifs.fail())
    {
        cout << "Opening mesh file failed. " << filename << endl;
        return false;
    }

    node_list.clear();
    triangle_list.clear();

    char line[1024];
    char* token;
    char vert[] = "v";
    char face[] = "f";

    int node_count = 0;
    int face_count = 0;

    while (!ifs.eof())
    {
        ifs.getline(line, 1024);
        if (strlen(line) == 0)
        {
            continue;
        }
//        cout << "line " << line << endl;

        token = strtok(line, " ");

        // a line of vertex
        if (strcmp(token, vert) == 0 )
        {
            Node *p_node = new Node;

            token = strtok(NULL, " ");
            p_node->original_position.x = atof(token);
            token = strtok(NULL, " ");
            p_node->original_position.y = atof(token);
            token = strtok(NULL, " ");
            p_node->original_position.z = atof(token);

            p_node->id = node_count++;
            node_list.push_back(p_node);
        }

        // a line of face
        else if (strcmp (token, face) == 0)
        {
            Triangle tri;

            //Generate a triangle face then add to the tail of the face list.
            //A face contains 3 index of the node_list.
            token = strtok(NULL, " ");
            tri.idx0 = atoi(token) - 1;
            token = strtok(NULL, " ");
            tri.idx1 = atoi(token) - 1;
            token = strtok(NULL, " ");
            tri.idx2 = atoi(token) - 1;

            tri.index = face_count++;
            triangle_list.push_back(tri);

//            cout << "f " << tri.idx0 << " " << tri.idx1 << " " << tri.idx2 << endl;

            //If the face is a polygon, keep getting the rest indices
            //and convert to a couple of triangles.
            int idx0 = tri.idx0;
            int idx1 = tri.idx2;
            while ((token = strtok(NULL, " \r\n")) != NULL)
            {
                int idx2 = atoi(token) - 1;

                tri.idx0 = idx0;
                tri.idx1 = idx1;
                tri.idx2 = idx2;

                tri.index = face_count++;
                triangle_list.push_back(tri);

                idx1 = idx2;
            }
        }
    }
    ifs.close();

    //Point 3 node-pointers in the face to the coresponding nodes in the node_list.
    for (vector<Triangle>::iterator fi = triangle_list.begin();
                                         fi != triangle_list.end(); ++fi)
    {
        (*fi).v0 = node_list[(*fi).idx0];
        (*fi).v1 = node_list[(*fi).idx1];
        (*fi).v2 = node_list[(*fi).idx2];
    }

//    cout << "Load mesh file '" << filename << " successfully. "
//         << "#node:" << node_list.size() << ", #triangle face:" << triangle_list.size() << endl;

    return true;
}

void TriangleMesh::setOriginalMesh(double scale_, double theta_0, double theta_1, double theta_2)
{
    double costheta_0 = cos(theta_0);
    double sintheta_0 = sin(theta_0);
    double costheta_1 = cos(theta_1);
    double sintheta_1 = sin(theta_1);
    double costheta_2 = cos(theta_2);
    double sintheta_2 = sin(theta_2);

    int size = node_list.size();
    for (int i=0; i<size; ++i)
    {
        Node *p_node = node_list[i];
        zVec3 pos = p_node->original_position;

        // scale
        pos *= scale_;

//        // rotate around x_axis
//        double alpha = atan2(pos.z, pos.y);
//        double length = sqrt(pos.y*pos.y + pos.z*pos.z);

//        pos.y = length * cos(alpha+theta_0);
//        pos.z = length * sin(alpha+theta_0);

//        // rotate around y_axis
//        alpha = atan2(pos.x, pos.z);
//        length = sqrt(pos.z*pos.z + pos.x*pos.x);

//        pos.z = length * cos(alpha+theta_1);
//        pos.x = length * sin(alpha+theta_1);

//        // rotate around z_axis
//        alpha = atan2(pos.y, pos.x);
//        length = sqrt(pos.x*pos.x + pos.y*pos.y);

//        pos.x = length * cos(alpha+theta_2);
//        pos.y = length * sin(alpha+theta_2);

        // rotate around x_axis
        zVec3 temp = pos;
        temp.y = costheta_0 * pos.y - sintheta_0 * pos.z;
        temp.z = sintheta_0 * pos.y + costheta_0 * pos.z;

        // rotate around y_axis
        pos = temp;
        temp.z = costheta_1 * pos.z - sintheta_1 * pos.x;
        temp.x = sintheta_1 * pos.z + costheta_1 * pos.x;

        // rotate around z_axis
        pos = temp;
        temp.x = costheta_2 * pos.x - sintheta_2 * pos.y;
        temp.y = sintheta_2 * pos.x + costheta_2 * pos.y;

        p_node->original_position = temp;
        p_node->position = p_node->original_position;
    }

    computeNormal();

    // find the max and min coordinates of the mesh
    double max_x = node_list[0]->position.x;
    double max_y = node_list[0]->position.y;
    double max_z = node_list[0]->position.z;
    double min_x = max_x;
    double min_y = max_y;
    double min_z = max_z;

    for (int i=0; i<size; ++i)
    {
        zVec3 pos = node_list[i]->position;

        if (max_x < pos.x)
        {
            max_x = pos.x;
        }
        if (min_x > pos.x)
        {
            min_x = pos.x;
        }

        if (max_y < pos.y)
        {
            max_y = pos.y;
        }
        if (min_y > pos.y)
        {
            min_y = pos.y;
        }

        if (max_z < pos.z)
        {
            max_z = pos.z;
        }
        if (min_z > pos.z)
        {
            min_z = pos.z;
        }
    }

    // determine the bounding box
    min_corner.x = min_x;
    min_corner.y = min_y;
    min_corner.z = min_z;
    max_corner.x = max_x;
    max_corner.y = max_y;
    max_corner.z = max_z;
    length = max_x - min_x;
    width = max_y - min_y;
    height = max_z - min_z;

    // translate the center of the mesh to the origin
    zVec3 center((max_x+min_x)*0.5, (max_y+min_y)*0.5, (max_z+min_z)*0.5);
    for (int i=0; i<size; ++i)
    {
        Node *p_node = node_list[i];
        p_node->original_position -= center;

        p_node->original_position[2] += height*0.5;
        p_node->position = p_node->original_position;
    }
}

void TriangleMesh::computeNormal()
{
    // Clear the norm of each vertex
    int size = node_list.size();
    for (int i=0; i<size; ++i)
    {
        Node *p_node = node_list[i];
        p_node->norm.clear();
    }

    // Compute the new normal
    int num_tri = triangle_list.size();
    for (int i=0; i<num_tri; ++i)
    {
        Triangle *p_tri = &triangle_list[i];

        zVec3 nl = zVec3::normalOfTriangle
                   (p_tri->v0->position, p_tri->v1->position, p_tri->v2->position);

        // the normal of the triangle for flat shading
        p_tri->norm = nl;

        // the average normal of vertex for gouraud shading
        p_tri->v0->norm += nl;
        p_tri->v0->norm = zVec3::normalize(p_tri->v0->norm);

        p_tri->v1->norm += nl;
        p_tri->v1->norm = zVec3::normalize(p_tri->v1->norm);

        p_tri->v2->norm += nl;
        p_tri->v2->norm = zVec3::normalize(p_tri->v2->norm);
    }
}

void TriangleMesh::resetToOriginalMesh()
{
    int size = node_list.size();
    for (int i=0; i<size; ++i)
    {
        Node *p_node = node_list[i];
        p_node->position = p_node->original_position;
    }
}

void TriangleMesh::scale(double factor)
{
    int size = node_list.size();
    for (int i=0; i<size; ++i)
    {
        Node *p_node = node_list[i];
        p_node->position *= factor;
    }
}

void TriangleMesh::rotate(int axis, double theta)
{
//    int axis_1 = (axis+1)%3;
//    int axis_2 = (axis+2)%3;

//    int size = node_list.size();
//    for (int i=0; i<size; ++i)
//    {
//        Node *p_node = &node_list[i];
//        zVec3 pos = p_node->position;

//        double alpha = atan2(pos[axis_2], pos[axis_1]);
//        double length = sqrt(pos[axis_1]*pos[axis_1] + pos[axis_2]*pos[axis_2]);

//        p_node->position[axis_1] = length * cos(alpha+theta);
//        p_node->position[axis_2] = length * sin(alpha+theta);
//    }

    int axis_1 = (axis+1)%3;
    int axis_2 = (axis+2)%3;

    double costheta = cos(theta);
    double sintheta = sin(theta);

    int size = node_list.size();
    for (int i=0; i<size; ++i)
    {
        Node *p_node = node_list[i];
        zVec3 pos = p_node->position;
        p_node->position[axis_1] = costheta * pos[axis_1] - sintheta * pos[axis_2];
        p_node->position[axis_2] = sintheta * pos[axis_1] + costheta * pos[axis_2];
    }
}

void TriangleMesh::buildMeshTwoNodesPerGroup()
{
    triangle_list.clear();

    int size = node_list.size();
    for (int i=2; i<size; i+=2)
    {
        Triangle tri;
        tri.v0 = node_list[i-2];
        tri.v1 = node_list[i-1];
        tri.v2 = node_list[i];
        triangle_list.push_back(tri);

        tri.v0 = node_list[i-1];
        tri.v1 = node_list[i+1];
        tri.v2 = node_list[i];
        triangle_list.push_back(tri);
    }
}

void TriangleMesh::buildMeshByNodesGroup(int offset, int num_nodes_per_group)
{
    // build only the last offset nodes.
    int start_node = node_list.size() - offset;
    int size = node_list.size();
    for (int i=start_node+num_nodes_per_group; i<size; i+=num_nodes_per_group)
    {
        int col_0 = i - num_nodes_per_group;
        int col_1 = i;

        for (int j=1; j<num_nodes_per_group; ++j)
        {
            Triangle tri;
            tri.v0 = node_list[col_0 + j - 1];
            tri.v1 = node_list[col_0 + j];
            tri.v2 = node_list[col_1 + j - 1];
            triangle_list.push_back(tri);

            tri.v0 = node_list[col_0 + j];
            tri.v1 = node_list[col_1 + j];
            tri.v2 = node_list[col_1 + j - 1];
            triangle_list.push_back(tri);
        }
    }
}

void TriangleMesh::buildMeshByRecentTwoColumnNodes(int column_0, int column_1, double dist)
{
    if (column_0 <= 0 || column_1 <= 0)
    {
        return;
    }

    int offset_p = node_list.size() - column_0 - column_1;
    int offset_q = offset_p + column_0;

    dist *= 1.1;

    //-----------------------------------------------------------------
    // first loop deals with the quad face.
    //-----------------------------------------------------------------
    for (int i_p=1; i_p<column_0; ++i_p)
    {
        Node* p0 = node_list[offset_p + (i_p-1)];
        Node* p1 = node_list[offset_p + i_p];

        for (int i_q=1; i_q<column_1; ++i_q)
        {
            Node* q0 = node_list[offset_q + (i_q-1)];
            Node* q1 = node_list[offset_q + i_q];

            if (abs(p1->position.y - q1->position.y) <= 1e-6 &&
                abs(p0->position.y - p1->position.y) <= dist &&
                abs(q0->position.y - q1->position.y) <= dist)
            {
                // face p0q0q1
                Triangle tri;
                tri.v0 = p0;
                tri.v1 = q0;
                tri.v2 = q1;
                triangle_list.push_back(tri);

                // face p0q1p1
                tri.v0 = p0;
                tri.v1 = q1;
                tri.v2 = p1;
                triangle_list.push_back(tri);
            }
        }
    }

    //---------------------------------------------------------------------------------
    // second loop deals with the edge triangle face with one vetex in col 0.
    //---------------------------------------------------------------------------------
    // find the isolated visual particles without previously adjacent neighbor in col 0
    vector<Node*> isolated_nodes;
    isolated_nodes.clear();
    isolated_nodes.push_back(node_list[offset_p]);
    for (int i_p=1; i_p<column_0; ++i_p)
    {
        Node* p0 = node_list[offset_p + (i_p-1)];
        Node* p1 = node_list[offset_p + i_p];
        if (abs(p0->position.y - p1->position.y) > dist)
        {
            isolated_nodes.push_back(p1);
        }
    }

    // connect the isolated particle to the col 1 to get faces.
    int size = isolated_nodes.size();
    for (int k=0; k<size; ++k)
    {
        Node* p1 = isolated_nodes[k];

        for (int i_q=1; i_q<column_1; ++i_q)
        {
            Node* q0 = node_list[offset_q + (i_q-1)];
            Node* q1 = node_list[offset_q + i_q];

            if (abs(p1->position.y - q1->position.y) <= 1e-6 &&
                abs(q0->position.y - q1->position.y) <= dist)
            {
                Triangle tri;
                tri.v0 = p1;
                tri.v1 = q0;
                tri.v2 = q1;
                triangle_list.push_back(tri);
            }
        }
    }

    // find the isolated visual particles without next adjaent neighbor in col 0
    isolated_nodes.clear();
    for (int i_p=0; i_p<column_0-1; ++i_p)
    {
        Node* p0 = node_list[offset_p + i_p];
        Node* p1 = node_list[offset_p + (i_p+1)];
        if (abs(p0->position.y - p1->position.y) > dist)
        {
            isolated_nodes.push_back(p0);
        }
    }
    isolated_nodes.push_back(node_list[offset_p + column_0-1]);

    // connect the isolated particle to the col 1 to get faces.
    size = isolated_nodes.size();
    for (int k=0; k<size; ++k)
    {
        Node* p1 = isolated_nodes[k];

        for (int i_q=1; i_q<column_1; ++i_q)
        {
            Node* q0 = node_list[offset_q + (i_q-1)];
            Node* q1 = node_list[offset_q + i_q];

            if (abs(p1->position.y - q0->position.y) <= 1e-6 &&
                abs(q0->position.y - q1->position.y) <= dist)
            {
                Triangle tri;
                tri.v0 = p1;
                tri.v1 = q0;
                tri.v2 = q1;
                triangle_list.push_back(tri);
            }
        }
    }

    //---------------------------------------------------------------------------------
    // third loop deals with the edge triangle face with one vetex in col 1.
    //---------------------------------------------------------------------------------
    // find the isolated visual particles without previously adjaent neighbor in col 1
    isolated_nodes.clear();
    isolated_nodes.push_back(node_list[offset_q]);
    for (int i_q=1; i_q<column_1; ++i_q)
    {
        Node* q0 = node_list[offset_q + (i_q-1)];
        Node* q1 = node_list[offset_q + i_q];
        if (abs(q0->position.y - q1->position.y) > dist)
        {
            isolated_nodes.push_back(q1);
        }
    }

    // connect the isolated particle to the col 1 to get faces.
    size = isolated_nodes.size();
    for (int k=0; k<size; ++k)
    {
        Node* q1 = isolated_nodes[k];

        for (int i_p=1; i_p<column_0; ++i_p)
        {
            Node* p0 = node_list[offset_p + (i_p-1)];
            Node* p1 = node_list[offset_p + i_p];

            if (abs(q1->position.y - p1->position.y) <= 1e-6 &&
                abs(p0->position.y - p1->position.y) <= dist)
            {
                Triangle tri;
                tri.v0 = p0;
                tri.v1 = q1;
                tri.v2 = p1;
                triangle_list.push_back(tri);
            }
        }
    }

    // find the isolated visual particles without next adjaent neighbor in col 1
    isolated_nodes.clear();
    for (int i_q=0; i_q<column_1-1; ++i_q)
    {
        Node* q0 = node_list[offset_q + i_q];
        Node* q1 = node_list[offset_q + (i_q+1)];
        if (abs(q0->position.y - q1->position.y) > dist)
        {
            isolated_nodes.push_back(q0);
        }
    }
    isolated_nodes.push_back(node_list[offset_q + column_1-1]);

    // connect the isolated particle to the col 1 to get faces.
    size = isolated_nodes.size();
    for (int k=0; k<size; ++k)
    {
        Node* q1 = isolated_nodes[k];

        for (int i_p=1; i_p<column_0; ++i_p)
        {
            Node* p0 = node_list[offset_p + (i_p-1)];
            Node* p1 = node_list[offset_p + i_p];

            if (abs(q1->position.y - p0->position.y) <= 1e-6 &&
                abs(p0->position.y - p1->position.y) <= dist)
            {
                Triangle tri;
                tri.v0 = p0;
                tri.v1 = q1;
                tri.v2 = p1;
                triangle_list.push_back(tri);
            }
        }
    }
}

void TriangleMesh::buildMeshAsOneCircle(double radius, double num_edges)
{
    if (num_edges < 3)
    {
        return;
    }

    // The first node[0] is the center(0,0).
    node_list.resize(num_edges+2);
    Node *p_node = new Node;
    p_node->position.clear();
    node_list[0] = p_node;

    double angle = 0;
    double angle_step = 2*PI / double(num_edges);

    for (int i=1; i<num_edges+1; ++i)
    {
        p_node = new Node;
        p_node->position.x = radius * cos(angle);
        p_node->position.y = radius * sin(angle);
        node_list[i] = p_node;

        angle += angle_step;
    }
    // The last node[num_edges+1] is identical to the second node[1].
    p_node = new Node;
    *p_node = *node_list[1];
    node_list[num_edges+1] = p_node;

    triangle_list.resize(num_edges);
    int size = node_list.size();
    for (int i=2; i<size; ++i)
    {
        Triangle tri;
        tri.v0 = node_list[0];
        tri.v1 = node_list[i-1];
        tri.v2 = node_list[i];
        triangle_list.push_back(tri);
    }
}

void TriangleMesh::buildSketchMesh()
{



}
