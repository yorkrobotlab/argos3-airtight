#ifndef ARGOS3_AIRTIGHT_DFRONTIER_LOOP_FUNCTIONS_H
#define ARGOS3_AIRTIGHT_DFRONTIER_LOOP_FUNCTIONS_H

#include <argos3/core/simulator/loop_functions.h>
#include "controllers/dfrontier/coverage_matrix.h"

namespace argos {

    class DFrontierLoopFunctions : public CLoopFunctions {

    public:
        void PreStep() override;
        void PostExperiment() override;
        bool IsExperimentFinished() override;
    };
}

#endif //ARGOS3_AIRTIGHT_DFRONTIER_LOOP_FUNCTIONS_H
