#ifndef ARGOS3_AIRTIGHT_PROTOCOL1FRAME_H
#define ARGOS3_AIRTIGHT_PROTOCOL1FRAME_H

#include <any>
#include "../generic/slot_radio_frame.h"

namespace argos {
    struct Protocol1Frame : SlotRadioFrame {
        Protocol1Frame(const std::string& from,
                      const std::string& to,
                      std::string originBuffer,
                      const std::any& msg,
                      UInt32 clock) :
            SlotRadioFrame(from, to, msg),
            frameOriginBuffer(std::move(originBuffer)),
            queueTime(clock) {};

        ~Protocol1Frame() override = default;

        std::vector<bool> delayedAcks;
        std::string frameOriginBuffer;
        UInt32 queueTime;
    };
}

#endif //ARGOS3_AIRTIGHT_PROTOCOL1FRAME_H
