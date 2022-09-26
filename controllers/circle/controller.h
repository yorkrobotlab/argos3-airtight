#ifndef CIRCLE_CONTROLLER
#define CIRCLE_CONTROLLER

#include <argos3/core/control_interface/ci_controller.h>
#include <argos3/core/utility/math/rng.h>
#include <argos3/plugins/robots/pi-puck/control_interface/ci_pipuck_differential_drive_actuator.h>
#include <vector>
#include <queue>

#include "../../simulator/BigLEDActuator.h"
#include "../../simulator/slot-radio/control_interface/ci_slot_radio_sensor.h"
#include "../../simulator/slot-radio/control_interface/ci_slot_radio_actuator.h"

namespace argos {

   class CTestController : public CCI_Controller {

   public:

      CTestController() {}

      virtual ~CTestController() {}

      virtual void Init(TConfigurationNode& t_tree);

      virtual void ControlStep() override;

   private:
      CI_SlotRadioActuator* radioActuator;
      CI_SlotRadioSensor* radioSensor;
      BigLEDActuator* ledActuator;
      CCI_PiPuckDifferentialDriveActuator* driveActuator;

      CRandom::CRNG* rng;

      UInt32 lastLocalLedSwitch = -10000;
      UInt32 lastMove = -10000;

      //std::map<std::tuple<UInt32, UInt32, UInt32>, double> scheduledMoves;
      std::map<std::pair<UInt32, UInt32>, CColor> scheduledLEDs;
      std::map<std::pair<UInt32, UInt32>, UInt32> scheduledSpeed;

      std::pair<UInt32, UInt32> lastLEDChange{0,0};
      std::pair<UInt32, UInt32> lastSpeedChange{0,0};
      UInt32 nodeID;
      UInt32 nodeSpeedBias;
   };
}

#endif