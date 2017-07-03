#include "MapParser.h"

MapParser::MapParser()
{
    breaking_threshold = -1.0;

    // set the reference time to 2000-1-1 00:00:00
    struct tm rt = {0};
    rt.tm_year = 100; rt.tm_mon = 0; rt.tm_mday = 1;
    rt.tm_hour = 0;   rt.tm_min = 0; rt.tm_sec = 0;
    reference_time = mktime(&rt);
}

bool MapParser::loadByTinyXML2(const char *filename)
{
    QElapsedTimer loadmap_timer;

    tinyxml2::XMLDocument doc;
    doc.LoadFile(filename);
    if (doc.ErrorID())
    {
        cout << "Opening map file " << filename << " in tinyxml2 failed!\n" << std::flush;
        return false;
    }

    clearRoadNodes();
    clearRoadLinks();

    // Get the bounds
    tinyxml2::XMLElement* ele = doc.RootElement()->FirstChildElement("bounds");
    if (ele)
    {
        map_mincorner.x = ele->DoubleAttribute("minlon");
        map_mincorner.y = ele->DoubleAttribute("minlat");
        map_maxcorner.x = ele->DoubleAttribute("maxlon");
        map_maxcorner.y = ele->DoubleAttribute("maxlat");

        boundsIsPosition = false;
        ele->QueryBoolAttribute("isPosition", &boundsIsPosition);
    }
    else
    {
        return false;
    }
    map_center = (map_mincorner + map_maxcorner) / 2.0;

    // Get all road nodes.
    loadmap_timer.start();
    ele = doc.RootElement()->FirstChildElement("node");
    while (ele)
    {
        RoadNode *p_roadnode = new RoadNode;
        p_roadnode->id = ele->Int64Attribute("id");
        p_roadnode->timestamp = string(ele->Attribute("timestamp"));
        p_roadnode->longitude = ele->DoubleAttribute("lon");
        p_roadnode->latitude = ele->DoubleAttribute("lat");

        p_roadnode->isPosition = false;
        ele->QueryBoolAttribute("isPosition", &p_roadnode->isPosition);

//        // build the tag list for the road node
//        tinyxml2::XMLElement* ele_tag = ele->FirstChildElement("tag");
//        while (ele_tag)
//        {
//            p_roadnode->tag.push_back(string(ele_tag->Attribute("k")));
//            p_roadnode->tag.push_back(string(ele_tag->Attribute("v")));

//            // move to next "tag"
//            ele_tag = ele_tag->NextSiblingElement("tag");
//        }

        roadnode_list.push_back(p_roadnode);

        // move to next "node"
        ele = ele->NextSiblingElement("node");
    }
    cout << "Getting nodes takes " << loadmap_timer.nsecsElapsed()/1e9
         << " seconds.\n" << std::flush;

    // Sort the road nodes by their ids
    loadmap_timer.start();
    quicksortByID(roadnode_list, 0, roadnode_list.size()-1);
    cout << "Quicksorting nodes takes " << loadmap_timer.nsecsElapsed()/1e9
         << " seconds.\n" << std::flush;
//    int num_roadnode = roadnode_list.size();
//    for (int i=0; i<num_roadnode; ++i)
//    {
//        RoadNode *p_roadnode = roadnode_list[i];
//        cout << i << " " << p_roadnode->id << endl;
//    }

    // Get all road links.
    loadmap_timer.start();
    ele = doc.RootElement()->FirstChildElement("way");
    while (ele)
    {
        RoadLink *p_roadlink = new RoadLink;
        p_roadlink->id = ele->Int64Attribute("id");
        p_roadlink->timestamp = string(ele->Attribute("timestamp"));

        // build the node list for the road link
        tinyxml2::XMLElement* ele_nd = ele->FirstChildElement("nd");
        while (ele_nd)
        {
            long long nd_ref = ele_nd->Int64Attribute("ref");

            int index = binarysearchByID(roadnode_list, nd_ref);
            if (index != -1)
            {
                p_roadlink->roadnode_list.push_back(roadnode_list[index]);
            }
            else
            {
                cout << "Warning! Binary searching failed when loading maps!" << endl;
            }

            // move to next "nd"
            ele_nd = ele_nd->NextSiblingElement("nd");
        }

        // build the tag list for the road link
        tinyxml2::XMLElement* ele_tag = ele->FirstChildElement("tag");
        while (ele_tag)
        {
            p_roadlink->tag.push_back(string(ele_tag->Attribute("k")));
            p_roadlink->tag.push_back(string(ele_tag->Attribute("v")));

            // move to next "tag"
            ele_tag = ele_tag->NextSiblingElement("tag");
        }

        roadlink_list.push_back(p_roadlink);

        // move to next "way"
        ele = ele->NextSiblingElement("way");
    }
    cout << "Getting ways takes " << loadmap_timer.nsecsElapsed()/1e9 << " seconds.\n" << std::flush;

    loadmap_timer.start();
    refineRoadNetWork();
    cout << "Refining the map takes " << loadmap_timer.nsecsElapsed()/1e9 << " seconds.\n" << std::flush;

    cout << "Nodes " << roadnode_list.size()  << ", Ways " << roadlink_list.size()
         << ", Total Polylines " << getNumberOfPolylinesOfRoadLinks() << endl;

    determineRoadLinkAttributes();
    int count_building = 0;
    int count_railway = 0;
    int count_others = 0;
    int count_highway = 0;
    int highway_primary = 0, highway_primary_link = 0;
    int highway_trunk = 0, highway_trunk_link = 0;
    int highway_motorway = 0, highway_motorway_link = 0;
    int highway_secondary = 0, highway_tertiary = 0;
    int highway_residential = 0, highway_otherhighways = 0;
    int highway_footway = 0, highway_unclassified = 0;
    int highway_service = 0, highway_path = 0;
    int num_road = roadlink_list.size();
    for (int i=0; i<num_road; ++i)
    {
        RoadLink *p_roadlink = roadlink_list[i];

        if (p_roadlink->link_type == highway)
        {
            count_highway++;
            if (p_roadlink->highway_type == primary)
            {
                highway_primary++;
            }
            else if (p_roadlink->highway_type == primary_link)
            {
                highway_primary_link++;
            }
            else if (p_roadlink->highway_type == secondary)
            {
                highway_secondary++;
            }
            else if (p_roadlink->highway_type == residential)
            {
                highway_residential++;
            }
            else if (p_roadlink->highway_type == tertiary)
            {
                highway_tertiary++;
            }
            else if (p_roadlink->highway_type == motorway)
            {
                highway_motorway++;
            }
            else if (p_roadlink->highway_type == motorway_link)
            {
                highway_motorway_link++;
            }
            else if (p_roadlink->highway_type == trunk)
            {
                highway_trunk++;
            }
            else if (p_roadlink->highway_type == trunk_link)
            {
                highway_trunk_link++;
            }

            else if (p_roadlink->highway_type == footway)
            {
                highway_footway++;
            }
            else if (p_roadlink->highway_type == unclassified)
            {
                highway_unclassified++;
            }
            else if (p_roadlink->highway_type == service)
            {
                highway_service++;
            }
            else if (p_roadlink->highway_type == path)
            {
                highway_path++;
            }

            else
            {
                highway_otherhighways++;
            }
        }
        else if (p_roadlink->link_type == building)
        {
            count_building++;
        }
        else if (p_roadlink->link_type == railway)
        {
            count_railway++;
        }
        else
        {
            count_others++;
        }
    }
    cout << "Highways " << count_highway << ", buildings " << count_building
         << ", railways " << count_railway << ", others " << count_others << endl << std::flush;
    cout << "In highways,"
         << " trunk " << highway_trunk << ", trunk_link " << highway_trunk_link
         << ", motorway " << highway_motorway << ", motorway_link " << highway_motorway_link
         << ", primary " << highway_primary << ", primary_link " << highway_primary_link
         << ", secondary " << highway_secondary << ", tertiary " << highway_tertiary
         << ", residential " << highway_residential << ", service " << highway_service
         << ", footway " << highway_footway << ", unclassified " << highway_unclassified
         << ", path " << highway_path << ", others " << highway_otherhighways << endl << std::flush;

    // Optionally, scale the beziers
    for (int i=0; i<num_road; ++i)
    {
        RoadLink *p_roadlink = roadlink_list[i];

        if (p_roadlink->link_type == beziers)
        {
            // Check the tags and determine the scaling.
            double ss = 1.0;  // default scaling
            int num_tag = p_roadlink->tag.size();
            for (int t=0; t<num_tag; t+=2)
            {
                string k = p_roadlink->tag[t];
                string v = p_roadlink->tag[t+1];
                if (k == "scaling")
                {
                    ss = atoi(v.c_str());
                    break;
                }
            }

            // Set the nodes unvisited
            int num_point = p_roadlink->roadnode_list.size();
            for (int k=0; k<num_point; ++k)
            {
                RoadNode *p_roadnode = p_roadlink->roadnode_list[k];
                p_roadnode->isVisited = false;
            }

            // Scale the nodes
            for (int k=0; k<num_point; ++k)
            {
                RoadNode *p_roadnode = p_roadlink->roadnode_list[k];
                if (!p_roadnode->isVisited)
                {
                    p_roadnode->position *= ss;
                    p_roadnode->isVisited = true;

                    // Update the big bounding box
                    zVec2 pp = p_roadnode->position;
                    if (min_corner.x > pp.x)
                    {
                        min_corner.x = pp.x;
                    }
                    if (min_corner.y > pp.y)
                    {
                        min_corner.y = pp.y;
                    }
                    if (max_corner.x < pp.x)
                    {
                        max_corner.x = pp.x;
                    }
                    if (max_corner.y < pp.y)
                    {
                        max_corner.y = pp.y;
                    }
                }
            }
        }
    }

    cout << "The map area is " << (max_corner.x-min_corner.x) << " by " << (max_corner.y-min_corner.y)
         << " m.\n" << std::flush;

    return true;
}

