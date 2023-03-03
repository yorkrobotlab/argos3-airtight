#ifndef ARGOS3_AIRTIGHT_SLOT_RADIO_ACTUATOR_H
#define ARGOS3_AIRTIGHT_SLOT_RADIO_ACTUATOR_H

#include <argos3/core/utility/math/rng.h>
#include <argos3/core/utility/math/vector3.h>
#include <argos3/core/simulator/actuator.h>
#include <argos3/core/simulator/entity/composable_entity.h>
#include <argos3/core/simulator/simulator.h>
#include "../control_interface/ci_slot_radio_actuator.h"
#include "slot_radio_frame.h"
#include "slot_radio_medium.h"

namespace argos {
    class SlotRadioActuator : public CSimulatedActuator, public CI_SlotRadioActuator {
    public:
        friend class SlotRadioMedium;

        void Init(TConfigurationNode& t_tree) override {
            rng = CRandom::CreateRNG("argos");

            std::string mediumName;
            GetNodeAttribute(t_tree, "medium", mediumName);
            medium = &CSimulator::GetInstance().GetMedium<SlotRadioMedium>(mediumName);
            medium->RegisterActuator(this);
        }

        void SetRobot(CComposableEntity &c_entity) override {
            robot = &c_entity;
        };

    protected:
        CComposableEntity* robot = nullptr;
        CRandom::CRNG* rng = nullptr;
        SlotRadioMedium* medium = nullptr;

        // Pointer to a frame to transmit, must remain valid until the next medium and sensor update loops have
        // completed.
        const SlotRadioFrame* txPort = nullptr;

        bool GotAck() const {return gotAck;}
        bool SentFrame() const {return sentFrame;}
        bool GetRequireCCA() const {return requireCCA;}
        void SetRequireCCA(bool value) {requireCCA = value;}

    private:
        bool requireCCA = false;
        bool sentFrame = false;
        bool gotAck = false;

        const CVector3& GetPosition() const {
            return robot->GetComponent<CEmbodiedEntity>("body").GetOriginAnchor().Position;
        }
    };
}

#endif //ARGOS3_AIRTIGHT_SLOT_RADIO_ACTUATOR_H
