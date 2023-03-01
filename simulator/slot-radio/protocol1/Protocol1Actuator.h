#ifndef ARGOS3_AIRTIGHT_PROTOCOL1ACTUATOR_H
#define ARGOS3_AIRTIGHT_PROTOCOL1ACTUATOR_H

#include <any>
#include <atomic>
#include <map>
#include <vector>
#include <argos3/core/simulator/actuator.h>
#include <argos3/core/utility/math/vector3.h>
#include <argos3/core/simulator/entity/composable_entity.h>

#include "Protocol1Buffer.h"
#include "../generic/slot_radio_medium.h"
#include "../generic/slot_radio_actuator.h"

namespace argos {
    class Protocol1Actuator : public SlotRadioActuator {

    public:
        friend class Protocol1Sensor;

        void Update() override;

        void Reset() override;

        void SetRobot(CComposableEntity &robot) override;

        void SetRobotConfiguration(TConfigurationNode &t_tree) override;

        void QueueFrame(const std::string &bufferName, const std::any& message) override;

        bool isInHighCritMode() const {
            return highCriticalityMode;
        }

    private:
        void SwitchHC();
        void HandleDelayedAck(const Protocol1Frame *frame);

        std::vector<bool> txSlots;
        std::vector<bool> pendingAcks;
        std::vector<Protocol1Buffer> buffers;
        Protocol1Buffer *txBuffer = nullptr;
        Protocol1Frame *dummyFrame = nullptr;
        std::map<std::string, UInt8> bufferNameIDLookup;

        bool highCriticalityMode = false;

        inline UInt8 GetBufferID(const std::string &bufferName) {
            return bufferNameIDLookup.at(bufferName);
        }

        UInt32 busyPeriodDuration = 0;
        UInt32 failedTransmissions = 0;
    };
};

#endif //ARGOS3_AIRTIGHT_PROTOCOL1ACTUATOR_H
