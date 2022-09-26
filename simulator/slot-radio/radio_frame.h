#ifndef ARGOS3_AIRTIGHT_TDMA_UTIL_H
#define ARGOS3_AIRTIGHT_TDMA_UTIL_H

#include <string>
#include <vector>
#include <argos3/core/utility/datatypes/datatypes.h>

typedef std::vector<argos::UInt32> RadioMessage;

namespace argos {
    struct RadioFrame {
        std::string frameFrom, frameTo, frameOriginBuffer;
        RadioMessage frameMsg;
        UInt32 queueTime;
    };
}


#endif //ARGOS3_AIRTIGHT_TDMA_UTIL_H
