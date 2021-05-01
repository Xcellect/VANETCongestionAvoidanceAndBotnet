#include "RoutingAPI.h"
#include <sstream>

using namespace std;
using namespace veins;

// Private methods
vector<string> &RoutingAPI::split(const string &s, char delim,vector<string> &elems) {
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {
        if (item.length() > 0) {
            elems.push_back(item);
        }
    }
    return elems;
}


vector<string> RoutingAPI::split(const string &s, char delim) {
    vector<string> elems;
    split(s, delim, elems);
    return elems;
}

void RoutingAPI::processRoute() {
    for(int i = 0; i < _routes.size(); i++) {
        _edgeList.push_back(split(_routes[i],' '));
    }
}

// Public methods
void RoutingAPI::generatePaths(string start, string end, int limit) {
    // specify the network file aka the graph definition
    Graph road_graph("grid_16_200_graph");
    YenTopKShortestPathsAlg yenAlg(road_graph, road_graph.get_vertex(
            road_graph.strToDec(start)), road_graph.get_vertex(
                    road_graph.strToDec(end)));

    // set the number here
    int counter = 0;
    while(yenAlg.has_next() && counter < limit)
    {
        ++counter;
        // Instead of printing out, we store it
        _routes.push_back(yenAlg.next()->edgelistOut());
    }
    processRoute();
}

vector<string> RoutingAPI::getShortestPath(string start, string end) {
    Graph* road_graph_pt = new Graph("grid_16_200_graph");
    DijkstraShortestPathAlg shortest_path_alg(road_graph_pt);
    BasePath* result =
        shortest_path_alg.get_shortest_path(
            road_graph_pt->get_vertex(road_graph_pt->strToDec(start)),
            road_graph_pt->get_vertex(road_graph_pt->strToDec(end)));
    _routes.push_back(result->edgelistOut());
    processRoute();
    return _edgeList[0];
}

vector<vector<string>> RoutingAPI::getKRoutes(int k) {
    vector<vector<string>> result;
    for(int i = 0; i < k; i++) {
        result.push_back(_edgeList[i]);
    }
    return result;
}
