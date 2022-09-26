#ifndef ARGOS3_AIRTIGHT_SLOT_RADIO_SENSOR_H
#define ARGOS3_AIRTIGHT_SLOT_RADIO_SENSOR_H

#include <argos3/core/utility/math/rng.h>
#include <argos3/core/utility/math/vector3.h>
#include <argos3/core/simulator/sensor.h>
#include <argos3/core/simulator/simulator.h>
#include <argos3/core/simulator/entity/composable_entity.h>
#include "../control_interface/ci_slot_radio_sensor.h"

namespace argos {
    class SlotRadioSensor : public CSimulatedSensor, public CI_SlotRadioSensor {
    public:
        void Init(TConfigurationNode& t_tree) override {
            rng = CRandom::CreateRNG("argos");
        }

        void SetRobot(CComposableEntity &c_entity) override {
            robot = &c_entity;
        };

        virtual const CVector3& GetPosition() const{
            return robot->GetComponent<CEmbodiedEntity>("body").GetOriginAnchor().Position;
        }

    protected:
        CComposableEntity *robot;
        CRandom::CRNG* rng = nullptr;
    };
}

#endif //ARGOS3_AIRTIGHT_SLOT_RADIO_SENSOR_H
