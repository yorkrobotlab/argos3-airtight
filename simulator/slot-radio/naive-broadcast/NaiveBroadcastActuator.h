#ifndef ARGOS3_AIRTIGHT_NAIVEBROADCASTACTUATOR_H
#define ARGOS3_AIRTIGHT_NAIVEBROADCASTACTUATOR_H

#include <any>
#include <optional>
#include <queue>
#include "slot-radio/generic/slot_radio_actuator.h"
#include "slot-radio/generic/slot_radio_medium.h"
#include "NaiveBroadcastFrame.h"

namespace argos {
    class NaiveBroadcastActuator : public SlotRadioActuator {
    public:
        void Init(TConfigurationNode& t_tree) override;
        void Update() override;
        void Reset() override;

        void QueueFrame(const std::string& buffer, const std::any& message) override;

        void SetRobotConfiguration(TConfigurationNode &t_tree) override;

    private:
        std::queue<NaiveBroadcastFrame> queuedFrames;
        UInt8 numRepeats;
    };
}

#endif //ARGOS3_AIRTIGHT_NAIVEBROADCASTACTUATOR_H