void MapParser::determineRoadLinkAttributes()
{
    int num_road = roadlink_list.size();
    for (int i=0; i<num_road; ++i)
    {
        RoadLink *p_roadlink = roadlink_list[i];

        // Assign indices for road links.
        p_roadlink->index = i;

        // Default attributes if not specified.
        p_roadlink->link_type = otherlinks;
        p_roadlink->isVisible = true;
        p_roadlink->isOneway = false;
        p_roadlink->lanes = 2;

        // Default boundary conditions (sources)
        p_roadlink->InflowSwitch[0] = true;
        p_roadlink->InflowSwitch[1] = true;
        p_roadlink->InflowRate[0] = 0.5; // vehicles per lane per second
        p_roadlink->InflowRate[1] = 0.5; // vehicles per lane per second

        // Check the tags.
        int num_tag = p_roadlink->tag.size();
        for (int t=0; t<num_tag; t+=2)
        {
            string k = p_roadlink->tag[t];
            string v = p_roadlink->tag[t+1];

            // Determine the type of the link
            if (k == "highway")
            {
                p_roadlink->link_type = highway;

                if (v == "primary")
                {
                    p_roadlink->highway_type = primary;
                }
                else if (v == "primary_link")
                {
                    p_roadlink->highway_type = primary_link;
                }
                else if (v == "secondary")
                {
                    p_roadlink->highway_type = secondary;
                }
                else if (v == "tertiary")
                {
                    p_roadlink->highway_type = tertiary;
                }
                else if (v == "residential")
                {
                    p_roadlink->highway_type = residential;
                }
                else if (v == "motorway")
                {
                    p_roadlink->highway_type = motorway;
                }
                else if (v == "motorway_link")
                {
                    p_roadlink->highway_type = motorway_link;
                }
                else if (v == "trunk")
                {
                    p_roadlink->highway_type = trunk;
                }
                else if (v == "trunk_link")
                {
                    p_roadlink->highway_type = trunk_link;
                }
                else if (v == "footway")
                {
                    p_roadlink->highway_type = footway;
                }
                else if (v == "unclassified")
                {
                    p_roadlink->highway_type = unclassified;
                }
                else if (v == "service")
                {
                    p_roadlink->highway_type = service;
                }
                else if (v == "path")
                {
                    p_roadlink->highway_type = path;
                }

                else
                {
                    p_roadlink->highway_type = otherhighways;
                }
            }

            if (k == "building")
            {
                p_roadlink->link_type = building;
            }

            if (k == "railway")
            {
                p_roadlink->link_type = railway;
            }

            if (k == "beziers")
            {
                p_roadlink->link_type = beziers; // makeup roads
                p_roadlink->highway_type = primary;
            }

            // Determine if it is oneway
            if (k == "oneway")
            {
                if (v == "yes")
                {
                    p_roadlink->isOneway = true;
                }
            }

            // Determine the lanes
            if (k == "lanes")
            {
                p_roadlink->lanes = atoi(v.c_str());
            }
        }
    }
}

