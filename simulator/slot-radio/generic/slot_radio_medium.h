#ifndef ARGOS3_AIRTIGHT_SLOT_RADIO_MEDIUM_H
#define ARGOS3_AIRTIGHT_SLOT_RADIO_MEDIUM_H

#include <map>
#include <set>
#include <argos3/core/simulator/entity/composable_entity.h>
#include <argos3/core/simulator/medium/medium.h>
#include <argos3/core/utility/logging/argos_log.h>
#include <argos3/core/utility/math/rng.h>

#include "slot_radio_frame.h"

namespace argos {
    class SlotRadioActuator;
    class SlotRadioSensor;

    class SlotRadioMedium : public CMedium {
    public:
        friend class SlotRadioActuator;
        friend class SlotRadioSensor;

        SlotRadioMedium() = default;

        void Init(TConfigurationNode& t_tree) override;
        void Update() override;
        void Reset() override;

        inline Real PDR(Real distance) const {
            auto k = (distance - dropoffStart) / dropoffFactor;
            return pdrModifier * ((distance < dropoffStart) ? 1.0 : 1.0 / (1.0 + k*k));
        }

    private:
        std::map<std::string, SlotRadioActuator*> actuators;
        std::map<std::string, SlotRadioSensor*> sensors;

        void RegisterActuator(SlotRadioActuator* actuator);
        void RegisterSensor(SlotRadioSensor* sensor);

        CRandom::CRNG* rng = nullptr;
        double pdrModifier, dropoffStart, dropoffFactor;
    };

}

#endif //ARGOS3_AIRTIGHT_SLOT_RADIO_MEDIUM_H
