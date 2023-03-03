// Flocking Controller

#include "controller.h"

#include <argos3/core/utility/logging/argos_log.h>
#include <argos3/core/simulator/simulator.h>
#include <optional>
#include <functional>

#include "simulator/slot-radio/airtight/AirTightActuator.h"

namespace argos {

    struct FlockingMessage {
        UInt32 clock;
        UInt32 nodeID;
        CVector3 position;
        CVector3 velocity;
    };
    struct DataMessage {
        UInt32 nodeID;
        UInt32 clock;
    };

   /****************************************/
   /****************************************/

   void CFlockingController::Init(TConfigurationNode& t_tree) {
       radioActuator = GetActuator<CI_SlotRadioActuator>("slot_radio");
       radioSensor = GetSensor<CI_SlotRadioSensor>("slot_radio");

       radioActuator->SetRobotConfiguration(t_tree);
       radioSensor->SetRobotConfiguration(t_tree);

       driveActuator = GetActuator<CCI_PiPuckDifferentialDriveActuator>("pipuck_differential_drive");
       driveActuator->SetLinearVelocity(0.0, 0.0);

       rangeFinder = GetSensor<CCI_PiPuckRangefindersSensor>("pipuck_rangefinders");

       positionSensor = GetSensor<CCI_PositioningSensor>("positioning");

       // ARGoS node IDs are strings, but a numeric ID is more convenient
       nodeID = std::hash<std::string>{}(GetId());

       rng = CRandom::CreateRNG("argos");

       if (CPhysicsEngine::GetSimulationClockTick() != 0.01) {
           THROW_ARGOSEXCEPTION("Expected 10ms time slots");
       }

       // position = CVector3();
       // goalPosition = CVector3(position);

       controlPeriod = stoi(t_tree.GetAttribute("controlPeriod"));

       if (dynamic_cast<AirTightActuator*>(radioActuator) == nullptr) {
           RLOG << "Not airtight\n";
           positionFlows.emplace_back("pos");
       }
       else {
           auto &airTightConfig = GetNode(t_tree, "airtight");
           TConfigurationNodeIterator it;
           for (it = it.begin(&airTightConfig); it != it.end(); ++it) {
               if (it->Value() != "buffer") {
                   THROW_ARGOSEXCEPTION("Unknown configuration element: " + it->Value());
               }
               const auto &name = it->GetAttribute("name");
               if (name.length() >= 3) {
                   if (name.substr(0, 3) == "pos") {
                       RLOG << "Position flow " << name << "\n";
                       positionFlows.emplace_back(name);
                   }
               }
           }
       }

       if (GetId() == "node0") {
           dataSinkNode = true;
       }
   }

   /****************************************/
   /****************************************/