void MapParser::refineRoadNetWork()
{
    // Transform the road nodes to xy coordinates, lon = x, lat = y.
    if (boundsIsPosition) {
        min_corner = map_mincorner;
        max_corner = map_maxcorner;
    } else {
        min_corner = convertLongitudeLatitudeToXY(map_mincorner);
        max_corner = convertLongitudeLatitudeToXY(map_maxcorner);
    }
    int num_roadnode = roadnode_list.size();
    for (int i=0; i<num_roadnode; ++i)
    {
        RoadNode *p_roadnode = roadnode_list[i];
        if (p_roadnode->isPosition) {
            p_roadnode->position.x = p_roadnode->longitude;
            p_roadnode->position.y = p_roadnode->latitude;
        } else {
            p_roadnode->position = convertLongitudeLatitudeToXY(p_roadnode->longitude,
                                                                p_roadnode->latitude);
        }
    }

    // Refine the road links.
    //removeShortRoadLinks();

    // Break the road links if wanted.
    //breakRoadLinks();

    return;
}

void MapParser::removeShortRoadLinks()
{
    vector<RoadLink*> roadlink_list_1;
    roadlink_list_1.clear();

    int num_road = roadlink_list.size();
    for (int i=0; i<num_road; ++i)
    {
        RoadLink *p_roadlink = roadlink_list[i];

        if (p_roadlink->link_type != beziers)
        {
            // this is a real link
            vector<RoadNode*> *p_roadnode_list = &p_roadlink->roadnode_list;
            int num_point = p_roadnode_list->size();

            if (num_point > 1)
            {
                // refine the road link by discarding very short polylines
                vector<RoadNode*> roadnode_newlist;
                roadnode_newlist.clear();
                roadnode_newlist.push_back(p_roadnode_list->at(0));
                int k = 0;
                for (int j=1; j<num_point; ++j)
                {
                    RoadNode *p_roadnode_0 = roadnode_newlist[k];
                    RoadNode *p_roadnode_1 = p_roadnode_list->at(j);
                    double len = (p_roadnode_1->position - p_roadnode_0->position).length();

                    // check if the line is shorter than 1m
                    if (len >= 1.0)
                    {
                        roadnode_newlist.push_back(p_roadnode_1);
                        k++;
                    }
                    else
                    {
                        cout << "Warning! Short polyline (< 1m) from node " << p_roadnode_0->id << " to "
                             << p_roadnode_1->id << " at Road link " << p_roadlink->id
                             << ". length = " << len << endl;
                    }
                }

                // the new road node list of the roadlink must have at least two nodes.
                // otherwise discard the roadlink
                if (roadnode_newlist.size() > 1)
                {
                    *p_roadnode_list = roadnode_newlist;
                    roadlink_list_1.push_back(p_roadlink);
                }
                else
                {
                    // delete the road link
                    delete p_roadlink;
                }
            }
            else
            {
                // delete the road link which contains only one node.
                cout << "Warning! Road link " << p_roadlink->id << " has only one node. " << endl;
                delete p_roadlink;
            }

        }
        else
        {
            // this is a makeup link, keep it as it is.
            roadlink_list_1.push_back(p_roadlink);
        }
    }

    // assign the new road link
    roadlink_list = roadlink_list_1;
}

