#ifndef ARGOS3_AIRTIGHT_AIRTIGHTBUFFER_H
#define ARGOS3_AIRTIGHT_AIRTIGHTBUFFER_H

#include <queue>
#include <argos3/core/utility/datatypes/datatypes.h>

#include "../control_interface/ci_slot_radio_actuator.h"
#include "AirTightFrame.h"

namespace argos {

    struct AirTightBuffer {
        std::string name;
        std::string recipient;
        std::deque<AirTightFrame> buffer;
        bool highCriticality;
        UInt32 responseTime[2];
        UInt32 period;
        UInt32 nextFrameTime = 0;
    };

}

#endif //ARGOS3_AIRTIGHT_AIRTIGHTBUFFER_H
