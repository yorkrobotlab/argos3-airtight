#include "NaiveP2PActuator.h"
#include <argos3/core/simulator/simulator.h>

namespace argos {
    void NaiveP2PActuator::Init(TConfigurationNode &t_tree) {
        SlotRadioActuator::Init(t_tree);
        std::string mediumName;
        GetNodeAttribute(t_tree, "medium", mediumName);
        medium = &CSimulator::GetInstance().GetMedium<SlotRadioMedium>(mediumName);
    }

    void NaiveP2PActuator::Update() {
        bool gotAck = medium->ReceiveAck(this);

        if (expectAck && gotAck) {
            queuedFrames.pop();
            retries = 0;
        }
        else if (gotAck) {
            THROW_ARGOSEXCEPTION("Got unexpected ack?");
        }
        else if (expectAck) {
            if (retries < maxRetries) {
                retries++;
                backoffRemaining = rng->Uniform(CRange<UInt32>(0, 1 << std::min(retries, maxBackoffExp)));
                //LOG << "BACKOFF: " << backoffRemaining << "\n";
            }
            else {
                queuedFrames.pop();
                retries = 0;
            }
        }

        if (!queuedFrames.empty() && backoffRemaining == 0 && !(medium->CarrierSenseBusy(this))) {
            medium->PushFrame(queuedFrames.front(), this);
            expectAck = true;
        }
        else {
            expectAck = false;
        }

        if (backoffRemaining > 0) {
            //LOG << "Backoff: " << backoffRemaining << "\n";
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

    void NaiveP2PActuator::QueueFrame(const std::string &buffer, const RadioMessage &message) {
        const auto clock = CSimulator::GetInstance().GetSpace().GetSimulationClock();
        for(const auto& node : recipients) {
            queuedFrames.emplace(RadioFrame {robot->GetId(), node, "", message, clock});
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
            if (it->Value() != "recipient") {
                THROW_ARGOSEXCEPTION("Unknown configuration element: " + it->Value());
            }

            recipients.emplace_back(it->GetText());
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