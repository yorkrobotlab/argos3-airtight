#include "NaiveBroadcastActuator.h"
#include <argos3/core/simulator/simulator.h>

namespace argos {
    void NaiveBroadcastActuator::Init(TConfigurationNode &t_tree) {
        SlotRadioActuator::Init(t_tree);
        SetRequireCCA(true);
    }

    void NaiveBroadcastActuator::Update() {
        // If we transmitted a frame last time
        if (SentFrame()) {
            queuedFrames.pop();
        }

        if (!queuedFrames.empty()) {
            txPort = &queuedFrames.front();
        }
    }

    void NaiveBroadcastActuator::Reset() {
        while (!queuedFrames.empty()) {
            queuedFrames.pop();
        }
    }

    void NaiveBroadcastActuator::QueueFrame(const std::string &buffer, const std::any& message) {
        for(int i=0; i<=numRepeats; i++) {
            queuedFrames.emplace(robot->GetId(), "", message);
        }
    }

    void NaiveBroadcastActuator::SetRobotConfiguration(TConfigurationNode &t_tree) {
        auto& config = GetNode(t_tree, "naive_broadcast");
        auto numRepeatsString = config.GetAttribute("numRepeats");

        if (numRepeatsString.empty()) {
            THROW_ARGOSEXCEPTION("NaiveBroadcast requires numRepeats attribute");
        }

        numRepeats = std::stoi(numRepeatsString);
    }

    REGISTER_ACTUATOR(NaiveBroadcastActuator,
                      "slot_radio",
                      "naive_broadcast",
                      "Sven Signer",
                      "1.0",
                      "Broadcast a fixed-number of times",
                      "Broadcast each frame a fixed number of times, using a channel-clear assessment prior to each\n"
                      "transmission in an attempt to reduce collisions.",
                      "unstable")
}