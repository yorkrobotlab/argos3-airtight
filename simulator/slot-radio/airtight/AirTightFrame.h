#ifndef ARGOS3_AIRTIGHT_AIRTIGHTFRAME_H
#define ARGOS3_AIRTIGHT_AIRTIGHTFRAME_H

#include <any>
#include "../generic/slot_radio_frame.h"

namespace argos {
    struct AirTightFrame : SlotRadioFrame {
        AirTightFrame(const std::string& from,
                      const std::string& to,
                      std::string originBuffer,
                      const std::any& msg,
                      UInt32 clock) :
            SlotRadioFrame(from, to, msg),
            frameOriginBuffer(std::move(originBuffer)),
            queueTime(clock) {};

        ~AirTightFrame() override = default;

        std::string frameOriginBuffer;
        UInt32 queueTime;
    };
}

#endif //ARGOS3_AIRTIGHT_AIRTIGHTFRAME_H
