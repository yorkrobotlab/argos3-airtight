// Circle Controller

#include "controller.h"

#include <argos3/core/utility/logging/argos_log.h>
#include <argos3/core/simulator/simulator.h>
#include <argos3/core/simulator/space/space.h>
#include <optional>
#include <functional>

#include "../../loop_functions/circle_loop_functions.h"

namespace argos {

   /****************************************/
   /****************************************/

   void CTestController::Init(TConfigurationNode& t_tree) {
       radioActuator = GetActuator<CI_SlotRadioActuator>("slot_radio");
       radioSensor = GetSensor<CI_SlotRadioSensor>("slot_radio");

       radioActuator->SetRobotConfiguration(t_tree);
       radioSensor->SetRobotConfiguration(t_tree);

       driveActuator = GetActuator<CCI_PiPuckDifferentialDriveActuator>("pipuck_differential_drive");
       driveActuator->SetLinearVelocity(0.0, 0.0);

       ledActuator = GetActuator<BigLEDActuator>("big_leds");
       ledActuator->SetLEDOn(true);

       // ARGoS node IDs are strings, but a numeric ID is more convenient
       nodeID = std::hash<std::string>{}(GetId());

       rng = CRandom::CreateRNG("argos");
       // Each node picks a move size multiplier so that if communication fails, their positions will diverge over time
       nodeSpeedBias = rng->Uniform(CRange<UInt32>(2, 10));

       if (CPhysicsEngine::GetSimulationClockTick() != 0.01) {
           THROW_ARGOSEXCEPTION("Expected 10ms time slots");
       }
   }

   /****************************************/
   /****************************************/

   void CTestController::ControlStep() {
       // Message Struct Definitions
       struct LEDMessage {
           UInt32 clock, originNode;
           CColor newColour;
           UInt32 switchTime;
       };

       struct MOVMessage {
            UInt32 clock, originNode, start, speed;
       };

       // Get current clock
       UInt32 clock = CSimulator::GetInstance().GetSpace().GetSimulationClock();

       // Receive messages
       if (radioSensor->HasFrame()) {
           const auto& rawMsg = radioSensor->GetMessage();

           // LED Frame
           if (rawMsg.type() == typeid(LEDMessage)) {
               const auto& msg = any_cast<const LEDMessage>(rawMsg);
               scheduledLEDs.emplace(std::make_pair(msg.switchTime, msg.originNode), msg.newColour);
           }
           else if (rawMsg.type() == typeid(MOVMessage)) {
               const auto& msg = any_cast<const MOVMessage>(rawMsg);
               scheduledSpeed.emplace(std::make_pair(msg.start, msg.originNode), msg.speed);
           }
           // We shouldn't end up here
           else {
               THROW_ARGOSEXCEPTION("Received unhandled frame type ");
           }
       }

       if ((clock - lastLocalLedSwitch >= 100) && (rng->Uniform(CRange<Real>(0.0, 1.0) ) < 0.02)){
           // Pick RGB Value
           /*auto r = rng->Uniform(CRange<UInt32>(1, 6))*51;
           auto g = rng->Uniform(CRange<UInt32>(1, 6))*51;
           auto b = rng->Uniform(CRange<UInt32>(1, 6))*51;*/
           UInt8 r = rng->Uniform(CRange<UInt32>(0, 256));
           UInt8 g = rng->Uniform(CRange<UInt32>(0, 256));
           UInt8 b = rng->Uniform(CRange<UInt32>(0, 256));
           auto newColour = CColor(r, g, b);

           // Pick future time for change to take effect
           UInt32 switchTime = clock + 200; //rng->Uniform(CRange<UInt32>(clock+200, clock+450));

           // Store change locally
           scheduledLEDs.emplace(std::make_pair(switchTime, nodeID), newColour);

           // Send change message
           radioActuator->QueueFrame("led", LEDMessage {clock, nodeID, newColour, switchTime});

           // Update last switch time to enforce inter-arrival period
           lastLocalLedSwitch = clock;

           // Register correct colour in loop function for data logging
           dynamic_cast<CircleLoopFunctions*>(&CSimulator::GetInstance().GetLoopFunctions())->SetCorrectColour(switchTime, nodeID, newColour);
       }

       if (clock - lastMove >= 500 && rng->Uniform(CRange<Real>(0.0, 1.0)) < 0.01) {
           // Select move parameters
           auto start = clock + 1000;
           auto speed = rng->Uniform(CRange<UInt32>(0, 3)) * nodeSpeedBias;

           scheduledSpeed.emplace(std::make_pair(start, nodeID), speed);
           radioActuator->QueueFrame("mov", MOVMessage {clock, nodeID, start, speed});
           lastMove = clock;
       }

       // Check if an LED change is scheduled for the current clock time
       for(auto it = scheduledLEDs.begin(), last = scheduledLEDs.end(); it != last;) {
           auto switch_time = it->first.first;
           if (clock == switch_time) {
               ledActuator->SetColour(it->second);
               lastLEDChange = it->first;
               it = scheduledLEDs.erase(it);
           }
           else if (clock > switch_time) {
               if (it->first > lastLEDChange) {
                   RLOG << "Late LED change\n";
                   ledActuator->SetColour(it->second);
                   lastLEDChange = it->first;
                   it = scheduledLEDs.erase(it);
               }
               else {
                   RLOG << "Missed led switch\n";
                   it = scheduledLEDs.erase(it);
               }
           }
           else {
               ++it;
           }
       }

       for(auto it = scheduledSpeed.begin(), last = scheduledSpeed.end(); it != last;) {
           auto switch_time = it->first.first;
           if (clock == switch_time) {
               Real speed = ((Real)it->second) / 200.0;
               driveActuator->SetLinearVelocity(speed, speed);
               lastSpeedChange = it->first;
               it = scheduledSpeed.erase(it);
           }
           else if (clock > switch_time) {
               // If we've missed a speed change, we should accept it late if it is newer than the current speed or for
               // an equal time but from a high priority node
               if (it->first > lastSpeedChange) {
                   RLOG << "Late speed change\n";
                   Real speed = ((Real)it->second) / 200.0;
                   driveActuator->SetLinearVelocity(speed, speed);
                   lastSpeedChange = it->first;
                   it = scheduledSpeed.erase(it);
               }
               else {
                   RLOG << "Missed speed change\n";
                   it = scheduledSpeed.erase(it);
               }
           }
           else {
               ++it;
           }
       }
   }

   /****************************************/
   /****************************************/

   REGISTER_CONTROLLER(CTestController, "circle_controller");

}



