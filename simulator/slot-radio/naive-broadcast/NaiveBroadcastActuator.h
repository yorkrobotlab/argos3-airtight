#ifndef ARGOS3_AIRTIGHT_NAIVEBROADCASTACTUATOR_H
#define ARGOS3_AIRTIGHT_NAIVEBROADCASTACTUATOR_H

#include <queue>
#include "../slot_radio_medium.h"

namespace argos {
    class NaiveBroadcastActuator : public SlotRadioActuator {
    public:
        void Init(TConfigurationNode &t_tree) override;
        void Update() override;
        void Reset() override;

        void QueueFrame(const std::string& buffer, const RadioMessage& message) override;

        void SetRobotConfiguration(TConfigurationNode &t_tree) override;

    private:
        SlotRadioMedium* medium;
        std::queue<RadioFrame> queuedFrames;
        UInt8 numRepeats;
    };
}

#endif //ARGOS3_AIRTIGHT_NAIVEBROADCASTACTUATOR_H
