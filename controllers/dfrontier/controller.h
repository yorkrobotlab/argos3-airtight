#ifndef DFRONTIER_CONTROLLER
#define DFRONTIER_CONTROLLER

#include <map>
#include <vector>

#include <argos3/core/control_interface/ci_controller.h>
#include <argos3/core/utility/math/quaternion.h>
#include <argos3/core/utility/math/rng.h>
#include <argos3/core/utility/math/vector3.h>
#include <argos3/plugins/robots/generic/control_interface/ci_positioning_sensor.h>
#include <argos3/plugins/robots/pi-puck/control_interface/ci_pipuck_differential_drive_actuator.h>
#include <argos3/plugins/robots/pi-puck/control_interface/ci_pipuck_rangefinders_sensor.h>

#include "simulator/slot-radio/control_interface/ci_slot_radio_sensor.h"
#include "simulator/slot-radio/control_interface/ci_slot_radio_actuator.h"
#include "coverage_matrix.h"

namespace argos {

   class CDFrontierController : public CCI_Controller {

   public:
       CDFrontierController() = default;

       ~CDFrontierController() override;

       void Init(TConfigurationNode& t_tree) override;

       void Reset() override;

       void ControlStep() override;

   private:
       friend class DFrontierOpenGLFunctions;
       friend class DFrontierLoopFunctions;

       struct PositionMessage {
           std::string fromNode;
           UInt32 expiry;
           CoverageMatrix::Cell currentCell, nextCell;
       };
       struct CoverageMessage {
           CoverageMatrix::Cell cell;
           CoverageMatrix::CellStatus status;
       };

       struct NodeDataEntry {
           CoverageMatrix::Cell currentCell, nextCell;
       };
       std::map<std::string, NodeDataEntry> nodeData;

       CRandom::CRNG* rng = nullptr;

       CI_SlotRadioActuator* radioActuator = nullptr;
       CI_SlotRadioSensor* radioSensor = nullptr;
       CCI_PiPuckDifferentialDriveActuator* driveActuator = nullptr;
       CCI_PiPuckRangefindersSensor* rangeFinder = nullptr;
       CCI_PositioningSensor* positionSensor = nullptr;

       // Sensor Readings
       CVector2 position;
       CRadians orientation;
       std::array<Real, 8> proximityReadings;

       void UpdateSensorReadings();

       CoverageMatrix::Cell prevCell;
       CoverageMatrix::Cell goalCell;
       std::set<CoverageMatrix::Cell> badCells;

       CoverageMatrix* cMatrix = nullptr;

       void SteerTowardsGoal();
       void DetermineNewGoalCell();

       UInt32 stuck = 0;
       UInt32 goalStuck = 0;
       UInt32 lastHC = 1 << 31;

       enum class CohesionMode {
           NoCohesion=0, ConstantCohesion=1, MixedCriticality=2, HalfCohesion=3
       };
       CohesionMode cohesionMode;
   };
}

#endif