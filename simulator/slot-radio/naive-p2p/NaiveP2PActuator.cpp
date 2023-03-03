#include "NaiveP2PActuator.h"
#include <argos3/core/simulator/simulator.h>

namespace argos {

    void NaiveP2PActuator::Init(TConfigurationNode &t_tree) {
        SlotRadioActuator::Init(t_tree);
        SetRequireCCA(true);
    }

    void NaiveP2PActuator::Update() {
        // If we didn't send a frame due to CCA failure, we no longer expect an ack
        if (expectAck && !SentFrame()) {
            expectAck = false;
        }

        // Got an ack
        if (expectAck && GotAck()) {
            queuedFrames.pop();
            retries = 0;
        }
        // Expected an ack that never happened
        else if (expectAck) {
            if (retries < maxRetries) {
                retries++;
                backoffRemaining = rng->Uniform(CRange<UInt32>(0, 1 << std::min(retries, maxBackoffExp)));
            }
            else {
                queuedFrames.pop();
                retries = 0;
            }
        }
        // Unexpected ack
        else if (GotAck()) {
            THROW_ARGOSEXCEPTION("Got unexpected ack?");
        }


        if (!queuedFrames.empty() && backoffRemaining == 0) {
            txPort = &queuedFrames.front();
            expectAck = true;
        }
        else {
            expectAck = false;
        }

        if (backoffRemaining > 0) {
            backoffRemaining--;
        }
    }

    void NaiveP2PActuator::Reset() {
        while (!queuedFrames.empty()) {
            queuedFrames.pop();
        }
        retries = 0;
        backoffRemaining = 0;
        expectAck = false;
    }

    void NaiveP2PActuator::QueueFrame(const std::string &buffer, const std::any& message) {
        const auto clock = CSimulator::GetInstance().GetSpace().GetSimulationClock();
        for(const auto& node : recipients[buffer]) {
            queuedFrames.emplace(robot->GetId(), node, message);
        }
    }

    void NaiveP2PActuator::SetRobotConfiguration(TConfigurationNode &t_tree) {
        auto& config = GetNode(t_tree, "naive_p2p");

        auto maxBackoffExpStr = config.GetAttribute("maxBackoffExp");
        auto maxRetriesStr = config.GetAttribute("maxRetries");

        if (maxBackoffExpStr.empty()) {
            THROW_ARGOSEXCEPTION("NaiveP2P requires maxBackoffExp parameter");
        }
        if (maxRetriesStr.empty()) {
            THROW_ARGOSEXCEPTION("NaiveP2P requires maxRetries parameter");
        }

        maxBackoffExp = std::stoi(maxBackoffExpStr);
        maxRetries = std::stoi(maxRetriesStr);

        TConfigurationNodeIterator it;
        for(it = it.begin(&config); it != it.end(); ++it) {
            if (it->Value() != "buffer") {
                THROW_ARGOSEXCEPTION("Unexpected config element "+it->Value());
            }
            recipients[it->GetAttribute("name")] = {};

            auto child  = it->FirstChild();
            while (child != nullptr) {
                if (child->Value() != "recipient") {
                    THROW_ARGOSEXCEPTION("Unexpected config element " + it->Value());
                }
                recipients[it->GetAttribute("name")].emplace_back(child->FirstChild()->Value());
                child = it->IterateChildren(child);
            }
        }
    }

    REGISTER_ACTUATOR(NaiveP2PActuator,
                      "slot_radio",
                      "naive_p2p",
                      "Sven Signer",
                      "1.0",
                      "Brief Description",
                      "Long Description",
                      "status")
}