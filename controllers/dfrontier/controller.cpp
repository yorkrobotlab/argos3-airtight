// Discretised-Space Frontier Controller

#include "controller.h"

#include <algorithm>
#include <limits>

#include <argos3/core/utility/logging/argos_log.h>
#include <argos3/core/simulator/simulator.h>
#include <argos3/core/simulator/space/space.h>

#include "simulator/slot-radio/airtight/AirTightActuator.h"
#include "simulator/slot-radio/protocol1/Protocol1Actuator.h"

namespace argos {

    // argos3/src/plugins/robots/pi-puck/control_interface/ci_pipuck_rangefinders_sensor.cpp defines Pi-Puck
    // rangefinder rays, but somewhat annoyingly they aren't accessible here, so just redefine them
    std::array<CVector2, 8> computeRangeFinderRays() {
        auto rays = std::array<CVector2, 8>();
        CVector3(CVector3::Z).Rotate(CQuaternion(0.5 * CRadians::PI, CVector3(0.218, 0.976, 0))).ProjectOntoXY(rays[0]);
        CVector3(CVector3::Z).Rotate(CQuaternion(0.5 * CRadians::PI, CVector3(0.730, 0.683, 0))).ProjectOntoXY(rays[1]);
        CVector3(CVector3::Z).Rotate(CQuaternion(0.5 * CRadians::PI, CVector3(1.0, 0, 0))).ProjectOntoXY(rays[2]);
        CVector3(CVector3::Z).Rotate(CQuaternion(0.5 * CRadians::PI, CVector3(0.630, -0.776, 0))).ProjectOntoXY(rays[3]);
        CVector3(CVector3::Z).Rotate(CQuaternion(-0.5 * CRadians::PI, CVector3(0.630, 0.776, 0))).ProjectOntoXY(rays[4]);
        CVector3(CVector3::Z).Rotate(CQuaternion(-0.5 * CRadians::PI, CVector3(1.0, 0, 0))).ProjectOntoXY(rays[5]);
        CVector3(CVector3::Z).Rotate(CQuaternion(-0.5 * CRadians::PI, CVector3(0.730, -0.683, 0))).ProjectOntoXY(rays[6]);
        CVector3(CVector3::Z).Rotate(CQuaternion(-0.5 * CRadians::PI, CVector3(0.218, -0.976, 0))).ProjectOntoXY(rays[7]);
        return rays;
    }
    const auto rangefinderRays = computeRangeFinderRays();

    void CDFrontierController::Init(TConfigurationNode &t_tree) {
        radioActuator = GetActuator<CI_SlotRadioActuator>("slot_radio");
        radioSensor = GetSensor<CI_SlotRadioSensor>("slot_radio");

        radioActuator->SetRobotConfiguration(t_tree);
        radioSensor->SetRobotConfiguration(t_tree);

        driveActuator = GetActuator<CCI_PiPuckDifferentialDriveActuator>("pipuck_differential_drive");
        driveActuator->SetLinearVelocity(0.0, 0.0);

        rangeFinder = GetSensor<CCI_PiPuckRangefindersSensor>("pipuck_rangefinders");
        positionSensor = GetSensor<CCI_PositioningSensor>("positioning");

        rng = CRandom::CreateRNG("argos");

        // TODO: Verify if further fixes are required before removing this check
        if (CPhysicsEngine::GetSimulationClockTick() != 0.01) {
            THROW_ARGOSEXCEPTION("Expected 10ms time slots");
        }

        cohesionMode = static_cast<CohesionMode>(stoi(t_tree.GetAttribute("cohesionMode")));

        const auto &arenaLimits = CSimulator::GetInstance().GetSpace().GetArenaLimits();
        cMatrix = new CoverageMatrix(arenaLimits, 10.0);

        UpdateSensorReadings();
        goalCell = cMatrix->PositionToCell(position);
        // TODO: Need some handling for if we're stuck in initial position?
        prevCell = goalCell;

    }

    CDFrontierController::~CDFrontierController() {
        delete cMatrix;
    }

