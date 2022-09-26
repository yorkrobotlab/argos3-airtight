#include "circle_loop_functions.h"
#include "../simulator/BigLEDActuator.h"
#include <argos3/plugins/robots/pi-puck/simulator/pipuck_entity.h>
#include <set>

namespace argos {

    void CircleLoopFunctions::Init(TConfigurationNode &t_tree) {
        const auto nodes = CSimulator::GetInstance().GetSpace().GetEntitiesByType("pipuck");
        std::set<size_t> nodeIDHashes;

        // Ensure there are node hash collisions for node IDs
        for(const auto& node : nodes) {
            nodeIDHashes.insert(std::hash<std::string>{}(node.first));
        }
        if (nodes.size() != nodeIDHashes.size()) {
            THROW_ARGOSEXCEPTION("Node ID Hash Collision");
        }

        correctColourMap.emplace(std::make_pair(std::make_pair(0,0), CColor(255, 0, 0)));
    }

    void CircleLoopFunctions::PostStep() {
        const auto nodes = CSimulator::GetInstance().GetSpace().GetEntitiesByType("pipuck");
        UInt32 clock = CSimulator::GetInstance().GetSpace().GetSimulationClock();
        //std::set<UInt32> colours;

        CVector3 centre;
        Real minDistance = 10e8;
        Real maxDistance = 0;

        int incorrectColourNodes = 0;

        UInt32 correctColour = CColor(255, 0, 0);
        for(auto it = correctColourMap.rbegin(); it != correctColourMap.rend(); it++) {
            if (it->first.first <= clock) {
                correctColour = it->second;
                break;
            }
        }

        auto cc2 = (--correctColourMap.upper_bound(std::make_pair(clock, UINT32_MAX)))->second;
        if (cc2 != correctColour) {
            THROW_ARGOSEXCEPTION("OH DEAR!");
        }

        for(const auto& [id, node_container] : nodes) {
            auto& node = any_cast<CPiPuckEntity*>(node_container);
            auto distance = node->GetComponent<CEmbodiedEntity>("body").GetOriginAnchor().Position.Length();

            if (distance < minDistance) {
                minDistance = distance;
            }
            if (distance > maxDistance) {
                maxDistance = distance;
            }

            auto led = node->GetComponent<CControllableEntity>("controller").GetController().GetActuator<BigLEDActuator>("big_leds");

            UInt32 colour = led->GetColour();
            if (colour != correctColour) {
                //colours.insert(colour);
                incorrectColourNodes++;
            }
        }

        LOG << "DATA: Clock: " << clock <<
               " MaxDev: " << (maxDistance - minDistance) <<
               " Colours: " << incorrectColourNodes <<
               " RMin: " << minDistance <<
               " RMax: " << maxDistance <<"\n";

        // End the experiment before the circle radius becomes to large for the arena
        if (maxDistance > 9.8) {
            isFinished = true;
        }

        if (clock % 3000 == 0) {
            LOG << "DATA: RMin: " << minDistance << " RMax: " << maxDistance << "\n";
        }
    }

    bool CircleLoopFunctions::IsExperimentFinished() {
        return isFinished;
    }

    void CircleLoopFunctions::Reset() {
        isFinished = false;
    }

    void CircleLoopFunctions::SetCorrectColour(UInt32 time, UInt32 node, UInt32 colour) {
        correctColourMap.emplace(std::make_pair(time, node), colour);
    }

    REGISTER_LOOP_FUNCTIONS(CircleLoopFunctions, "circle_logging_loop_functions")
}