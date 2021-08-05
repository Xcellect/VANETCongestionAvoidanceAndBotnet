/*
 * P2PBotnetProtocol.cpp
 *
 *  Created on: May 6, 2021
 *      Author: cytro
 */

#pragma once

#include "veins/veins.h"
#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"
#include "veins/modules/application/traci/modified/P2PCongestionAvoidance/RoutingAPI/RoutingAPI.h"
#include "veins/modules/application/traci/modified/P2PCongestionAvoidance/Messages/CongestionRequest_m.h"
#include "veins/modules/application/traci/modified/P2PCongestionAvoidance/Messages/CongestionResponse_m.h"
#include "veins/modules/application/traci/modified/P2PCongestionAvoidance/Messages/BroadcastCongestionInfoStructEvt_m.h"
#include <algorithm>
#include <tuple>

namespace veins {

class VEINS_API P2PBotnetProtocol : public DemoBaseApplLayer {
public:
    void initialize(int stage) override;
protected:
    // For RoutingAPI
    std::string start;
    std::string end;
    RoutingAPI R;
    // Current route to follow
    std::list<std::string> activeRoute;

    string targetEdge1 = "B2C2";
    string targetEdge2 = "C2B2";

    simtime_t lastDroveAt;
    bool sentMessage;
    int currentSubscribedServiceId;
    simtime_t t_alpha = 5;
    bool localAdded = false;

    // Local congestion response message that acts as a DB
    // While respond, create a new CongestionResponse msg
    // and populate with cInfo structs for roadsOfInterest
    vector<CongestionInfoStruct> DBx;
    CongestionInfoStruct* cx;
    BroadcastCongestionInfoStructEvt* broadcastCInfoEvt;
    vector<vector<string>> candidateRoutes;
    vector<string> candidateEdges;
protected:
    // Your application has received a data message from another car or RSU
    // code for handling the message goes here, see TraciDemo11p.cc for examples
    void onWSM(BaseFrame1609_4* wsm) override;

    // Your application has received a beacon message from another car or RSU
    // code for handling the message goes here
    void onBSM(DemoSafetyMessage* bsm) override;

    // Your application has received a service advertisement from another car or RSU
    // code for handling the message goes here, see TraciDemo11p.cc for examples
    void onWSA(DemoServiceAdvertisment* wsa) override;

    // This method is for self messages (mostly timers)
    // it is important to call the DemoBaseApplLayer function for BSM and WSM transmission
    void handleSelfMsg(cMessage* msg) override;

    // The vehicle has moved. Code that reacts to new positions goes here.
    // member variables such as currentPosition and currentSpeed are updated in the parent class
    void handlePositionUpdate(cObject* obj) override;
    double vector2speed(Coord speed);
    void reroute();
    CongestionRequest* prepCongestionRequest();
    bool containsEdge(std::string responseEdge);
    void addOrDrop(veins::CongestionInfoStruct* msg);
};
bool sortByDist(const std::pair<std::string,double> &a, const std::pair<std::string,double> &b) {
    return (a.second < b.second);
}

} // namespace veins
