//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#pragma once

#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"
#include <vector>
#include "../messages/BroadcastParkingPlaceInformationEvt_m.h"
#include "../messages/ResourceReport_m.h"
namespace veins {
class P2PRSU: public DemoBaseApplLayer {
public:
    static std::vector<P2PRSU*> RSUs;

    int capacity;
    int occupancy;

    Coord& pos() {
        return curPosition;
    }

    int id() {
        return myId;
    }

protected:
    BroadcastParkingPlaceInformationEvt* broadcastPPIEvt;

    void initialize(int stage) override;

    void handleSelfMsg(cMessage* msg) override;

private:
    cOutVector occupancyVector;

    ResourceReport* generateReport();
};
}
