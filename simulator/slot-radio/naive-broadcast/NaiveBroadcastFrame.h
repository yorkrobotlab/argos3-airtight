#ifndef ARGOS3_AIRTIGHT_NAIVEBROADCASTFRAME_H
#define ARGOS3_AIRTIGHT_NAIVEBROADCASTFRAME_H

#include "../generic/slot_radio_frame.h"

namespace argos {
    struct NaiveBroadcastFrame : SlotRadioFrame {
        using SlotRadioFrame::SlotRadioFrame;
        ~NaiveBroadcastFrame() override = default;
    };
}

#endif //ARGOS3_AIRTIGHT_NAIVEBROADCASTFRAME_H
