#include "AirTightSensor.h"
#include "AirTightActuator.h"
#include <argos3/core/simulator/simulator.h>

#include <vector>
#include <tuple>

namespace argos {

    void AirTightSensor::Init(TConfigurationNode& t_tree) {
        SlotRadioSensor::Init(t_tree);
        std::string mediumName;
        GetNodeAttribute(t_tree, "medium", mediumName);
        medium = &CSimulator::GetInstance().GetMedium<SlotRadioMedium>(mediumName);
        //rng = CRandom::CreateRNG("argos");
        actuator = robot->GetComponent<CControllableEntity>("controller").GetController().GetActuator<AirTightActuator>("slot_radio");
    }

    void AirTightSensor::Update() {
        auto frameStruct = medium->ReceiveFrame(this);

        if (frameStruct == nullptr) {
            receivedFrame.reset();
        }
        else {
            const auto &frame = frameStruct->frame;
            receivedFrame.emplace(frame);
            if (frame.frameTo == robot->GetId()) {
                medium->PushAck(frameStruct, this);

                auto fwdIt = fwdMap.find(std::make_pair(frame.frameFrom, frame.frameOriginBuffer));
                if (fwdIt != fwdMap.end()) {
                    if (frame.queueTime != fwdIt->second.second) {
                        fwdIt->second.second = frame.queueTime;
                        actuator->QueueFrame(fwdIt->second.first, frame.frameMsg);
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

    const RadioMessage& AirTightSensor::GetMessage() {
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