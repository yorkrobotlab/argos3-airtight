#ifndef ARGOS3_AIRTIGHT_NaiveP2PSENSOR_H
#define ARGOS3_AIRTIGHT_NaiveP2PSENSOR_H

#include <optional>
#include "../slot_radio_medium.h"
#include "../generic/slot_radio_sensor.h"

namespace argos {

    class NaiveP2PSensor : public SlotRadioSensor  {
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

#endif //ARGOS3_AIRTIGHT_NaiveP2PSENSOR_H
