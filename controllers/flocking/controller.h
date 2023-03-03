#ifndef FLOCKING_CONTROLLER
#define FLOCKING_CONTROLLER

#include <argos3/core/control_interface/ci_controller.h>
#include <argos3/core/utility/math/quaternion.h>
#include <argos3/core/utility/math/rng.h>
#include <argos3/core/utility/math/vector3.h>
#include <argos3/plugins/robots/generic/control_interface/ci_positioning_sensor.h>
#include <argos3/plugins/robots/pi-puck/control_interface/ci_pipuck_differential_drive_actuator.h>
#include <argos3/plugins/robots/pi-puck/control_interface/ci_pipuck_rangefinders_sensor.h>

#include <vector>
#include <queue>
#include <set>

#include "../../simulator/BigLEDActuator.h"
#include "../../simulator/slot-radio/control_interface/ci_slot_radio_sensor.h"
#include "../../simulator/slot-radio/control_interface/ci_slot_radio_actuator.h"

namespace argos {

   class CFlockingController : public CCI_Controller {

   public:
       CFlockingController() = default;

       virtual ~CFlockingController() = default;

       virtual void Init(TConfigurationNode& t_tree);

       virtual void ControlStep() override;

   private:
       friend class FlockingLoopFunctions;

       UInt32 controlPeriod;
       CI_SlotRadioActuator* radioActuator;
       CI_SlotRadioSensor* radioSensor;
       CCI_PiPuckDifferentialDriveActuator* driveActuator;
       CCI_PiPuckRangefindersSensor* rangeFinder;
       CCI_PositioningSensor* positionSensor;

       CRandom::CRNG* rng;
       UInt32 nodeID;

       std::map<UInt32, std::pair<CVector3, CVector3>> nodePositions;
       CVector3 goalPosition;

       std::vector<std::string> positionFlows;

       bool dataSinkNode = false;
       std::set<std::pair<UInt32, UInt32>> dataSink;
   };
}

#endif