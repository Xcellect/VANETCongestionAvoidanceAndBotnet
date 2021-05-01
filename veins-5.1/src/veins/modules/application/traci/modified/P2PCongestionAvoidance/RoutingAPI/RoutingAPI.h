#include <limits>
#include <set>
#include <map>
#include <queue>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>
#include "GraphElements.h"
#include "Graph.h"
#include "DijkstraShortestPathAlg.h"
#include "YenTopKShortestPathsAlg.h"

#pragma once
namespace veins {
    class RoutingAPI {
        private:
            vector<string> _routes;
            vector<vector<string>> _edgeList;
            vector<string> &split(const string &s, char delim,vector<string> &elems);
            vector<string> split(const string &s, char delim);
            // Turn the list of vertices into paths
            void processRoute();
        public:
            // Genereate all possible routes
            void generatePaths(string start, string end, int limit);
            vector<string> getShortestPath(string start, string end);
            // Return k routes. This function uses processRoute
            vector<vector<string>> getKRoutes(int k);

    };
}
