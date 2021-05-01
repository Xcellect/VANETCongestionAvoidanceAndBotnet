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

#include "veins/modules/application/traci/modified/P2PCongestionAvoidance/Application/P2PCongestionProtocol.h"
#include <math.h>
using namespace veins;

Define_Module(veins::P2PCongestionProtocol);

void P2PCongestionProtocol::initialize(int stage)
{
    DemoBaseApplLayer::initialize(stage);
    if (stage == 0) {
        sentMessage = false;
        lastDroveAt = simTime();
        currentSubscribedServiceId = -1;

        // get the start and end vertices here
        activeRoute = traciVehicle->getPlannedRoadIds();
        start = activeRoute.front().substr(0,2);
        end = activeRoute.back().substr(2,4);
        int sameStartEnd = start.compare(end);
        if(sameStartEnd == 0) {
            start = activeRoute.front().substr(2,4);
        }
        // generate all possible routes
        R.generatePaths(start, end, 50);
        broadcastCInfoEvt = new BroadcastCongestionInfoStructEvt();
        scheduleAt(simTime() + 2, broadcastCInfoEvt);
    }
}

// Your application has received a service advertisement from another car or RSU
// code for handling the message goes here, see TraciDemo11p.cc for examples
void P2PCongestionProtocol::onWSA(DemoServiceAdvertisment* wsa)
{
    if (currentSubscribedServiceId == -1) {
        mac->changeServiceChannel(static_cast<Channel>(wsa->getTargetChannel()));
        currentSubscribedServiceId = wsa->getPsid();
        if (currentOfferedServiceId != wsa->getPsid()) {
            stopService();
            startService(static_cast<Channel>(wsa->getTargetChannel()),
                    wsa->getPsid(), "Mirrored Traffic Service");
        }
    }
}

// Your application has received a data message from another car or RSU
// code for handling the message goes here, see TraciDemo11p.cc for examples
// ## this is where we GET either congestion request message and should send
// the response here or get periodic congestion info struct
void P2PCongestionProtocol::onWSM(BaseFrame1609_4* frame)
{
    if(frame->getPsid() == 1) {
        CongestionRequest* request = check_and_cast<CongestionRequest*>(frame);
        findHost()->getDisplayString().setTagArg("i", 1, "green");
        if (!DBx.empty()) {
            // repeat the received traffic update once in 2 seconds plus some random delay
            CongestionResponse* responseToSend = new CongestionResponse();
            vector<CongestionInfoStruct> responseVector;
            int sizeOfResponse = request->getRoadsOfInterestArraySize();
            for(int i = 0; i < sizeOfResponse; i++) {
                string roi = request->getRoadsOfInterest(i);
                for(int j = 0; j < DBx.size(); j++) {
                    if(roi.compare(DBx[j].getEdgeID()) == 0) {
                        responseVector.push_back(DBx[j]);
                    }
                }
            }
            if(!responseVector.empty()) {
                responseToSend->setCInfoStructDB(responseVector);
                responseToSend->setPsid(2);
                LAddress::L2Type addr = request->getSenderAddress();
                populateWSM(responseToSend, addr, 0);
                sentMessage = true;
                if (dataOnSch) {
                    startService(Channel::sch1, 42, "Congestion Response Service");
                    // started service and server advertising, schedule message to self to send later
                    scheduleAt(computeAsynchronousSendingTime(1, ChannelType::service), responseToSend);
                } else {
                  // send right away on CCH, because channel switching is disabled
                    sendDown(responseToSend);
                }
            }
        }
    } else if(frame->getPsid() == 2) {
        CongestionResponse* responseReceived = check_and_cast<CongestionResponse*>(frame);
        findHost()->getDisplayString().setTagArg("i", 1, "purple");
        vector<CongestionInfoStruct> DBr = responseReceived->getCInfoStructDB();
        string simtime = simTime().str();
        for(int i = 0; i < DBr.size(); i++) {
            addOrDrop(&DBr[i]);
        }
        // reroute here.
        reroute();
    } else if(frame->getPsid() == 3) {
        CongestionInfoStruct* beacon = check_and_cast<CongestionInfoStruct*>(frame);
        addOrDrop(beacon);
    }
}

