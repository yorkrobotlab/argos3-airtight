#ifndef ARGOS3_AIRTIGHT_SLOT_RADIO_FRAME_H
#define ARGOS3_AIRTIGHT_SLOT_RADIO_FRAME_H

#include <any>
#include <argos3/core/utility/datatypes/datatypes.h>

namespace argos {
    struct SlotRadioFrame {
        //SlotRadioFrame() = default;

        SlotRadioFrame(std::string from, std::string to, std::any msg) :
            frameFrom(std::move(from)), frameTo(std::move(to)), frameMsg(std::move(msg)) {};

        // We need a virtual method to allow dynamic casts
        virtual ~SlotRadioFrame() = default;

        std::string frameFrom, frameTo;
        std::any frameMsg;
    };
}

#endif //ARGOS3_AIRTIGHT_SLOT_RADIO_FRAME_H
