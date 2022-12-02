#include "flocking_loop_functions.h"
#include "../controllers/flocking/controller.h"
#include <argos3/plugins/robots/pi-puck/simulator/pipuck_entity.h>
#include <set>

namespace argos {

    void FlockingLoopFunctions::Init(TConfigurationNode &t_tree) {
        const auto nodes = CSimulator::GetInstance().GetSpace().GetEntitiesByType("pipuck");

        // Ensure there are node hash collisions for node IDs
        for(const auto& [node_id, node_container] : nodes) {
            auto entity = any_cast<CPiPuckEntity*>(node_container);
            auto controller = dynamic_cast<CFlockingController*>(&(entity->GetControllableEntity().GetController()));
            auto& position = entity->GetEmbodiedEntity().GetOriginAnchor().Position;

            auto [iterator, success] = nodePositions.insert({controller->nodeID, position});
            if (!success) {
                THROW_ARGOSEXCEPTION("Node ID Hash Collision");
            }
        }
    }

    void FlockingLoopFunctions::PreStep() {
        UInt32 clock = CSimulator::GetInstance().GetSpace().GetSimulationClock();
        CVector3 mean_velocity, centroid;
        auto nodes = CSimulator::GetInstance().GetSpace().GetEntitiesByType("pipuck");
        for(auto& [node_id, node_container] : nodes) {
            auto entity = any_cast<CPiPuckEntity*>(node_container);

            const auto& anchor = entity->GetEmbodiedEntity().GetOriginAnchor();
            auto& position = anchor.Position;
            auto& orientation = anchor.Orientation;

            // LOG << "ORIENTATION: " << orientation.GetW() << " " << orientation.GetX() << " " << orientation.GetY() << " " << orientation.GetZ() << "\n";

            auto controller = dynamic_cast<CFlockingController*>(&(entity->GetControllableEntity().GetController()));
            if (controller == nullptr) {
                THROW_ARGOSEXCEPTION("Controller is not a flocking controller?")
            }
            else {
                controller->position = position;
                controller->orientation = orientation;
            }

            mean_velocity += position - nodePositions[controller->nodeID];
            nodePositions[controller->nodeID] = CVector3(position);
            centroid += position;
        }
        centroid /= nodes.size();
        Real maxCentroidDistance = 0.0;
        for (auto &[node_id, position]: nodePositions) {
            auto distance = (position - centroid).Length();
            if (distance > maxCentroidDistance) {
                maxCentroidDistance = distance;
            }
        }
        meanMaxCentroidDistance += maxCentroidDistance;
        meanVelocity += mean_velocity.Length() / nodes.size();
    }

    bool FlockingLoopFunctions::IsExperimentFinished() {
        return isFinished;
    }

    void FlockingLoopFunctions::PostExperiment() {
        UInt32 clock = CSimulator::GetInstance().GetSpace().GetSimulationClock();

        auto dataPoints = 0;
        auto nodes = CSimulator::GetInstance().GetSpace().GetEntitiesByType("pipuck");
        for(auto& [node_id, node_container] : nodes) {
            auto entity = any_cast<CPiPuckEntity *>(node_container);
            auto controller = dynamic_cast<CFlockingController *>(&(entity->GetControllableEntity().GetController()));
            if (controller->dataSinkNode) {
                dataPoints += controller->dataSink.size();
            }
        }
        LOG << "Mean velocity was " << meanVelocity / clock <<
            " Mean max centroid distance was " << meanMaxCentroidDistance / clock <<
            " Data points " << dataPoints << "\n";
    }

    void FlockingLoopFunctions::Reset() {
        isFinished = false;
    }

    REGISTER_LOOP_FUNCTIONS(FlockingLoopFunctions, "flocking_loop_functions")
}