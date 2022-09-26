#ifndef ARGOS3_AIRTIGHT_SLOT_RADIO_ACTUATOR_H
#define ARGOS3_AIRTIGHT_SLOT_RADIO_ACTUATOR_H

#include <argos3/core/utility/math/rng.h>
#include <argos3/core/utility/math/vector3.h>
#include <argos3/core/simulator/actuator.h>
#include <argos3/core/simulator/entity/composable_entity.h>
#include <argos3/core/simulator/simulator.h>
#include "../control_interface/ci_slot_radio_actuator.h"


namespace argos {
    class SlotRadioActuator : public CSimulatedActuator, public CI_SlotRadioActuator {
    public:
        void Init(TConfigurationNode& t_tree) override {
            rng = CRandom::CreateRNG("argos");
        }

        void SetRobot(CComposableEntity &c_entity) override {
            robot = &c_entity;
        };

        virtual const CVector3& GetPosition() const {
            return robot->GetComponent<CEmbodiedEntity>("body").GetOriginAnchor().Position;
        }

    protected:
        CComposableEntity *robot;
        CRandom::CRNG* rng = nullptr;
    };
}


#endif //ARGOS3_AIRTIGHT_SLOT_RADIO_ACTUATOR_H
