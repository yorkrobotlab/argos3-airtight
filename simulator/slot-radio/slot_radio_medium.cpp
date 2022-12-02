#include "slot_radio_medium.h"

namespace argos{

    void SlotRadioMedium::Init(TConfigurationNode& t_tree) {
        CMedium::Init(t_tree);
        rng = CRandom::CreateRNG("argos");
        pdrModifier = std::stod(t_tree.GetAttributeOrDefault("pdrModifier", "1.0"));
    }

    void SlotRadioMedium::Update() {
        acks.clear();
        rxFrames.clear();
        txFrames.swap(rxFrames);
    }

    void SlotRadioMedium::Reset() {
        acks.clear();
        txFrames.clear();
        rxFrames.clear();
    }

    void SlotRadioMedium::PushFrame(const RadioFrame &frame, const SlotRadioActuator *origin) {
        // Need a mutex here to not die horribly if we're running with threads
        txFrames.emplace_back(InFlightFrame {frame, origin});
    }

    const SlotRadioMedium::InFlightFrame* SlotRadioMedium::ReceiveFrame(const SlotRadioSensor *sensor) {
        const InFlightFrame* returnFrame = nullptr;

        auto& sensorPosition = sensor->GetPosition();
        for(const auto& frame : rxFrames) {
            auto &originPosition = frame.origin->GetPosition();
            Real distance = (sensorPosition - originPosition).Length();

            if (rng->Uniform(CRange<Real>(0.0, 1.0)) < PDR(distance)) {
                // No collision
                if (returnFrame == nullptr) {
                    returnFrame = &frame;
                } else {
                    return nullptr;
                }
            }
        }
        return returnFrame;
    }

    /*const SlotRadioMedium::InFlightFrame* SlotRadioMedium::ReceiveFrameNoCollisions(const SlotRadioSensor *sensor) {
        std::set<std::pair<Real, const InFlightFrame*>> receivedFrames;
        auto& sensorPosition = sensor->GetPosition();

        for(const auto& frame : rxFrames) {
            auto& originPosition = frame.origin->GetPosition();
            Real distance = (sensorPosition - originPosition).Length();

            if (rng->Uniform(CRange<Real>(0.0, 1.0)) < PDR(distance)) {
                receivedFrames.emplace(std::make_pair(0.0, &frame));
            }
        }

        if (receivedFrames.empty()) {
            return nullptr;
        }
        else {
            return receivedFrames.begin()->second;
        }
    }*/

    void SlotRadioMedium::PushAck(const InFlightFrame* frame, const SlotRadioSensor* sensor) {
        acks.emplace_back(InFlightAck {frame->origin, sensor});
    }

    bool SlotRadioMedium::ReceiveAck(const SlotRadioActuator* actuator, bool checkCollisions) {
        bool gotAck = false;
        bool collision = false;
        auto& actuatorPos = actuator->GetPosition();

        for(const auto& ack : acks) {
            auto& ackPosition = ack.sensor->GetPosition();
            Real distance = (actuatorPos - ackPosition).Length();

            if (rng->Uniform(CRange<Real>(0.0, 1.0)) < PDR(distance)) {
                if (ack.actuator == actuator) {
                    gotAck = true;
                }
                else {
                    collision = true;
                }
            }
        }

        return gotAck && !(collision && checkCollisions);
    }

    // FIXME: This is sort of a cheat; it works in single-threaded mode, modelling nodes having inter-frame spacings
    //  according to their ARGoS execution order. In multi-threaded mode, however, it would lead to horrible race
    //  conditions and return completely invalid results.
    bool SlotRadioMedium::CarrierSenseBusy(const SlotRadioActuator *actuator) {
        const auto& sensePosition = actuator->GetPosition();
        for(const auto& frame : txFrames) {
            const auto& txPosition = frame.origin->GetPosition();
            Real distance = (sensePosition - txPosition).Length();
            if (rng->Uniform(CRange<Real>(0.0, 1.0)) < PDR(distance)) {
                return true;
            }
        }
        return false;
    }

    REGISTER_MEDIUM(SlotRadioMedium,
                    "slot_radio",
                    "Sven Signer",
                    "1.0",
                    "Breif Description",
                    "Long Description",
                    "status")
}