#include <argos3/core/simulator/simulator.h>
#include "NaiveP2PSensor.h"


namespace argos {
    void NaiveP2PSensor::Update() {
        receivedFrame.reset();

        auto frame = dynamic_cast<const NaiveP2PFrame*>(rxPort);
        if (frame != nullptr) {
            if (frame->frameTo == robot->GetId()) {
                receivedFrame.emplace(*frame);
            }
        }
    }

    void NaiveP2PSensor::Reset() {
        receivedFrame.reset();
    }

    bool NaiveP2PSensor::HasFrame() {
        return (receivedFrame.has_value() && receivedFrame->frameTo == robot->GetId());
    }

    const std::any& NaiveP2PSensor::GetMessage() {
        if (HasFrame()) {
            return receivedFrame->frameMsg;
        }
        else {
            THROW_ARGOSEXCEPTION("No frame to get");
        }
    }

    REGISTER_SENSOR(NaiveP2PSensor,
                    "slot_radio",
                    "naive_p2p",
                    "Sven Signer",
                    "1.0",
                    "Brief Description",
                    "Long Description",
                    "status")
}