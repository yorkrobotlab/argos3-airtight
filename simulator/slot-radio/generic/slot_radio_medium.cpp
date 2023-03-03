#include "slot_radio_medium.h"
#include "slot_radio_actuator.h"
#include "slot_radio_sensor.h"

namespace argos{

    void SlotRadioMedium::RegisterActuator(SlotRadioActuator *actuator) {
        actuators.emplace(actuator);
    }

    void SlotRadioMedium::RegisterSensor(SlotRadioSensor *sensor) {
        sensors.emplace(sensor);
    }

    void SlotRadioMedium::Init(TConfigurationNode& t_tree) {
        CMedium::Init(t_tree);
        rng = CRandom::CreateRNG("argos");
        pdrModifier = std::stod(t_tree.GetAttributeOrDefault("pdrModifier", "1.0"));
    }

    void SlotRadioMedium::Reset() {
        actuators.clear();
        sensors.clear();
        rng->Reset();
    }

    void SlotRadioMedium::Update() {
        struct InFlightFrame {
            const SlotRadioFrame* frame;
            SlotRadioActuator* actuator;
            SlotRadioSensor* sensor;
        };
        // This map is indexed by SlotRadioSensor* to ensure rxFrames is ordered equivalently to the sensor set.
        // An InFlightFrame element in this map with a nullptr frame value indicates that a collision has occurred
        std::map<SlotRadioSensor*, InFlightFrame> rxFrames;
        std::set<ssize_t> channelBusy;

        for (auto actuator : actuators) {
            // Check if the actuator transmits a frame
            actuator->sentFrame = (actuator->txPort != nullptr) && (!actuator->requireCCA || !channelBusy.contains(actuator->robot->GetIndex()));
            // Unset gotAck for all actuators in this loop, subsequent loop will set it
            actuator->gotAck = false;

            if (actuator->sentFrame) {
                const auto& txPosition = actuator->GetPosition();

                for (auto sensor : sensors) {
                    const auto& rxPosition = sensor->GetPosition();
                    auto distance = (txPosition - rxPosition).Length();

                    if (rng->Uniform(CRange<Real>(0.0, 1.0)) < PDR(distance)) {
                        auto [element, inserted] = rxFrames.emplace(sensor, InFlightFrame {actuator->txPort, actuator, sensor});
                        // Sensor has already received a frame => collision
                        if (!inserted) {
                            element->second.frame = nullptr;
                        }

                        // If a sensor receives a frame, then mark channel busy for the corresponding robot
                        channelBusy.emplace(sensor->robot->GetIndex());
                    }
                }

                actuator->txPort = nullptr;
            }
        }

        // Deliver frames to the receiving sensors
        // rxFrames is a map indexed by sensor-pointer, while sensors is a set of sensor-pointers, so correct ordering
        // is guaranteed by underlying datatype
        auto rxFramesIterator = rxFrames.begin();
        auto sensorIterator = sensors.begin();
        while (sensorIterator != sensors.end()) {
            const auto sensor = *sensorIterator;
            const auto& [frameSensor,frameStruct] = *rxFramesIterator;
            if (sensor == frameSensor) {
                if (frameStruct.frame != nullptr) {
                    sensor->rxPort = frameStruct.frame;

                    if (sensor->sendAcks && frameStruct.frame->frameTo == sensor->robot->GetId()) {
                        auto distance = (sensor->GetPosition() - frameStruct.actuator->GetPosition()).Length();
                        if (rng->Uniform(CRange<Real>(0.0, 1.0)) < PDR(distance)) {
                            frameStruct.actuator->gotAck = true;
                        }
                    }
                }
                else {
                    sensor->rxPort = nullptr;
                }
                rxFramesIterator++;
                sensorIterator++;
            }
            else {
                sensor->rxPort = nullptr;
                sensorIterator++;
            }
        }

        // Shouldn't happen
        if (rxFramesIterator != rxFrames.end()) {
            THROW_ARGOSEXCEPTION("Unhandled rxFrame?");
        }
    }

    REGISTER_MEDIUM(SlotRadioMedium,
                    "slot_radio",
                    "Sven Signer",
                    "1.0",
                    "Breif Description",
                    "Long Description",
                    "status")
}