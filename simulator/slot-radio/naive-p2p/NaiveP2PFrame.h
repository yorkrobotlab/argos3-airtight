#ifndef ARGOS3_AIRTIGHT_NAIVEP2PFRAME_H
#define ARGOS3_AIRTIGHT_NAIVEP2PFRAME_H

#include "../generic/slot_radio_frame.h"

namespace argos {
    struct NaiveP2PFrame : SlotRadioFrame {
        using SlotRadioFrame::SlotRadioFrame;
        ~NaiveP2PFrame() override = default;
    };
}

#endif //ARGOS3_AIRTIGHT_NAIVEP2PFRAME_H
