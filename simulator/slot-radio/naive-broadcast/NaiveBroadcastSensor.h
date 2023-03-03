#ifndef ARGOS3_AIRTIGHT_NAIVEBROADCASTSENSOR_H
#define ARGOS3_AIRTIGHT_NAIVEBROADCASTSENSOR_H

#include <any>
#include <optional>

#include "slot-radio/generic/slot_radio_medium.h"
#include "slot-radio/generic/slot_radio_sensor.h"
#include "NaiveBroadcastFrame.h"

namespace argos {

    class NaiveBroadcastSensor : public SlotRadioSensor  {
    public:
        void Update() override;
        void Reset() override;

        void SetRobotConfiguration(TConfigurationNode &t_tree) override {};

        bool HasFrame() override;
        const std::any& GetMessage() override;

    private:
        std::optional<NaiveBroadcastFrame> receivedFrame;
    };

} // argos

#endif //ARGOS3_AIRTIGHT_NAIVEBROADCASTSENSOR_H
