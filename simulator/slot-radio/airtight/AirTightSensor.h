#ifndef ARGOS3_AIRTIGHT_AIRTIGHTSENSOR_H
#define ARGOS3_AIRTIGHT_AIRTIGHTSENSOR_H

#include <map>
#include <optional>
#include <argos3/core/simulator/sensor.h>
#include <argos3/core/utility/math/vector3.h>
#include <argos3/core/simulator/entity/composable_entity.h>
#include <argos3/core/utility/math/rng.h>

#include "AirTightBuffer.h"
#include "../slot_radio_medium.h"
#include "../generic/slot_radio_sensor.h"
#include "../generic/slot_radio_actuator.h"

namespace argos {
    class AirTightSensor : public SlotRadioSensor {
    public:
        AirTightSensor() : medium(nullptr), actuator(nullptr) {};

        void Init(TConfigurationNode& t_tree) override;
        void Update() override;
        void Reset() override;

        void SetRobotConfiguration(TConfigurationNode &t_tree) override;

        bool HasFrame() override;
        const RadioMessage& GetMessage() override;

    private:
        SlotRadioMedium* medium;
        std::optional<RadioFrame> receivedFrame;
        // Map <originNode, originBuffer> -> <outputBuffer, lastForwardedMessageQueueTime (for dedup)>
        std::map<std::pair<std::string, std::string>, std::pair<std::string, UInt32>> fwdMap;

        SlotRadioActuator *actuator;
    };

}

#endif //ARGOS3_AIRTIGHT_AIRTIGHTSENSOR_H