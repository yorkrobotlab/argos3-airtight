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
        AirTightBuffer *txBuffer;
        std::map<std::string, UInt8> bufferNameIDLookup;

        bool highCriticalityMode = false;

        inline UInt8 GetBufferID(const std::string &bufferName) {
            return bufferNameIDLookup.at(bufferName);
        }

        UInt32 busyPeriodDuration = 0;
        UInt32 failedTransmissions = 0;

        inline static double binomial(const double n, const double k) {
            return exp(lgamma(n+1)-lgamma(k+1)-lgamma(n-k+1));
        }

        std::vector<UInt32> faultLoadLUTS[2];
        inline UInt32 FaultLoadDYNLUT(bool critLevel, UInt32 slots) {
            if (slots < faultLoadLUTS[critLevel].size()) {
                return faultLoadLUTS[critLevel][slots];
            }
            else {
                LOG << "[" << robot->GetId() << "] Extending FLLUT " << critLevel << "to size " << slots << "\n";
                for(UInt32 i = faultLoadLUTS[critLevel].size(); i <= slots; i++) {
                    if (i != 0) {
                        faultLoadLUTS[critLevel].emplace_back(
                                FaultLoad(critLevel, slots, faultLoadLUTS[critLevel].back()));
                    }
                    else {
                        faultLoadLUTS[critLevel].emplace_back(FaultLoad(critLevel, slots));
                    }
                }
                return faultLoadLUTS[critLevel][slots];
            }
        }

        UInt32 FaultLoad(bool critLevel, UInt32 slots, UInt32 hint=0);
    };
};

#endif //ARGOS3_AIRTIGHT_AIRTIGHTACTUATOR_H