    void CDFrontierController::Reset() {
        CCI_Controller::Reset();
        rng->Reset();
        delete cMatrix;

        const auto &arenaLimits = CSimulator::GetInstance().GetSpace().GetArenaLimits();
        cMatrix = new CoverageMatrix(arenaLimits, 10.0);

        stuck = 0;
    }

    void CDFrontierController::UpdateSensorReadings() {
        // Update Position & Orientation
        const auto &positionSensorReading = positionSensor->GetReading();
        positionSensorReading.Position.ProjectOntoXY(position);
        // TODO: Is there a nicer way of computing this?
        CVector2 tmp;
        CVector3(CVector3::X).Rotate(positionSensorReading.Orientation).ProjectOntoXY(tmp);
        orientation = tmp.Angle();

        // Update Rangefinders
        auto visitFn = [&reading = this->proximityReadings](const CCI_PiPuckRangefindersSensor::SInterface &sensor) {
            reading[sensor.Label] = sensor.Proximity;
        };
        rangeFinder->Visit(visitFn);
    }

    /****************************************/
    /****************************************/

    void CDFrontierController::SteerTowardsGoal() {
        auto movementGoal = cMatrix->CellCentre(goalCell) - position;
        auto rotationGoal = (orientation - movementGoal.Angle()).SignedNormalize();

        // We sometimes get rotationGoal == NaN when the angle is very small (?)
        // FIXME: This should no longer be the case, verify & remove this
        if (std::isnan(rotationGoal.GetValue())) {
            RLOG << "BAD BAD BAD NAN-ANGLE\n";
            rotationGoal.SetValue(0.0);
        }

        // Set forwards velocity
        Real linearVelocity = 0.0;
        if (rotationGoal.GetAbsoluteValue() < CRadians::PI_OVER_TWO.GetValue()) {
            auto forwardsComponent = Cos(rotationGoal) * movementGoal.Length();
            linearVelocity = forwardsComponent / CPhysicsEngine::GetSimulationClockTick();
        }

        // Compute angular velocity
        auto angularVelocity = rotationGoal.GetValue() * 0.5 * 0.0565 / CPhysicsEngine::GetSimulationClockTick();

        const double maxWheelSpeed = 0.1;
        double lWheel, rWheel;
        // Limit maximum speed, preserving angular component over linear component
        if (abs(angularVelocity) < maxWheelSpeed) {
            auto linear = std::min(linearVelocity, maxWheelSpeed - abs(angularVelocity));
            lWheel = linear + angularVelocity;
            rWheel = linear - angularVelocity;
        }
        else {
            lWheel = +copysign(maxWheelSpeed, angularVelocity);
            rWheel = -copysign(maxWheelSpeed, angularVelocity);
        }

        if (abs(lWheel) > 0.1 || abs(rWheel) > 0.1) {
            THROW_ARGOSEXCEPTION("BAD BAD WHEEL SPEED");
        }

        // Shouldn't happen
        if (std::isnan(lWheel) || std::isnan(rWheel)) {
            RLOG << "linearVel: " << linearVelocity << " angularVel:" << angularVelocity << " lWheel: " << lWheel
                 << " rWheel: " << rWheel << "\n";
            THROW_ARGOSEXCEPTION("Wheel speed is NaN");
        }

        driveActuator->SetLinearVelocity(lWheel, rWheel);
    }


