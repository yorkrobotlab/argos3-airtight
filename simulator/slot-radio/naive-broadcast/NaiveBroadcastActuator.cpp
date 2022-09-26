#include "NaiveBroadcastActuator.h"
#include <argos3/core/simulator/simulator.h>

namespace argos {
    void NaiveBroadcastActuator::Init(TConfigurationNode &t_tree) {
        SlotRadioActuator::Init(t_tree);
        std::string mediumName;
        GetNodeAttribute(t_tree, "medium", mediumName);
        medium = &CSimulator::GetInstance().GetMedium<SlotRadioMedium>(mediumName);
    }

    void NaiveBroadcastActuator::Update() {
        if (!queuedFrames.empty() && !(medium->CarrierSenseBusy(this))) {
            medium->PushFrame(queuedFrames.front(), this);
            queuedFrames.pop();
        }
    }

    void NaiveBroadcastActuator::Reset() {
        while (!queuedFrames.empty()) {
            queuedFrames.pop();
        }
    }


    void NaiveBroadcastActuator::QueueFrame(const std::string &buffer, const RadioMessage &message) {
        const auto clock = CSimulator::GetInstance().GetSpace().GetSimulationClock();
        for(int i=0; i<=numRepeats; i++) {
            queuedFrames.emplace(RadioFrame{robot->GetId(), "", "", message, clock});
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
                      "Brief Description",
                      "Long Description",
                      "status")
}