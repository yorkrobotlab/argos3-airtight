#ifndef ARGOS3_AIRTIGHT_NaiveP2PSENSOR_H
#define ARGOS3_AIRTIGHT_NaiveP2PSENSOR_H

#include <any>
#include <optional>

#include "slot-radio/generic/slot_radio_medium.h"
#include "slot-radio/generic/slot_radio_sensor.h"
#include "NaiveP2PFrame.h"

namespace argos {

    class NaiveP2PSensor : public SlotRadioSensor  {
    public:
        void Update() override;
        void Reset() override;

        void SetRobotConfiguration(TConfigurationNode &t_tree) override {};

        bool HasFrame() override;
        const std::any& GetMessage() override;

    private:
        std::optional<NaiveP2PFrame> receivedFrame;
    };

} // argos

#endif //ARGOS3_AIRTIGHT_NaiveP2PSENSOR_H
