#include "dfrontier_loop_functions.h"
#include <argos3/plugins/robots/pi-puck/simulator/pipuck_entity.h>
#include "controllers/dfrontier/controller.h"

namespace argos {

    void DFrontierLoopFunctions::PreStep() {
        UInt32 covered = 0;
        UInt32 total = 0;

        auto nodes = CSimulator::GetInstance().GetSpace().GetEntitiesByType("pipuck");
        for(auto& [node_id, node_container] : nodes) {
            auto entity = any_cast<CPiPuckEntity *>(node_container);
            auto controller = dynamic_cast<CDFrontierController *>(&(entity->GetControllableEntity().GetController()));

            auto mDim = controller->cMatrix->xDim * controller->cMatrix->yDim;
            for(auto i=0; i<mDim; i++) {
                if (controller->cMatrix->matrix[i] > CoverageMatrix::CellStatus::Frontier) {
                    covered++;
                }
            }
            total += mDim;
        }

        if (CSimulator::GetInstance().GetSpace().GetSimulationClock() % 100 == 0) {
            LOG << "DATA:C:" << static_cast<Real>(covered) / static_cast<Real>(total) << "\n";
        }
    }

    void DFrontierLoopFunctions::PostExperiment() {}

    bool DFrontierLoopFunctions::IsExperimentFinished() {
        return false;
    }

    REGISTER_LOOP_FUNCTIONS(DFrontierLoopFunctions, "dfrontier_loop_functions")
}