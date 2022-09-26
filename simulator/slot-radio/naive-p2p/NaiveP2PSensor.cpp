#include <argos3/core/simulator/simulator.h>
#include "NaiveP2PSensor.h"


namespace argos {
    void NaiveP2PSensor::Init(TConfigurationNode &t_tree) {
        SlotRadioSensor::Init(t_tree);
        std::string mediumName;
        GetNodeAttribute(t_tree, "medium", mediumName);
        medium = &CSimulator::GetInstance().GetMedium<SlotRadioMedium>(mediumName);
    }

    void NaiveP2PSensor::Update() {
        auto frameStruct = medium->ReceiveFrame(this);

        if (frameStruct != nullptr) {
            if (frameStruct->frame.frameTo == robot->GetId()) {
                medium->PushAck(frameStruct, this);
            }
            receivedFrame.emplace(frameStruct->frame);
        }
    }

    void NaiveP2PSensor::Reset() {
        receivedFrame.reset();
    }

    bool NaiveP2PSensor::HasFrame() {
        return (receivedFrame.has_value() && receivedFrame->frameTo == robot->GetId());
    }

    const RadioMessage &NaiveP2PSensor::GetMessage() {
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
} // argos