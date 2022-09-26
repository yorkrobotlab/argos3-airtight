#include <argos3/core/simulator/simulator.h>
#include "NaiveBroadcastSensor.h"


namespace argos {
    void NaiveBroadcastSensor::Init(TConfigurationNode &t_tree) {
        SlotRadioSensor::Init(t_tree);
        std::string mediumName;
        GetNodeAttribute(t_tree, "medium", mediumName);
        medium = &CSimulator::GetInstance().GetMedium<SlotRadioMedium>(mediumName);
    }

    void NaiveBroadcastSensor::Update() {
        auto frameStruct = medium->ReceiveFrame(this);

        if (frameStruct == nullptr) {
            receivedFrame.reset();
        }
        else {
            receivedFrame.emplace(frameStruct->frame);
        }
    }

    void NaiveBroadcastSensor::Reset() {
        receivedFrame.reset();
    }

    bool NaiveBroadcastSensor::HasFrame() {
        return (receivedFrame.has_value() && receivedFrame->frameFrom != robot->GetId());
    }

    const RadioMessage &NaiveBroadcastSensor::GetMessage() {
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
} // argos