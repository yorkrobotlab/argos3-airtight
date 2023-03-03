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
            return pdrModifier * ((distance < 1.5) ? 0.99 : 0.99 / (1.0 + pow((distance-1.5)/3.0, 2.0)));
        }

    private:
        std::set<SlotRadioActuator*> actuators;
        std::set<SlotRadioSensor*> sensors;

        void RegisterActuator(SlotRadioActuator* actuator);
        void RegisterSensor(SlotRadioSensor* sensor);

        CRandom::CRNG* rng = nullptr;
        double pdrModifier = 1.0;
    };

}

#endif //ARGOS3_AIRTIGHT_SLOT_RADIO_MEDIUM_H
