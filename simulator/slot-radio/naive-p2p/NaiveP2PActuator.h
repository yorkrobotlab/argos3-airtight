#ifndef ARGOS3_AIRTIGHT_NaiveP2PACTUATOR_H
#define ARGOS3_AIRTIGHT_NaiveP2PACTUATOR_H

#include <any>
#include <queue>
#include <argos3/core/utility/math/rng.h>

#include "slot-radio/generic/slot_radio_actuator.h"
#include "slot-radio/generic/slot_radio_medium.h"
#include "NaiveP2PFrame.h"

namespace argos {
    class NaiveP2PActuator : public SlotRadioActuator {
    public:
        void Init(TConfigurationNode& t_tree) override;
        void Update() override;
        void Reset() override;

        void QueueFrame(const std::string& buffer, const std::any& message) override;

        void SetRobotConfiguration(TConfigurationNode &t_tree) override;

    private:
        std::queue<NaiveP2PFrame> queuedFrames;
        std::map<std::string, std::vector<std::string>> recipients;

        bool expectAck = false;
        UInt32 backoffRemaining = 0;
        UInt32 retries = 0;
        UInt32 maxBackoffExp = 0;
        UInt32 maxRetries = 0;
    };
}

#endif //ARGOS3_AIRTIGHT_NaiveP2PACTUATOR_H
