#ifndef ARGOS3_AIRTIGHT_AIRTIGHTFRAME_H
#define ARGOS3_AIRTIGHT_AIRTIGHTFRAME_H

#include <string>
#include <queue>
#include <argos3/core/utility/datatypes/datatypes.h>

#include "../control_interface/ci_slot_radio_actuator.h"

namespace argos {

    struct AirTightBuffer {
        std::string name;
        std::string recipient;
        std::deque<RadioFrame> buffer;
        bool highCriticality;
        UInt32 responseTime[2];
        UInt32 period;
        UInt32 nextFrameTime = 0;
    };

}

#endif //ARGOS3_AIRTIGHT_AIRTIGHTFRAME_H