// This method is for self messages (mostly timers)
// it is important to call the DemoBaseApplLayer function for BSM and WSM transmission
// ## can be used to periodically broadcast congestion info struct (every 2 seconds)
void P2PCongestionProtocol::handleSelfMsg(cMessage* msg)
{
    // The following check could be useful to check what kind of message we got.
    // Or we could check the message kind. wsm->setKind (>0 for user defined ones)
    if ( dynamic_cast<BroadcastCongestionInfoStructEvt*>(msg) ) {
        // send this message on the service channel until the counter is 3 or higher.
        // this code only runs when channel switching is enabled
        /***************** Broadcast CongestionInfoStruct (Beacon message) **********/
        // TODO: Figure out where to send the cInfo structs (as beacon)
        // schecduleAt is for self messages. should probably use send down instead
        // Just get the own road measurements directly.
        // Send structs as bsms here since this acts as a time and can be used for
        // periodic broadcast.
        CongestionInfoStruct* cInfo = new CongestionInfoStruct();
        cInfo->setCarID(mobility->getNode()->getIndex());
        cInfo->setEdgeID( traciVehicle->getRoadId().c_str() );
        // TODO: This should be the average speed of all cars in the road
        cInfo->setAverageSpeed(traciVehicle->getSpeed());
        cInfo->setTimestamp(simTime());
        addOrDrop(cInfo);   // add this measurement to local
        populateWSM(cInfo);
        cInfo->setPsid(3);
        sendDown(cInfo);
        // Send beacon every 2 seconds
        scheduleAt(simTime() + 2, dynamic_cast<BroadcastCongestionInfoStructEvt*>(msg));
    } else {
        DemoBaseApplLayer::handleSelfMsg(msg);
    }
}

// 1. set checkpoint,
    // 2. send congestion request messages,
    // 3. calculate congestion up to next checkpoint.
    // 4. if it is in a congested area, ask the API for
    // another k set of routes and calculate their congestion

    // check distance traveled every 300 meters
    // if the comm range is 300m the next cp is in the next edge.
    // calculate least congested route till that. i.e. check if that
    // cp is in a congested area. we could just bypass this and check
    // only the next road which would be basically the same. so check
    // the end junction of the next edge, for k sets of roads, see
    // which one yields the maximum speed. that's it.

    // CURRENTAPPROACH: Send request at the end of every road.


// The vehicle has moved. Code that reacts to new positions goes here.
// member variables such as currentPosition and currentSpeed are updated in the parent class
// ## this is where we send congestion request messages
void P2PCongestionProtocol::handlePositionUpdate(cObject* obj)
{
    DemoBaseApplLayer::handlePositionUpdate(obj);
    // removing sentmessage flag
    if(traciVehicle->getLanePosition() >= 160) {
        sentMessage = true;
        candidateRoutes = R.getKRoutes(10);
        CongestionRequest* requestToSend = prepCongestionRequest();
        requestToSend->setPsid(1);
        requestToSend->setSenderAddress(mac->getMACAddress());
        if (dataOnSch) {
            startService(Channel::sch1, 24, "Congestion Request Service");
            // started service and server advertising, schedule message to self to send later
            scheduleAt(computeAsynchronousSendingTime(1, ChannelType::service), requestToSend);
        } else {
            // send right away on CCH, because channel switching is disabled
            sendDown(requestToSend);
        }

        // car is in checkpoint range
    } else {
        findHost()->getDisplayString().setTagArg("i", 1, "yellow");
        // car is outside the checkpoint range
    }

}
// Make congestion request
    // time to process the response
    // 1. the car store all its collected congestion info in
    // its local congestion response message.
    //      1. a. storing requires that algorithm [done]
    // 2. when it gets a request it will query this db
    // and create a local variable to store this response
    //      2. b. happens in onWSM/onBSM??? ***
    //      2. c. send this local message
    // 3. when it gets a response it merges with the local DB
    //      3. a. As BSM every 2 seconds???
    //      3. b. As congestion response message (See 1, 2)
    // 4. send congestion request in handlePositionUpdate
    //      4. a. requires prepCongestionRequest  [done]

