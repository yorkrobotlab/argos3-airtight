#include <argos3/core/simulator/simulator.h>
#include "NaiveBroadcastSensor.h"

namespace argos {
    void NaiveBroadcastSensor::Update() {
        receivedFrame.reset();

        auto frame = dynamic_cast<const NaiveBroadcastFrame*>(rxPort);
        if (frame != nullptr) {
            receivedFrame.emplace(*frame);
        }
    }

    void NaiveBroadcastSensor::Reset() {
        receivedFrame.reset();
    }

    bool NaiveBroadcastSensor::HasFrame() {
        return (receivedFrame.has_value() && receivedFrame->frameFrom != robot->GetId());
    }

    const std::any& NaiveBroadcastSensor::GetMessage() {
        if (HasFrame()) {
            return receivedFrame->frameMsg;
        }
        else {
            THROW_ARGOSEXCEPTION("No frame to get");
        }
    }

    REGISTER_SENSOR(NaiveBroadcastSensor,
                    "slot_radio",
                    "naive_broadcast",
                    "Sven Signer",
                    "1.0",
                    "Brief Description",
                    "Long Description",
                    "status")
}