void MapParser::breakRoadLinks()
{
    vector<RoadLink*> roadlink_list_1;
    roadlink_list_1.clear();

    // break the links if applied
    int num_road = roadlink_list.size();
    for (int i=0; i<num_road; ++i)
    {
        RoadLink *p_roadlink = roadlink_list[i];

        if (p_roadlink->link_type != beziers)
        {
            // this is a real link
            vector<RoadNode*> *p_roadnode_list = &p_roadlink->roadnode_list;
            int num_point = p_roadnode_list->size();
            if (num_point > 1)
            {
                //            bool breaking_flag = false;
                int start_idx = 0;
                int end_idx = 1;

                while (end_idx < num_point-1)
                {
                    end_idx++;

                    RoadNode *p_roadnode_0 = p_roadnode_list->at(end_idx-2);
                    RoadNode *p_roadnode_1 = p_roadnode_list->at(end_idx-1);
                    RoadNode *p_roadnode_2 = p_roadnode_list->at(end_idx);

                    zVec2 v0 = p_roadnode_1->position - p_roadnode_0->position;
                    zVec2 v1 = p_roadnode_2->position - p_roadnode_1->position;

                    double breaking_value = v0.dotproduct(v1) / v0.length() / v1.length();
                    if (breaking_value <= breaking_threshold)
                    {
                        //                    breaking_flag = true;
                        //                    cout << "Breaking " << roadlink_list[i]->id << " "
                        //                         << start_idx << " " << end_idx-1<< endl;
                        roadlink_list_1.push_back(buildOneRoadLink(p_roadlink, start_idx, end_idx-1));
                        start_idx = end_idx - 1;
                    }
                }

                roadlink_list_1.push_back(buildOneRoadLink(p_roadlink, start_idx, end_idx));
                //            if (breaking_flag)
                //            {
                //                cout << "Breaking (last) " << roadlink_list[i]->id << " "
                //                     << start_idx << " " << end_idx << endl;
                //            }
            }
        }
        else
        {
            // this is a makeup link, keep it as it is.
            vector<RoadNode*> *p_roadnode_list = &p_roadlink->roadnode_list;
            roadlink_list_1.push_back(buildOneRoadLink(p_roadlink, 0, p_roadnode_list->size()-1));
        }
    }

    // delete the old road links and assign the new one.
    clearRoadLinks();
    roadlink_list = roadlink_list_1;

    return;
}

