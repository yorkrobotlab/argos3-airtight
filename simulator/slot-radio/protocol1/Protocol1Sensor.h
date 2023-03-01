#ifndef ARGOS3_AIRTIGHT_PROTOCOL1SENSOR_H
#define ARGOS3_AIRTIGHT_PROTOCOL1SENSOR_H

#include <map>
#include <optional>
#include <argos3/core/simulator/sensor.h>
#include <argos3/core/utility/math/vector3.h>
#include <argos3/core/simulator/entity/composable_entity.h>
#include <argos3/core/utility/math/rng.h>


#include "../generic/slot_radio_medium.h"
#include "../generic/slot_radio_sensor.h"
#include "../generic/slot_radio_actuator.h"
#include "Protocol1Actuator.h"
#include "Protocol1Buffer.h"

namespace argos {
    class Protocol1Sensor : public SlotRadioSensor {
    public:
        void Init(TConfigurationNode& t_tree) override;
        void Update() override;
        void Reset() override;

        void SetRobotConfiguration(TConfigurationNode &t_tree) override {};

        bool HasFrame() override;
        const std::any& GetMessage() override;

    private:
        std::optional<Protocol1Frame> receivedFrame;
        std::map<std::pair<std::string, std::string>, std::pair<std::string, UInt32>> fwdMap;

        Protocol1Actuator *actuator;
    };

}

#endif //ARGOS3_AIRTIGHT_PROTOCOL1SENSOR_H
