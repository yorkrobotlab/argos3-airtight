#ifndef ARGOS3_AIRTIGHT_CIRCLE_LOOP_FUNCTIONS_H
#define ARGOS3_AIRTIGHT_CIRCLE_LOOP_FUNCTIONS_H

#include <map>
#include <argos3/core/simulator/loop_functions.h>

namespace argos {

    class CircleLoopFunctions : public CLoopFunctions {

    public:
        void Init(TConfigurationNode &t_tree) override;
        void Reset() override;
        void PostStep() override;
        bool IsExperimentFinished() override;

        void SetCorrectColour(UInt32 time, UInt32 node, UInt32 colour);

    private:
        bool isFinished = false;
        std::map<std::pair<UInt32, UInt32>, UInt32> correctColourMap;
    };
}

#endif //ARGOS3_AIRTIGHT_CIRCLE_LOOP_FUNCTIONS_H