RoadLink *MapParser::buildOneRoadLink(RoadLink *oldlink, int start_nodeid, int end_nodeid)
{
    RoadLink *p_roadlink = new RoadLink;
    *p_roadlink = *oldlink;

    vector<RoadNode*> *p_roadnode_list = &p_roadlink->roadnode_list;
    p_roadnode_list->clear();
    for (int i=start_nodeid; i<=end_nodeid; ++i)
    {
        p_roadnode_list->push_back(oldlink->roadnode_list[i]);
    }
    return p_roadlink;
}

int MapParser::getNumberOfPolylinesOfRoadLinks()
{
    int bez_count = 0;
    for (int i=0; i<roadlink_list.size(); ++i)
    {
        vector<RoadNode*> *p_roadnode_list = &roadlink_list[i]->roadnode_list;
        bez_count += p_roadnode_list->size()-1;
    }
    return bez_count;
}

double MapParser::convertTimestamp(const string &str)
{
  struct tm ts = {0};
  sscanf(str.c_str(), "%i-%i-%iT%i:%i:%iZ",
         &ts.tm_year, &ts.tm_mon, &ts.tm_mday,
         &ts.tm_hour, &ts.tm_min, &ts.tm_sec);

  ts.tm_year -= 1900; // Years from 1900
  ts.tm_mon -= 1; // Months form January

  return difftime(mktime(&ts), reference_time);
}

