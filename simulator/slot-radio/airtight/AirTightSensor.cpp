#include "AirTightSensor.h"
#include "AirTightActuator.h"
#include <argos3/core/simulator/simulator.h>

#include <vector>
#include <tuple>

namespace argos {

    void AirTightSensor::Init(TConfigurationNode& t_tree) {
        SlotRadioSensor::Init(t_tree);
        actuator = robot->GetComponent<CControllableEntity>("controller").GetController().GetActuator<AirTightActuator>("slot_radio");
    }

    void AirTightSensor::Update() {
        receivedFrame.reset();
        if (rxPort != nullptr) {
            auto frame = dynamic_cast<const AirTightFrame*>(rxPort);

            // frame is nullptr is received frame was not an AirTightFrame
            if (frame == nullptr) {
                // We might eventually want some handling here at some point for mixed protocol tests, but just throw
                // for now.
                THROW_ARGOSEXCEPTION("AirTightSensor got frame that wasn't AirTightFrame");
            }
            else if (frame->frameTo == robot->GetId()) {
                receivedFrame.emplace(*frame);

                auto fwdIt = fwdMap.find(std::make_pair(receivedFrame->frameFrom, receivedFrame->frameOriginBuffer));
                if (fwdIt != fwdMap.end()) {
                    if (receivedFrame->queueTime != fwdIt->second.second) {
                        fwdIt->second.second = receivedFrame->queueTime;
                        actuator->QueueFrame(fwdIt->second.first, receivedFrame->frameMsg);
                    }
                    else {
                        //LOG << "Dropping duplicate frame, assuming failed ack\n";
                    }
                }
            }
        }
    }

    // TODO: Verify me
    void AirTightSensor::Reset() {
        receivedFrame.reset();
    }

    void AirTightSensor::SetRobotConfiguration(TConfigurationNode &t_tree) {
        auto& airTightConfig = GetNode(t_tree, "airtight");

        TConfigurationNodeIterator it;
        for(it = it.begin(&airTightConfig); it != it.end(); ++it) {
            if (it->Value() != "buffer") {
                THROW_ARGOSEXCEPTION("Unknown configuration element: " + it->Value());
            }

            auto name = it->GetAttribute("name");
            auto fwdFrom = it->GetAttribute("fwdFrom");
            auto fwdBuffer = it->GetAttribute("fwdBuffer");

            if (!fwdFrom.empty() && !fwdBuffer.empty()) {
                fwdMap[std::make_pair(fwdFrom, fwdBuffer)] = std::make_pair(name, 0);
            }
        }
    }

    bool AirTightSensor::HasFrame() {
        return (receivedFrame.has_value() && receivedFrame->frameTo == robot->GetId());
    }

    const std::any& AirTightSensor::GetMessage() {
        if (HasFrame()) {
            return receivedFrame->frameMsg;
        }
        else {
            THROW_ARGOSEXCEPTION("No message to get");
        }
    }

    REGISTER_SENSOR(AirTightSensor,
                    "slot_radio",
                    "airtight",
                    "Sven Signer",
                    "1.0",
                    "Brief Description",
                    "Long Description",
                    "status")
}