#ifndef ARGOS3_AIRTIGHT_PROTOCOL1BUFFER_H
#define ARGOS3_AIRTIGHT_PROTOCOL1BUFFER_H

#include <set>
#include <queue>
#include <argos3/core/utility/datatypes/datatypes.h>

#include "../control_interface/ci_slot_radio_actuator.h"
#include "Protocol1Frame.h"

namespace argos {

    struct Protocol1Buffer {
        std::string name;
        std::deque<Protocol1Frame> buffer;
        std::set<std::string> receivedAcks;
        bool highCriticality;
        UInt32 responseTime[2];
        UInt32 period;
        UInt32 nextFrameTime = 0;
        UInt32 targetAcks;
    };
}

#endif //ARGOS3_AIRTIGHT_PROTOCOL1BUFFER_H
