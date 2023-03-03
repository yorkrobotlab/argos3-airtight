#ifndef ARGOS3_AIRTIGHT_FLOCKING_LOOP_FUNCTIONS_H
#define ARGOS3_AIRTIGHT_FLOCKING_LOOP_FUNCTIONS_H

#include <argos3/core/simulator/loop_functions.h>

namespace argos {

    class FlockingLoopFunctions : public CLoopFunctions {

    public:
        void Init(TConfigurationNode &t_tree) override;
        void Reset() override;
        void PreStep() override;
        void PostExperiment() override;
        bool IsExperimentFinished() override;
    private:
        bool isFinished = false;
        std::map<ssize_t, CVector3> nodePositions;
        Real meanVelocity = 0.0;
        Real meanMaxCentroidDistance = 0.0;
    };
}

#endif //ARGOS3_AIRTIGHT_FLOCKING_LOOP_FUNCTIONS_H