void P2PCongestionProtocol::reroute() {
    // BEFORE GETTING HERE: Figure out where we receive the RESPONSE [done]
    // TODO: Calculate the best route from the local DB
    map<string,double> candidateRoads;
    for(int i = 0; i < DBx.size(); i++) {
        string edgeInDB = DBx[i].getEdgeID();
        if(traciVehicle->getRoadId().substr(2,4).compare(edgeInDB.substr(0,2)) == 0) {
            candidateRoads[edgeInDB] += DBx[i].getAverageSpeed();
        }
    }
    // sorting is unnecessary. just find the road with max speed.
    double maxSpeed = 0;
    double temp = 0;
    string bestEdge = "";
    for(auto& it : candidateRoads) {
        temp = it.second;
        if(temp > maxSpeed) {
            maxSpeed = temp;
            bestEdge = it.first;
        }
    }
    list<string> prevRoute(activeRoute);
    bool active = false;
    bool abort = false;
    activeRoute.clear();
    // A0 E0 A4 E4
    list<string> forbidden = {"A0", "E0", "A4", "E4"};
    for(auto str : forbidden) {
        if(bestEdge.substr(0,2).compare(str) == 0) {
            abort = true;
        }
    }
    // abort the junctions that don't allow u turns
    if(!abort) {
        // Start with the current road.
        for(int i = 0; i < candidateRoutes.size(); i++) {
            for(int j = 0; j < candidateRoutes[i].size(); j++) {
                if(bestEdge.compare(candidateRoutes[i][j]) == 0 || active) {
                    activeRoute.push_back(candidateRoutes[i][j]);
                    active = true;
                }
            }
            if(active) {
                break;
            }
        }
    }

    // if active is false or has only 1 edge that no better road was found
    if(!active) {
        activeRoute.assign(prevRoute.begin(),prevRoute.end());
    } else {
        activeRoute.push_front(traciVehicle->getRoadId());
    }

    // Finally set this as active roads using the Traci interface
    traciVehicle->changeVehicleRoute(activeRoute);
    candidateRoutes.clear();
}

CongestionRequest* P2PCongestionProtocol::prepCongestionRequest() {
    vector<string> candidateEdges;
    for(int i = 0; i < candidateRoutes.size(); i++) {
        for(int j = 0; j < candidateRoutes[i].size(); j++) {
            if(traciVehicle->getRoadId().substr(2,4).compare(candidateRoutes[i][j].substr(0,2)) == 0) {
                candidateEdges.push_back(candidateRoutes[i][j]);
            }
        }
    }
    CongestionRequest* cRequest = new CongestionRequest();
    populateWSM(cRequest);
    cRequest->setRoadsOfInterestArraySize(candidateEdges.size());
    for(int i = 0; i < candidateEdges.size(); i++) {
        cRequest->setRoadsOfInterest(i, candidateEdges[i]);
    }
    return cRequest;
}

// check if the local DB contains the edge in response
bool P2PCongestionProtocol::containsEdge(std::string responseEdge) {
    for(int i = 0; i < DBx.size(); i++) {
        string edge = DBx[i].getEdgeID();
        if(edge.compare(responseEdge) == 0 ) {
            return true;
        }
    }
    return false;
}

void P2PCongestionProtocol::addOrDrop(CongestionInfoStruct* msg) {
    if(DBx.empty() || !containsEdge(msg->getEdgeID())) {
        DBx.push_back(*msg);
    } else {
        CongestionInfoStruct* m = new CongestionInfoStruct();
        int mIndex;
        for(int i = 0; i < DBx.size(); i++) {
            string edge = DBx[i].getEdgeID();
            if(edge.compare(msg->getEdgeID()) == 0 ) {
                m = &DBx[i];
                mIndex = i;
                break;
            }
        }
        if(msg->getTimestamp() > m->getTimestamp()) {
            if(m->getCarID() != mobility->getNode()->getIndex()) {
                DBx.erase(DBx.begin()+mIndex);
                DBx.push_back(*msg);
            } else if(msg->getTimestamp() - m->getTimestamp() > t_alpha) {
                DBx.erase(DBx.begin()+mIndex);
                DBx.push_back(*msg);
            } else {
                drop(msg);
            }

        } else {
            drop(msg);
        }
    }
}