    void CDFrontierController::ControlStep() {
        // Get current clock
        UInt32 clock = CSimulator::GetInstance().GetSpace().GetSimulationClock();
        UpdateSensorReadings();

        if (auto p1a = dynamic_cast<Protocol1Actuator *>(radioActuator); p1a != nullptr) {
            if (p1a->isInHighCritMode()) {
                lastHC = clock;
            }
        }

        bool goalInvalid = false;

        // Handle proximity sensor readings
        auto currentCell = cMatrix->PositionToCell(position);
        bool positionClear = true;
        for(int i=0; i<8; i++) {
            if (proximityReadings[i] < 0.09) {
                auto collisionPosition = position + (proximityReadings[i] * CVector2(rangefinderRays[i]).Rotate(orientation));
                auto collisionCell = cMatrix->PositionToCell(collisionPosition);

                if (cMatrix->GetValue(collisionCell) != CoverageMatrix::CellStatus::Occupied) {
                    cMatrix->SetValue(collisionCell, CoverageMatrix::CellStatus::Occupied);
                    radioActuator->QueueFrame("coverage", CoverageMessage {collisionCell, CoverageMatrix::CellStatus::Occupied});
                }

                if (collisionCell == currentCell) {
                    positionClear = false;
                }
                if (collisionCell == goalCell) {
                    stuck = std::max(stuck, 250u);
                }
            }
        }

        // Robot is very stuck
        if (stuck == 500) {
            RLOG << "BAD BAD VERY STUCK\n";
            RLOG << "GOAL CELL: " << goalCell.first << "," << goalCell.second << "    currentCell " << currentCell.first << "," << currentCell.second << "\n";
            RLOG << "Goal cell bad??: " << badCells.contains(goalCell) << "\n";
            badCells.emplace(goalCell);
            DetermineNewGoalCell();
            radioActuator->QueueFrame("position", PositionMessage{GetId(), clock + 100000, currentCell, goalCell});
            stuck = 251;
        }
        // Robot is stuck
        if (stuck == 250) {
            if (cMatrix->GetValue(goalCell) != CoverageMatrix::CellStatus::Occupied) {
                cMatrix->SetValue(goalCell, CoverageMatrix::CellStatus::Occupied);
                radioActuator->QueueFrame("coverage", CoverageMessage {goalCell, CoverageMatrix::CellStatus::Occupied});
            }

            if (goalCell != prevCell && !badCells.contains(prevCell)) {
                goalCell = prevCell;
                radioActuator->QueueFrame("position", PositionMessage{GetId(), clock + 100000, currentCell, goalCell});
            }
            else {
                RLOG << "OH NO IM STUCK\n";
                DetermineNewGoalCell();
                radioActuator->QueueFrame("position", PositionMessage{GetId(), clock + 100000, currentCell, goalCell});
            }
        }

        // Reached goal position
        if ((cMatrix->CellCentre(goalCell) - position).Length() < 0.01) {
            stuck = 0;

            // if (positionClear && cMatrix->GetValue(currentCell) <= CoverageMatrix::CellStatus::Frontier) {
            if (positionClear) {
                cMatrix->SetValue(currentCell, CoverageMatrix::CellStatus::Free);
                radioActuator->QueueFrame("coverage", CoverageMessage {currentCell, CoverageMatrix::CellStatus::Free});
            }

            DetermineNewGoalCell();
            prevCell = currentCell;

            if (goalCell != currentCell) {
                radioActuator->QueueFrame("position", PositionMessage{GetId(), clock + 100000, currentCell, goalCell});
            }
        }
        else {
            stuck++;
        }

        // Receive Positions
        if (radioSensor->HasFrame()) {
            const auto &rawMsg = radioSensor->GetMessage();

            if (rawMsg.type() == typeid(PositionMessage)) {
                const auto &message = any_cast<const PositionMessage>(rawMsg);
                if (message.expiry >= clock) {
                    nodeData[message.fromNode] = {message.currentCell, message.nextCell};
                } else {
                    RLOG << "Discarding late message\n";
                }
            }
            else if (rawMsg.type() == typeid(CoverageMessage)) {
                const auto &message = any_cast<const CoverageMessage>(rawMsg);
                auto localStatus = cMatrix->GetValue(message.cell);
                // TODO: When to accept remote information?
                if (localStatus <= CoverageMatrix::CellStatus::Frontier) {
                    cMatrix->SetValue(message.cell, message.status);
                }
            }
            else {
                THROW_ARGOSEXCEPTION("Controller got unexpected message type");
            }
        }

        SteerTowardsGoal();
    }

