#ifndef ARGOS3_AIRTIGHT_NaiveP2PACTUATOR_H
#define ARGOS3_AIRTIGHT_NaiveP2PACTUATOR_H

#include <queue>
#include <argos3/core/utility/math/rng.h>
#include "../slot_radio_medium.h"

namespace argos {
    class NaiveP2PActuator : public SlotRadioActuator {
    public:
        void Init(TConfigurationNode &t_tree) override;
        void Update() override;
        void Reset() override;

        void QueueFrame(const std::string& buffer, const RadioMessage& message) override;

        void SetRobotConfiguration(TConfigurationNode &t_tree) override;

    private:
        SlotRadioMedium* medium;
        std::queue<RadioFrame> queuedFrames;
        std::vector<std::string> recipients;

        bool expectAck = false;
        UInt32 backoffRemaining = 0;
        UInt32 retries = 0;
        UInt32 maxBackoffExp = 0;
        UInt32 maxRetries = 0;
    };
}

#endif //ARGOS3_AIRTIGHT_NaiveP2PACTUATOR_H