   void CFlockingController::ControlStep() {
       // Get current clock
       UInt32 clock = CSimulator::GetInstance().GetSpace().GetSimulationClock();

       Real proxReadings[8] = {0, 0, 0, 0, 0, 0, 0};
       std::function<void(const CCI_PiPuckRangefindersSensor::SInterface&)> visitFn =
               [&proxReadings](const CCI_PiPuckRangefindersSensor::SInterface& sensor) {
           proxReadings[sensor.Label] = sensor.Proximity;
       };
       rangeFinder->Visit(visitFn);

       // Get position
       auto positionSensorReading = positionSensor->GetReading();
       const auto position = positionSensorReading.Position;
       const auto orientation = positionSensorReading.Orientation;


       // Receive Positions
       if (radioSensor->HasFrame()) {
           const auto& rawMsg = radioSensor->GetMessage();

           if (rawMsg.type() == typeid(FlockingMessage)) {
               const auto& msg = any_cast<const FlockingMessage>(rawMsg);
               if (msg.clock >= clock) {
                   nodePositions[msg.nodeID] = std::make_pair(msg.position, msg.velocity);
               }
               else {
                   RLOG << "Discarding late message\n";
               }
           }
           else if (rawMsg.type() == typeid(DataMessage)) {
               const auto& msg = any_cast<const DataMessage>(rawMsg);
               if (dataSinkNode) {
                   dataSink.emplace(msg.nodeID, msg.clock);
               }
           }
           else {
               THROW_ARGOSEXCEPTION("Unexpected message");
           }
       }

       if (!dataSinkNode && (clock % 33) == 1) {
           // Pretend we've got some important data packet
           radioActuator->QueueFrame("data", DataMessage {nodeID, clock});
       }

       if (clock % controlPeriod == 1) {
           if ((goalPosition - position).Length() > 0.01) {
               RLOG << "Goal difference: " << (goalPosition - position).Length() << "\n";
           }

           auto goal = CVector3();

           // Update Movement
           if (!nodePositions.empty()) {
               auto centroid = CVector3();
               auto velocity = CVector3();
               auto collision = CVector3();
               for (auto& nodePositionPair: nodePositions) {
                   // Also add node velocity, since we want the centroid position at the end of the control step?
                   centroid += (nodePositionPair.second.first + nodePositionPair.second.second);

                   if (nodePositionPair.first != nodeID) {
                       auto repelVector = (nodePositionPair.second.first - position);
                       auto repelForce = 0.1 / pow(repelVector.Length()-0.0724, 2);
                       collision -= repelVector * repelForce;
                   }

                   velocity += nodePositionPair.second.second;

               }
               centroid /= nodePositions.size();
               velocity /= nodePositions.size();

               auto cohesion = (centroid - position);
               auto cohesion_factor = std::min(cohesion.Length(), 1.0);
               cohesion = cohesion.Normalize() * cohesion_factor;
               goal = cohesion + collision + velocity;

               for(int i = 0; i < 8; i++) {
                   if (proxReadings[i] <= 0.085) {
                       RLOG << position << " PROXCOL\n";
                       CVector3 rayVector;
                       CRadians angle;
                       switch(i) {
                           case 0:
                               rayVector = CVector3(CVector3::Z);
                               rayVector = rayVector.Rotate(CQuaternion(0.5 * CRadians::PI, CVector3(0.218, 0.976,0)));
                               rayVector = rayVector.Rotate(orientation);

                               angle = rayVector.GetAngleWith(goal);

                               if (angle < CRadians::PI_OVER_TWO) {
                                   goal -= (rayVector * Cos(angle) * goal.Length());
                               }
                               break;
                           case 1:
                               rayVector = CVector3(CVector3::Z).Rotate(CQuaternion( 0.5 * CRadians::PI, CVector3(0.730, 0.683,0)));
                               rayVector = rayVector.Rotate(orientation);

                               angle = rayVector.GetAngleWith(goal);

                               if (angle < CRadians::PI_OVER_TWO) {
                                   goal -= (rayVector * Cos(angle) * goal.Length());
                               }
                               break;
                           case 2:
                               rayVector = CVector3(CVector3::Z).Rotate(CQuaternion( 0.5 * CRadians::PI, CVector3(1.0,0,0)));
                               rayVector = rayVector.Rotate(orientation);

                               angle = rayVector.GetAngleWith(goal);

                               if (angle < CRadians::PI_OVER_TWO) {
                                   goal -= (rayVector * Cos(angle) * goal.Length());
                               }
                               break;
                           case 3:
                               rayVector = CVector3(CVector3::Z).Rotate( CQuaternion( 0.5 * CRadians::PI, CVector3(0.630,-0.776,0)));
                               rayVector = rayVector.Rotate(orientation);

                               angle = rayVector.GetAngleWith(goal);

                               if (angle < CRadians::PI_OVER_TWO) {
                                   goal -= (rayVector * Cos(angle) * goal.Length());
                               }
                               break;
                           case 4:
                               rayVector = CVector3(CVector3::Z).Rotate(CQuaternion(-0.5 * CRadians::PI, CVector3(0.630, 0.776,0)));
                               rayVector = rayVector.Rotate(orientation);

                               angle = rayVector.GetAngleWith(goal);

                               if (angle < CRadians::PI_OVER_TWO) {
                                   goal -= (rayVector * Cos(angle) * goal.Length());
                               }
                               break;
                           case 5:
                               rayVector = CVector3(CVector3::Z).Rotate(CQuaternion(-0.5 * CRadians::PI, CVector3(1.0,0,0)));
                               rayVector = rayVector.Rotate(orientation);

                               angle = rayVector.GetAngleWith(goal);

                               if (angle < CRadians::PI_OVER_TWO) {
                                   goal -= (rayVector * Cos(angle) * goal.Length());
                               }
                               break;
                           case 6:
                               rayVector = CVector3(CVector3::Z).Rotate(CQuaternion(-0.5 * CRadians::PI, CVector3(0.730,-0.683,0)));
                               rayVector = rayVector.Rotate(orientation);

                               angle = rayVector.GetAngleWith(goal);

                               if (angle < CRadians::PI_OVER_TWO) {
                                   goal -= (rayVector * Cos(angle) * goal.Length());
                               }
                               break;
                           case 7:
                               rayVector = CVector3(CVector3::Z).Rotate(CQuaternion(-0.5 * CRadians::PI, CVector3(0.218,-0.976,0)));
                               rayVector = rayVector.Rotate(orientation);

                               angle = rayVector.GetAngleWith(goal);

                               if (angle < CRadians::PI_OVER_TWO) {
                                   goal -= (rayVector * Cos(angle) * goal.Length());
                               }
                               break;
                       }
                       RLOG << "Changed goal to " << goal << "\n";
                   }
               }

               if (goal.Length() >= (0.075 * controlPeriod / 100)) {
                   goal = goal.Normalize() * (0.075 * controlPeriod / 100);
               }

               goalPosition = position + goal;
           }
           nodePositions.clear();

           // Transmit
           for(auto& buffer : positionFlows) {
               radioActuator->QueueFrame(buffer, FlockingMessage {clock+controlPeriod, nodeID, goalPosition, goal});
           }
           nodePositions[nodeID] = std::make_pair(goalPosition, goal);
       }

       // Handle Movement
       if (clock >= 0) {
           auto forwardsVector = CVector3(1, 0, 0).Rotate(orientation);
           auto normalisedGoal = (goalPosition - position).Normalize();
           auto rotationGoal = normalisedGoal.GetAngleWith(forwardsVector);

           if (std::isnan(rotationGoal.GetValue())) {
               RLOG << "ROTGOAL IS NAN\n";
               rotationGoal.SetValue(0.0);
           }

           auto diff = (normalisedGoal.RotateZ(rotationGoal) - forwardsVector).Length();
           rotationGoal *= (diff > 0.01) ? -1 : 1;
           rotationGoal.SignedNormalize();

           double lWheel = 0.0;
           double rWheel = 0.0;

           // 0.565: PiPuck interwheel distance
           auto wheelDiff = (rotationGoal.GetValue() / 2) * 0.0565 * 100;
           lWheel += wheelDiff;
           rWheel -= wheelDiff;

           if ((rotationGoal.GetAbsoluteValue() < CRadians::PI_OVER_TWO.GetValue()) &&
                    (proxReadings[0] >= 0.085) && (proxReadings[7] >= 0.085)) {
               lWheel += 100 * (goalPosition - position).Length() / (controlPeriod - (clock % controlPeriod));
               rWheel += 100 * (goalPosition - position).Length() / (controlPeriod - (clock % controlPeriod));
           }

           if (abs(lWheel) > 0.1) {
               lWheel = (lWheel / abs(lWheel)) * 0.1;
           }
           if (abs(rWheel) > 0.1) {
               rWheel = (rWheel / abs(rWheel)) * 0.1;
           }

           //RLOG << "Final speeds: " << lWheel << " " << rWheel << "\n";

           driveActuator->SetLinearVelocity(lWheel, rWheel);
       }
   }

   /****************************************/
   /****************************************/

   REGISTER_CONTROLLER(CFlockingController, "flocking_controller");

}



