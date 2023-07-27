#include "slot_radio_medium.h"
#include "slot_radio_actuator.h"
#include "slot_radio_sensor.h"

namespace argos{

    void SlotRadioMedium::RegisterActuator(SlotRadioActuator *actuator) {
        auto [it, inserted] = actuators.emplace(actuator->robot->GetId(), actuator);
        if (!inserted) {
            THROW_ARGOSEXCEPTION("Nodes must not have multiple slot_radio actuators on the same medium");
        }
    }

    void SlotRadioMedium::RegisterSensor(SlotRadioSensor *sensor) {
        auto [it, inserted] = sensors.emplace(sensor->robot->GetId(), sensor);
        if (!inserted) {
            THROW_ARGOSEXCEPTION("Nodes must not have multiple slot_radio sensors on the same medium")
        }
    }

    void SlotRadioMedium::Init(TConfigurationNode& t_tree) {
        CMedium::Init(t_tree);
        rng = CRandom::CreateRNG("argos");
        pdrModifier = std::stod(t_tree.GetAttributeOrDefault("pdrModifier", "0.99"));
        dropoffStart = std::stod(t_tree.GetAttributeOrDefault("dropoffStart", "1.5"));
        dropoffFactor = std::stod(t_tree.GetAttributeOrDefault("dropoffFactor", "3.0"));
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
        // This map is indexed by robot id string to ensure rxFrames is ordered equivalently to the sensor set.
        // An InFlightFrame element in this map with a nullptr frame value indicates that a collision has occurred
        std::map<std::string, InFlightFrame> rxFrames;
        std::set<std::string> channelBusy;

        // For each transmission, compute which sensors receive them and store results into rxFrames
        for (auto [actuatorRobotId, actuator] : actuators) {
            // Check if the actuator transmits a frame
            actuator->sentFrame = (actuator->txPort != nullptr) && (!actuator->requireCCA || !channelBusy.contains(actuatorRobotId));
            // Unset gotAck for all actuators in this loop, subsequent loop will set it
            actuator->gotAck = false;

            if (actuator->sentFrame) {
                const auto& txPosition = actuator->GetPosition();

                for (auto [sensorRobotId, sensor] : sensors) {
                    const auto& rxPosition = sensor->GetPosition();
                    auto distance = (txPosition - rxPosition).Length();

                    if (rng->Uniform(CRange<Real>(0.0, 1.0)) < PDR(distance)) {
                        auto [element, inserted] = rxFrames.emplace(sensorRobotId, InFlightFrame {actuator->txPort, actuator, sensor});
                        // Sensor has already received a frame => collision
                        if (!inserted) {
                            element->second.frame = nullptr;
                        }

                        // If a sensor receives a frame, then mark channel busy for the corresponding robot
                        channelBusy.emplace(sensorRobotId);
                    }
                }

                actuator->txPort = nullptr;
            }
        }

        // Deliver frames to the receiving sensors
        // Both sensors and rxFrames are maps indexed by sensor robot id, so correct (i.e. equivalent) ordering is
        // guaranteed by the underlying datatype
        auto rxFramesIterator = rxFrames.begin();
        auto sensorIterator = sensors.begin();
        while (sensorIterator != sensors.end()) {
            const auto sensor = sensorIterator->second;
            const auto& frameStruct = rxFramesIterator->second;
            if (sensor == frameStruct.sensor) {
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
                    "Medium for communication between slot_radio sensors/actuators",
                    "Simulate low-bandwidth wireless protocols, where each simulation set allows for each actuator\n"
                    "to send at most one frame, and each sensor to receive at most one frame. If multiple frames are\n"
                    "frames within a single step they are assumed to interfere destructively",
                    "unstable")
}