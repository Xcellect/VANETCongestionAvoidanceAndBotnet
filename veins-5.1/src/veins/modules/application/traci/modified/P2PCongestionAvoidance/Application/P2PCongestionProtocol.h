//
// Copyright (C) 2006-2011 Christoph Sommer <christoph.sommer@uibk.ac.at>
//
// Documentation for these modules is at http://veins.car2x.org/
//
// SPDX-License-Identifier: GPL-2.0-or-later
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#pragma once

#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"
#include "veins/modules/application/traci/modified/P2PCongestionAvoidance/RoutingAPI/RoutingAPI.h"
#include "veins/modules/application/traci/modified/P2PCongestionAvoidance/Messages/CongestionRequest_m.h"
#include "veins/modules/application/traci/modified/P2PCongestionAvoidance/Messages/CongestionResponse_m.h"
#include "veins/modules/application/traci/modified/P2PCongestionAvoidance/Messages/BroadcastCongestionInfoStructEvt_m.h"

namespace veins {

/**
 * @brief
 * A tutorial demo for TraCI. When the car is stopped for longer than 10 seconds
 * it will send a message out to other cars containing the blocked road id.
 * Receiving cars will then trigger a reroute via TraCI.
 * When channel switching between SCH and CCH is enabled on the MAC, the message is
 * instead send out on a service channel following a Service Advertisement
 * on the CCH.
 *
 * @author Christoph Sommer : initial DemoApp
 * @author David Eckhoff : rewriting, moving functionality to DemoBaseApplLayer, adding WSA
 *
 */

class VEINS_API P2PCongestionProtocol : public DemoBaseApplLayer {
public:
    void initialize(int stage) override;
protected:
    // For RoutingAPI
    std::string start;
    std::string end;
    RoutingAPI R;
    // Current route to follow
    std::list<std::string> activeRoute;

    simtime_t lastDroveAt;
    bool sentMessage;
    int currentSubscribedServiceId;
    simtime_t t_alpha = 5.0;

    // Local congestion response message that acts as a DB
    // While respond, create a new CongestionResponse msg
    // and populate with cInfo structs for roadsOfInterest
    vector<CongestionInfoStruct> DBx;
    BroadcastCongestionInfoStructEvt* broadcastCInfoEvt;
    vector<vector<string>> candidateRoutes;
protected:
    // Your application has received a data message from another car or RSU
    // code for handling the message goes here, see TraciDemo11p.cc for examples
    void onWSM(BaseFrame1609_4* wsm) override;

    // Your application has received a beacon message from another car or RSU
    // code for handling the message goes here
    //void onBSM(DemoSafetyMessage* bsm) override;

    // Your application has received a service advertisement from another car or RSU
    // code for handling the message goes here, see TraciDemo11p.cc for examples
    void onWSA(DemoServiceAdvertisment* wsa) override;

    // This method is for self messages (mostly timers)
    // it is important to call the DemoBaseApplLayer function for BSM and WSM transmission
    void handleSelfMsg(cMessage* msg) override;

    // The vehicle has moved. Code that reacts to new positions goes here.
    // member variables such as currentPosition and currentSpeed are updated in the parent class
    void handlePositionUpdate(cObject* obj) override;

    void reroute();
    CongestionRequest* prepCongestionRequest();
    bool containsEdge(std::string responseEdge);
    void addOrDrop(veins::CongestionInfoStruct* msg);
};

} // namespace veins
