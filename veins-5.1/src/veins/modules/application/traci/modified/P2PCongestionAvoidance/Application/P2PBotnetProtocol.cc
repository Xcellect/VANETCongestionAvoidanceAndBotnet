/*
 * P2PBotnetProtocol.cpp
 *
 *  Created on: May 6, 2021
 *      Author: cytro
 */

#include <veins/modules/application/traci/modified/P2PCongestionAvoidance/Application/P2PBotnetProtocol.h>
#include <math.h>
using namespace veins;

Define_Module(veins::P2PBotnetProtocol);

void P2PBotnetProtocol::initialize(int stage)
{
    DemoBaseApplLayer::initialize(stage);
    if (stage == 0) {
        sentMessage = false;
        lastDroveAt = simTime();
        currentSubscribedServiceId = -1;

        // get the start and end vertices here
        activeRoute = traciVehicle->getPlannedRoadIds();
        start = traciVehicle->getRoadId().substr(0,2);
        end = activeRoute.back().substr(2,4);
        int sameStartEnd = start.compare(end);
        if(sameStartEnd == 0) {
            start = traciVehicle->getRoadId().substr(2,4);
        }
        // generate all possible routes
        R.generatePaths(start, end, 50);


        // initializing local cinfo struct, namely cx
        cx = new CongestionInfoStruct();
        cx->setCarID(mobility->getNode()->getIndex());
        cx->setEdgeID(traciVehicle->getRoadId().c_str());

        broadcastCInfoEvt = new BroadcastCongestionInfoStructEvt();
        scheduleAt(simTime() + 2, broadcastCInfoEvt);

    }
}

