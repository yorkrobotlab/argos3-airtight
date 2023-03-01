#ifndef ARGOS3_AIRTIGHT_AIRTIGHTACTUATOR_H
#define ARGOS3_AIRTIGHT_AIRTIGHTACTUATOR_H

#include <any>
#include <atomic>
#include <map>
#include <vector>
#include <argos3/core/simulator/actuator.h>
#include <argos3/core/utility/math/vector3.h>
#include <argos3/core/simulator/entity/composable_entity.h>

#include "AirTightBuffer.h"
#include "../generic/slot_radio_medium.h"
#include "../generic/slot_radio_actuator.h"

namespace argos {
    class AirTightActuator : public SlotRadioActuator {

    public:
        void Update() override;

        void Reset() override;

        void SetRobotConfiguration(TConfigurationNode &t_tree) override;

        void QueueFrame(const std::string &bufferName, const std::any& message) override;

    private:
        std::vector<bool> txSlots;
        std::vector<AirTightBuffer> buffers;
        AirTightBuffer *txBuffer = nullptr;
        std::map<std::string, UInt8> bufferNameIDLookup;

        bool highCriticalityMode = false;

        inline UInt8 GetBufferID(const std::string &bufferName) {
            return bufferNameIDLookup.at(bufferName);
        }

        UInt32 busyPeriodDuration = 0;
        UInt32 failedTransmissions = 0;

        UInt32 FaultLoad(UInt32 slots, bool highCriticality);
    };
};

#endif //ARGOS3_AIRTIGHT_AIRTIGHTACTUATOR_H