    void CDFrontierController::DetermineNewGoalCell() {
        UInt32 clock = CSimulator::GetInstance().GetSpace().GetSimulationClock();
        const auto& currentCell = goalCell; //cMatrix->PositionToCell(position);

        CoverageMatrix::Cell candidateCells[9] = {
                {currentCell.first-1, currentCell.second-1},
                {currentCell.first-1, currentCell.second},
                {currentCell.first-1, currentCell.second+1},
                {currentCell.first, currentCell.second-1},
                {currentCell.first, currentCell.second},
                {currentCell.first, currentCell.second+1},
                {currentCell.first+1, currentCell.second-1},
                {currentCell.first+1, currentCell.second},
                {currentCell.first+1, currentCell.second+1}
        };
        std::array<Real, 9> candidateScore = {-1,0,-1,0,0,0,-1,0,-1};

        auto cohesionCentroid = CVector2();
        for (const auto &node: nodeData) {
            cohesionCentroid += cMatrix->CellCentre(node.second.nextCell);
        }
        cohesionCentroid /= nodeData.size();

        for(int i=0; i<9; i++) {
            // Force minimum score for illegal cells and continue
            if (!cMatrix->CellValid(candidateCells[i])) {
                candidateScore[i] = std::numeric_limits<Real>::lowest();
                continue;
            }

            if (badCells.contains(candidateCells[i])) {
                candidateScore[i] = std::numeric_limits<Real>::lowest();
                continue;
            }

            // Don't get stuck in the same place forever
            if (candidateCells[i] == currentCell) {
                candidateScore[i] -= goalStuck / 10.0;
            }

            // Avoid returning to previous cell
            if (candidateCells[i] == prevCell) {
                candidateScore[i] -= 1;
            }

            // Consider Cell Status
            switch(cMatrix->GetValue(candidateCells[i])) {
                case CoverageMatrix::CellStatus::Unknown:
                case CoverageMatrix::CellStatus::Frontier:
                    candidateScore[i] += 10;
                    break;
                case CoverageMatrix::CellStatus::Free:
                    break;
                case CoverageMatrix::CellStatus::Occupied:
                    candidateScore[i] -= 1000;
                    break;
            }

            // Separation
            for(const auto& node : nodeData) {
                switch(CellDistance(candidateCells[i], node.second.nextCell)) {
                    case 0:
                        // RLOG << "BAD BAD BAD collision?\n";
                        candidateScore[i] -= 40000;
                        break;
                    case 1:
                        candidateScore[i] -= 20000;
                        break;
                    case 2:
                        candidateScore[i] -= 10000;
                        break;
                    case 3:
                        candidateScore[i] -= 4;
                        break;
                    case 4:
                        candidateScore[i] -= 1;
                        break;
                    case 5:
                        candidateScore[i] -= 0.25;
                        break;
                    case 6:
                        candidateScore[i] -= 0.1;
                        break;
                }
            }

            auto d = Distance(cMatrix->CellCentre(candidateCells[i]), cohesionCentroid);
            switch(cohesionMode) {
                case CohesionMode::NoCohesion:
                    break;
                case CohesionMode::ConstantCohesion:
                    candidateScore[i] -= 8*d*d*d;
                    break;
                case CohesionMode::MixedCriticality:
                    if (auto p1a = dynamic_cast<Protocol1Actuator *>(radioActuator); p1a != nullptr) {
                        if (clock - lastHC < 300) {
                            candidateScore[i] -= 8*d*d*d;
                        }
                    }
                    else {
                        THROW_ARGOSEXCEPTION("Cannot apply cohesion mode")
                    }
                    break;
                case CohesionMode::HalfCohesion:
                    candidateScore[i] -= d*d*d;
                    break;
            }

            auto thisVector = cMatrix->CellCentre(candidateCells[i]) - position;
            candidateScore[i] += thisVector.DotProduct(CVector2(1, orientation));
            candidateScore[i] -= cMatrix->FrontierDistance(candidateCells[i]);
        }

        auto bestCell = std::distance(candidateScore.begin(), std::max_element(candidateScore.begin(), candidateScore.end()));

        if (bestCell == 4) {
            goalStuck++;
        }
        else {
            goalStuck = 0;
        }

        goalCell = candidateCells[bestCell];
    }

    REGISTER_CONTROLLER(CDFrontierController, "dfrontier_controller");
}



