#include "Protocol1Sensor.h"
#include "Protocol1Actuator.h"
#include <argos3/core/simulator/simulator.h>

#include <vector>
#include <tuple>

namespace argos {

    void Protocol1Sensor::Init(TConfigurationNode& t_tree) {
        SlotRadioSensor::Init(t_tree);
        actuator = robot->GetComponent<CControllableEntity>("controller").GetController().GetActuator<Protocol1Actuator>("slot_radio");
    }

    void Protocol1Sensor::Update() {
        receivedFrame.reset();

        auto frame = dynamic_cast<const Protocol1Frame*>(rxPort);
        if (frame != nullptr && frame->frameFrom != robot->GetId()) {
            // Check the message wasn't only an ack
            if (frame->frameMsg.has_value()) {
                receivedFrame.emplace(*frame);
            }
            actuator->HandleDelayedAck(frame);
        }
    }

    // FIXME: This is incomplete
    void Protocol1Sensor::Reset() {
        receivedFrame.reset();
    }

    bool Protocol1Sensor::HasFrame() {
        return (receivedFrame.has_value());
    }

    const std::any& Protocol1Sensor::GetMessage() {
        if (HasFrame()) {
            return receivedFrame->frameMsg;
        }
        else {
            THROW_ARGOSEXCEPTION("No message to get");
        }
    }

    REGISTER_SENSOR(Protocol1Sensor,
                    "slot_radio",
                    "protocol1",
                    "Sven Signer",
                    "1.0",
                    "Brief Description",
                    "Long Description",
                    "status")
}