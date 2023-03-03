#ifndef ARGOS3_AIRTIGHT_SLOT_RADIO_SENSOR_H
#define ARGOS3_AIRTIGHT_SLOT_RADIO_SENSOR_H

#include <argos3/core/utility/math/rng.h>
#include <argos3/core/utility/math/vector3.h>
#include <argos3/core/simulator/sensor.h>
#include <argos3/core/simulator/simulator.h>
#include <argos3/core/simulator/entity/composable_entity.h>
#include "../control_interface/ci_slot_radio_sensor.h"
#include "slot_radio_medium.h"

namespace argos {
    class SlotRadioMedium;

    class SlotRadioSensor : public CSimulatedSensor, public CI_SlotRadioSensor {
    public:
        friend class SlotRadioMedium;

        void Init(TConfigurationNode& t_tree) override {
            rng = CRandom::CreateRNG("argos");

            std::string mediumName;
            GetNodeAttribute(t_tree, "medium", mediumName);
            medium = &CSimulator::GetInstance().GetMedium<SlotRadioMedium>(mediumName);
            medium->RegisterSensor(this);
        }

        void SetRobot(CComposableEntity &c_entity) override {
            robot = &c_entity;
        };

    protected:
        CComposableEntity *robot = nullptr;
        CRandom::CRNG* rng = nullptr;
        SlotRadioMedium* medium = nullptr;
        const SlotRadioFrame* rxPort = nullptr;

        bool SendsAcks() {return sendAcks;}
        void SetSendsAcks(bool value) {sendAcks = value;}

    private:
        bool sendAcks = true;

        const CVector3& GetPosition() const{
            return robot->GetComponent<CEmbodiedEntity>("body").GetOriginAnchor().Position;
        }
    };
}

#endif //ARGOS3_AIRTIGHT_SLOT_RADIO_SENSOR_H
