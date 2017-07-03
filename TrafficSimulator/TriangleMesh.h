#ifndef TRIANGLEMESH_H
#define TRIANGLEMESH_H

#include "global.h"

class Node
{
public:
    Node(){}

    int id;

    zVec3 norm;

    zVec3 original_position;
    zVec3 position;

    // assistant parameter, range in 0.0 ~ 1.0
    // used to indicate a one-dimension coordinate mapped.
    double x_position, x_mapped;

    double density;
    double velocity;
    double normalized_density;
    double normalized_velocity;

    int color_id; // for some visual node like observation point
    zVec4 color; // for some visual node like observation point
    bool isBad;
    int road_id;
    int lane_id;
    int cell_id;
};

class Triangle
{
public:
    Triangle(){}

    int index;
    int idx0, idx1, idx2; //the indices of the three nodes.

    zVec3 norm;

    Node* v0;
    Node* v1;
    Node* v2;
};

class TriangleMesh
{
public:
    TriangleMesh();

    // The full mesh
    vector<Node*> node_list; //It will be more flexible to use pointers than instances.
    vector<Triangle> triangle_list;

    // The sketch mesh, i.e. a box
    vector<Node*> sketch_node_list;
    vector<Triangle> sketch_triangle_list;

    zVec3 min_corner, max_corner;
    double length, width, height; // corresponding to x y z dimensions.

    // load the original mesh file
    bool loadMeshFile(const char *filename);

    // the original mesh is translated to the origin,
    // then scale/rotate to an appropriate size/orientation.
    // This is its original pose.
    void setOriginalMesh(double scale_, double theta_0, double theta_1, double theta_2);

    // compute the normal of each triangle and vertex
    void computeNormal();

    // restore the mesh to their original pose
    void resetToOriginalMesh();

    // translate/scale/rotation the mesh
    void translate(double xx, double yy, double zz);
    void scale(double factor);
    void rotate(int axis, double theta);

    // Given the node list, build the triangle mesh, two nodes as a group
    void buildMeshTwoNodesPerGroup();

    // Given the node list, build the triangle mesh, taking 'num_nodes_per_group' nodes as a group
    void buildMeshByNodesGroup(int offset, int num_nodes_per_group);

    // Generate faces from the two most recent columns of the node list
    void buildMeshByRecentTwoColumnNodes(int column_0, int column_1, double dist);

    // Generate a circle mesh, i.e. a polygon.
    void buildMeshAsOneCircle(double radius, double num_edges);

    // Generate the sketch mesh.
    void buildSketchMesh();
};

#endif // TRIANGLEMESH_H
