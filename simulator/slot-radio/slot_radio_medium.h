#ifndef ARGOS3_AIRTIGHT_SLOT_RADIO_MEDIUM_H
#define ARGOS3_AIRTIGHT_SLOT_RADIO_MEDIUM_H

#include <map>
#include <set>
#include <argos3/core/simulator/entity/composable_entity.h>
#include <argos3/core/simulator/medium/medium.h>
#include <argos3/core/utility/logging/argos_log.h>
#include <argos3/core/utility/math/rng.h>

#include "generic/slot_radio_sensor.h"
#include "generic/slot_radio_actuator.h"


namespace argos {
    class SlotRadioMedium : public CMedium {
    private:
        struct InFlightFrame {
            RadioFrame frame;
            const SlotRadioActuator *origin;
        };
        std::vector<InFlightFrame> txFrames;
        std::vector<InFlightFrame> rxFrames;

        struct InFlightAck {
            const SlotRadioActuator* actuator;
            const SlotRadioSensor* sensor;
        };
        std::vector<InFlightAck> acks;

        CRandom::CRNG* rng;
        double pdrModifier = 1.0;

    public:
        SlotRadioMedium() : txFrames({}), rxFrames({}), rng(nullptr) {};

        void Init(TConfigurationNode& t_tree) override;
        void Update() override;
        void Reset() override;

        void PushFrame(const RadioFrame& frame, const SlotRadioActuator* origin);
        const InFlightFrame* ReceiveFrame(const SlotRadioSensor *sensor);
        //const InFlightFrame* ReceiveFrameNoCollisions(const SlotRadioSensor *sensor);
        bool CarrierSenseBusy(const SlotRadioActuator* actuator);

        void PushAck(const InFlightFrame* frame, const SlotRadioSensor* sensor);
        bool ReceiveAck(const SlotRadioActuator* actuator, bool checkCollisions = true);

        inline Real PDR(Real distance) const {
             //return 1.0;
            //return 0.99 / (1.0 + pow(distance, 2.0));
            return pdrModifier * ((distance < 1.5) ? 0.99 : 0.99 / (1.0 + pow((distance-1.5)/3.0, 2.0)));
        }
    };

}

#endif //ARGOS3_AIRTIGHT_SLOT_RADIO_MEDIUM_H