zVec2 MapParser::convertLongitudeLatitudeToXY(zVec2 ll)
{
    return convertLongitudeLatitudeToXY(ll.x, ll.y);
}

zVec2 MapParser::convertLongitudeLatitudeToXY(double longitude, double latitude)
{
    // constants
    double cc_toradians = PI/180.0;
    double cc_equator2 = 6371000 * 2.0;

    // compute delta angles
    zVec2 result;
    result.x = longitude - map_center.x;
    result.y = latitude - map_center.y;

    // compute x
    double angle = result.x * cc_toradians;
    double sinangled2 = sin(angle/2.0);
    double haversine = cos(latitude * cc_toradians) * cos(map_center.y * cc_toradians) *
                       sinangled2 * sinangled2;
    double dd = cc_equator2 * atan2(sqrt(haversine), sqrt(1-haversine));
    if (angle >= 0)
    {
        result.x = dd;
    }
    else
    {
        result.x = -dd;
    }

    // compute y
    angle = result.y * cc_toradians;
    sinangled2 = sin(angle/2.0);
    haversine = sinangled2 * sinangled2;
    dd = cc_equator2 * atan2(sqrt(haversine), sqrt(1-haversine));
    if (angle >= 0)
    {
        result.y = dd;
    }
    else
    {
        result.y = -dd;
    }

    return result;
}

void MapParser::clearRoadNodes()
{
    int size = roadnode_list.size();
    for (int i=0; i<size; ++i)
    {
        delete roadnode_list[i];
    }
    roadnode_list.clear();
}

void MapParser::clearRoadLinks()
{
    int size = roadlink_list.size();
    for (int i=0; i<size; ++i)
    {
        delete roadlink_list[i];
    }
    roadlink_list.clear();
}

//=====================================================================================
void MapParser::quicksortByID(vector<RoadNode*> &nodelist, int p, int r)
{
    if (p < r)
    {
        int q = partitionByID(nodelist, p, r);
        quicksortByID(nodelist, p, q-1);
        quicksortByID(nodelist, q+1, r);
    }
}

int MapParser::partitionByID(vector<RoadNode*> &nodelist, int p, int r)
{
    // determine the pivot as the middle one
    int m = (p+r)/2;
    std::swap(nodelist[m], nodelist[r]);
    long long pivot_id = nodelist[r]->id;

    int i = p;
    for (int j=p; j<r; ++j)
    {
        if (nodelist[j]->id <= pivot_id)
        {
            std::swap(nodelist[i], nodelist[j]);

            i++;
        }
    }
    std::swap(nodelist[i], nodelist[r]);

    // So far the element [r] have moved to the right place and stay there.
    return i;
}

int MapParser::binarysearchByID(vector<RoadNode*> &nodelist, long long id_)
{
    int size_array = nodelist.size();
    int low = 0, high = size_array - 1, middle;
    while (low <= high)
    {
        middle = (low + high) / 2;
        if (id_ == nodelist[middle]->id)
        {
            return middle;
        }
        else if (id_ < nodelist[middle]->id)
        {
            high = middle - 1;
        }
        else
        {
            low = middle + 1;
        }
    }

    // no such road node.
    return -1;
}