// Your application has received a service advertisement from another car or RSU
// code for handling the message goes here, see TraciDemo11p.cc for examples
void P2PBotnetProtocol::onWSA(DemoServiceAdvertisment* wsa)
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
void P2PBotnetProtocol::onWSM(BaseFrame1609_4* frame)
{
    if(frame->getPsid() == 1) {
        /***************** Exploiting Congestion Response messages **********/
        /* Idea: A
         * 1. Generate all shortest paths from target road to the roi
         * 2. Return the edges that are in requested road list & this path list
         *    a. with high avg speed (preferably max)
         *    b. fresh timestamps
         * Idea: B
         * 1. From the requested road list, find which on is the closest to the target junc
         * 2. Return that edge with high speed and others with low
         */

        CongestionRequest* request = check_and_cast<CongestionRequest*>(frame);
        findHost()->getDisplayString().setTagArg("i", 1, "green");

            // repeat the received traffic update once in 2 seconds plus some random delay
        CongestionResponse* responseToSend = new CongestionResponse();
        vector<pair<string, int>> roadToTarget;
        vector<CongestionInfoStruct> responseVector;
        vector<string> roiVec = request->getRoadsOfInterest();

        for(int i = 0; i < roiVec.size(); i++) {
            string roi = roiVec[i];
            // here find which next edge takes it to the target
            Coord roiPos = traci->junction(roi.substr(2, 4)).getPosition();
            Coord tarFPos = traci->junction(targetEdge1.substr(0, 2)).getPosition();
            Coord tarBPos = traci->junction(targetEdge1.substr(2, 4)).getPosition();
            // Only check the roi dest junction with both target junc
            double distF = traci->getDistance(roiPos, tarFPos, false); // need air dist
            double distB = traci->getDistance(roiPos, tarBPos, false); // bc driving dist is negative?
            if(distF > distB) {
                roadToTarget.push_back(make_pair(roi,distF));
            } else {
                roadToTarget.push_back(make_pair(roi,distB));
            }
        }

        std::sort(roadToTarget.begin(),roadToTarget.end(), sortByDist);
        bool targetSet = false;
        // construct fake data
        for(auto it : roadToTarget) {
            CongestionInfoStruct* fakeData = new CongestionInfoStruct();
            fakeData->setEdgeID(it.first.c_str());
            fakeData->setCarID(cx->getCarID());
            (!targetSet) ? (fakeData->setAverageSpeed(traciVehicle->getMaxSpeed())) :
                    (fakeData->setAverageSpeed(0));
            targetSet = true;
            fakeData->setTimestamp(simTime()+1);
            responseVector.push_back(*fakeData);
        }
        /*****************************************************************/
        if(!responseVector.empty() && !sentMessage) {
            sentMessage = false;
            responseToSend->setCInfoStructDB(responseVector);
            responseToSend->setPsid(2);
            LAddress::L2Type addr = request->getSenderAddress();
            populateWSM(responseToSend, addr, 0);
            if (dataOnSch) {
                startService(Channel::sch2, 42, "Congestion response");
                // started service and server advertising, schedule message to self to send later
                scheduleAt(computeAsynchronousSendingTime(1, ChannelType::service), responseToSend);
            } else {
                // send right away on CCH, because channel switching is disabled
                sendDown(responseToSend);
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
    }
}

void P2PBotnetProtocol::onBSM(DemoSafetyMessage* bsm)
{
    DemoSafetyMessage* beacon = check_and_cast<DemoSafetyMessage*>(bsm);
    // figure out how this coord belongs to this edge
    string beaconEdge = get<0>(traci->getRoadMapPos(beacon->getSenderPos()));
    // calculate average every time it gets a beacon
    if(beaconEdge.compare(cx->getEdgeID()) == 0) {
        double avg = 0;
        if(cx->getAverageSpeed() == 0) {
            avg = (traciVehicle->getSpeed() + vector2speed(beacon->getSenderSpeed()))/2;
        } else {
            avg = (cx->getAverageSpeed() + vector2speed(beacon->getSenderSpeed()))/2;
        }
            cx->setAverageSpeed(avg);
        }
        cx->setTimestamp(simTime());
        addOrDrop(cx);
}
double P2PBotnetProtocol::vector2speed(Coord speed) {
    double extracted;
    if(speed.x < 0 || speed.x > 1) {
        extracted = abs(speed.x);
    } else if(speed.y < 0 || speed.y > 1) {
        extracted = abs(speed.y);
    } else if(speed.z < 0 || speed.z > 1) {
        extracted = abs(speed.z);
    }
    return extracted;
}

// This method is for self messages (mostly timers)
// it is important to call the DemoBaseApplLayer function for BSM and WSM transmission
// ## can be used to periodically broadcast congestion info struct (every 2 seconds)
void P2PBotnetProtocol::handleSelfMsg(cMessage* msg)
{
    if ( dynamic_cast<BroadcastCongestionInfoStructEvt*>(msg) ) {
        /***************** Exploiting BSMs **********/
        DemoSafetyMessage* bsm = new DemoSafetyMessage();
        if(targetEdge1.compare( cx->getEdgeID() ) == 0 ||
                targetEdge2.compare( cx->getEdgeID() ) == 0) {
            bsm->setSenderPos(curPosition);
            // send max speed when on target road
            bsm->setSenderSpeed(mobility->getCurrentDirection() * traciVehicle->getMaxSpeed());
            bsm->setPsid(-1);
            bsm->setChannelNumber(static_cast<int>(Channel::cch));
            bsm->addBitLength(beaconLengthBits);
            bsm->setUserPriority(beaconUserPriority);
        } else {
            //bsm->setChannelNumber(channelNumber)
            populateWSM(bsm);
        }
        sendDown(bsm);

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
void P2PBotnetProtocol::handlePositionUpdate(cObject* obj)
{
    DemoBaseApplLayer::handlePositionUpdate(obj);
    // make measurement every position update. push it only when it exits that road
    cx->setAverageSpeed(traciVehicle->getSpeed());
    cx->setEdgeID(traciVehicle->getRoadId().c_str());

    // removing sentmessage flag
    if(traciVehicle->getLanePosition() >= 160 && !sentMessage) {
        candidateRoutes = R.getKRoutes(50);
        sentMessage = false;
        CongestionRequest* requestToSend = prepCongestionRequest();
        requestToSend->setPsid(1);
        requestToSend->setSenderAddress(mac->getMACAddress());
        if (dataOnSch) {
            startService(Channel::sch1, 24, "Congestion Service");
            // started service and server advertising, schedule message to self to send later
            scheduleAt(computeAsynchronousSendingTime(1, ChannelType::service), requestToSend);
        } else {
            // send right away on CCH, because channel switching is disabled
            sendDown(requestToSend);
        }

        // car is in checkpoint range. push own measurement first. reset local cinfo struct
        if(!localAdded) {
            addOrDrop(cx);
            // reset and prepare for next edge
            cx->setAverageSpeed(0);
            localAdded = true;
        }
    } else {
        findHost()->getDisplayString().setTagArg("i", 1, "yellow");
        // car is outside the checkpoint range. measure bsm stuff here.
        localAdded = false;
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

void P2PBotnetProtocol::reroute() {
    // BEFORE GETTING HERE: Figure out where we receive the RESPONSE [done]
            // TODO: Calculate the best route from the local DB

            map<string,double> candidateRoads;
            for(int i = 0; i < DBx.size(); i++) {
                string edgeInDB = DBx[i].getEdgeID();
                if(traciVehicle->getRoadId().substr(2,4).compare(edgeInDB.substr(0,2)) == 0) {
                    candidateRoads.insert(pair<string,double>(DBx[i].getEdgeID(),DBx[i].getAverageSpeed()));
                }
            }

            // sorting is unnecessary. just find the road with max avg speed.

            double maxSpeed = 10;
            double temp = 0;
            string bestEdge = "";
            for(auto& it : candidateRoads) {
                temp = it.second;
                if(temp > maxSpeed) {
                    maxSpeed = temp;
                    bestEdge = it.first;
                }
            }
            //if(bestEdge.compare("")!=0) traciVehicle->changeRoute(bestEdge, 1);
            bool active = false;
            bool abort = false;
            // A0 E0 A4 E4
            list<string> forbidden = {"A0", "E0", "A4", "E4"};
            // abort the best edge junctions that don't allow u turns
            for(auto str : forbidden) {
                if(bestEdge.substr(0,2).compare(str) == 0) {
                    abort = true;
                }
            }
            // abort if the best edge is already in prev route
            for(auto str : activeRoute) {
                if(bestEdge.compare(str) == 0) {
                    abort = true;
                }
            }

            list<string> prevRoute(activeRoute);
            activeRoute.clear();

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

            // active = selected
            // if(!active || (activeRoute.size() > (prevRoute.size()-1))) {
            /*
            if(active && (activeRoute.size() > (prevRoute.size()-1))) {
                activeRoute.push_front(traciVehicle->getRoadId());
            } else {
                activeRoute.assign(prevRoute.begin(),prevRoute.end());
            }

            */

            // only find the next best edge not globally optimal. so this route might be
            // longer than original route
            if(active) {
                activeRoute.push_front(traciVehicle->getRoadId());
            } else {
                activeRoute.assign(prevRoute.begin(),prevRoute.end());
            }
            // Finally set this as active roads using the Traci interface
            traciVehicle->changeVehicleRoute(activeRoute);
            candidateRoutes.clear();

}

CongestionRequest* P2PBotnetProtocol::prepCongestionRequest() {
    vector<string> candidateEdges;
    for(int i = 0; i < candidateRoutes.size(); i++) {
        for(int j = 0; j < candidateRoutes[i].size(); j++) {
            if(traciVehicle->getRoadId().substr(2,4).compare(candidateRoutes[i][j].substr(0,2)) == 0) {
                if (find(candidateEdges.begin(), candidateEdges.end(), candidateRoutes[i][j]) == candidateEdges.end()) {
                  // someName not in name, add it
                    candidateEdges.push_back(candidateRoutes[i][j]);
                }
            }
        }
    }
    CongestionRequest* cRequest = new CongestionRequest();
    populateWSM(cRequest);
    cRequest->setRoadsOfInterest(candidateEdges);
    return cRequest;
}

// check if the local DB contains the edge in response
bool P2PBotnetProtocol::containsEdge(std::string responseEdge) {
    for(int i = 0; i < DBx.size(); i++) {
        string edge = DBx[i].getEdgeID();
        if(edge.compare(responseEdge) == 0 ) {
            return true;
        }
    }
    return false;
}

void P2PBotnetProtocol::addOrDrop(CongestionInfoStruct* msg) {
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
