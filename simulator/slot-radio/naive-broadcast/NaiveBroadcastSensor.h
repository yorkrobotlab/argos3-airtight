#ifndef ARGOS3_AIRTIGHT_NAIVEBROADCASTSENSOR_H
#define ARGOS3_AIRTIGHT_NAIVEBROADCASTSENSOR_H

#include <optional>
//#include <argos3/core/simulator/sensor.h>
//#include <argos3/core/simulator/entity/composable_entity.h>
//#include "control_interface/ci_slot_radio_sensor.h"
#include "../slot_radio_medium.h"
#include "../generic/slot_radio_sensor.h"

namespace argos {

    class NaiveBroadcastSensor : public SlotRadioSensor  {
    public:
        void Init(TConfigurationNode& t_tree) override;
        void Update() override;
        void Reset() override;

        void SetRobotConfiguration(TConfigurationNode &t_tree) override {};

        bool HasFrame() override;
        const RadioMessage& GetMessage() override;

    private:
        SlotRadioMedium* medium;
        std::optional<RadioFrame> receivedFrame;
    };

} // argos

#endif //ARGOS3_AIRTIGHT_NAIVEBROADCASTSENSOR_H
