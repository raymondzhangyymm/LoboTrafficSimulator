#ifndef MAPPARSER_H
#define MAPPARSER_H

#include "global.h"
#include "Bezier.h"
#include "tinyxml2.h"

struct RoadNode
{
    // the original data from maps
    long long id;
    string timestamp;
    double longitude;
    double latitude;
    bool isPosition; // true for makeup nodes
    //vector<string> tag;

    // the information converted
    zVec2 position;
    bool isVisited;
};

struct RoadLink
{
    // the original data from maps
    long long id;
    string timestamp;
    vector<RoadNode*> roadnode_list;
    vector<string> tag;

    // the information converted
    int index;
    int lanes;
    bool isVisible;
    bool isOneway;
    MapElementType link_type;  // "beziers" means it is a makeup road.
    HighwayType highway_type;

    // the source for two direction
    bool InflowSwitch[2];
    double InflowRate[2];
};

class MapParser
{
public:
    MapParser();

    // the original map data loaded
    zVec2 map_mincorner, map_maxcorner, map_center; // longitude/latitude pairs
    bool boundsIsPosition;
    vector<RoadNode*> roadnode_list;
    vector<RoadLink*> roadlink_list;

    // the result of parsing the road network
    zVec2 min_corner, max_corner, center;

    // the breaking threshold of the cosine value of two consecutive polyline
    double breaking_threshold;

    bool loadByTinyXML2(const char *filename);

    // convert timestamp to seconds from the reference time
    zVec2 convertLongitudeLatitudeToXY(zVec2 ll);
    zVec2 convertLongitudeLatitudeToXY(double longitude, double latitude);

private:
    // quicksort the road nodes
    void quicksortByID(vector<RoadNode*> &nodelist, int p, int r);
    int partitionByID(vector<RoadNode*> &nodelist, int p, int r);
    int binarysearchByID(vector<RoadNode*> &nodelist, long long id_);

    // determine attributes of links
    void determineRoadLinkAttributes();

    // generate beziers according to the original road link
    void refineRoadNetWork();

    // remove very short polylines in roadlinks, then
    // remove the single-node roadlink.
    void removeShortRoadLinks();

    // breaking consecutive polylines with sharp-angle
    void breakRoadLinks();
    RoadLink* buildOneRoadLink(RoadLink *old_link, int start_nodeid, int end_nodeid);

    void clearRoadNodes();
    void clearRoadLinks();

    int getNumberOfPolylinesOfRoadLinks();

    // convert timestamp to seconds from the reference time
    time_t reference_time;
    double convertTimestamp(const string &str);
};

#endif // MAPPARSER_H
