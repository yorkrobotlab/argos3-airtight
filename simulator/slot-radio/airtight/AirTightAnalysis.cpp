#include <cmath>
#include <functional>
#include <numeric>
#include <vector>

#include "AirTightBuffer.h"
#include "AirTightAnalysis.h"


#define PDR(distance) (0.99 / (1.0 + ((1.0 / 3.0) * (distance - 1.5)) * ((1.0 / 3.0) * (distance - 1.5))))

namespace argos {

        double BinomialCoefficient(const double n, const double k) {
            return exp(lgamma(n+1)-lgamma(k+1)-lgamma(n-k+1));
        }

        unsigned int FaultModel(unsigned int slots, bool highCrit, unsigned int hint) {
            double confidenceBound, assumedMinPDRSquared;
            if (highCrit) {
                confidenceBound = 0.99999;
                assumedMinPDRSquared = PDR(3.0) * PDR(3.0);
            }
            else {
                confidenceBound = 0.999;
                assumedMinPDRSquared = PDR(2.0) * PDR(2.0);
            }

            unsigned int maxFailures = hint;
            double probability;

            do {
                probability = 0.0;
                for(int failures=0; failures<=maxFailures; failures++) {
                    probability += BinomialCoefficient(slots, failures) * pow(assumedMinPDRSquared, slots-failures) * pow(1.0-assumedMinPDRSquared, failures);
                }
            } while (probability < confidenceBound && ++maxFailures);

            if (highCrit) {
                // TODO: Verify me
                return std::max(maxFailures, FaultModel(slots, false, hint));
            }
            else {
                return maxFailures;
            }
        }

        unsigned int SlotRequirementLC(int i, std::vector<AirTightBuffer> *buffers, std::function<unsigned int(unsigned int)> SupplyFn) {
            unsigned int x = 1 + FaultModel(1, false);
            while (x <= (*buffers)[i].period) {
                unsigned int x_new = 1 + FaultModel(x, false);
                for(int k=0; k<i; k++) {
                    x_new += std::ceil((double)SupplyFn(x) / (double)(*buffers)[k].period);
                }
                if (x_new == x) {
                    return x_new;
                }
                x = x_new;
            }
            throw std::runtime_error("Unschedulable AirTight configuration");
        }

        unsigned int SlotRequirementHC(int i, std::vector<AirTightBuffer> *buffers, std::function<unsigned int(unsigned int)> SupplyFn) {
            unsigned int x = 1 + FaultModel(1, true);
            while (x <= (*buffers)[i].period) {
                unsigned int x_new = 1 + FaultModel(x, true);
                for (int k=0; k<i; k++) {
                    if ((*buffers)[k].highCriticality) {
                        x_new += std::ceil((double)SupplyFn(x) / (double)(*buffers)[k].period);
                    }
                    else {
                        x_new += std::ceil((double)(*buffers)[i].responseTime[0] / (double)(*buffers)[k].period);
                    }
                }
                if (x_new == x) {
                    return x_new;
                }
                x = x_new;
            }
            throw std::runtime_error("Unschedulable AirTight configuration");
        }

        void ComputeResponseTimes(std::vector<AirTightBuffer> *buffers, const std::vector<bool> &slotTable) {
            const double slotTableSlots = std::accumulate(slotTable.begin(), slotTable.end(), 0u);
            const double slotTableLength = slotTable.size();
            auto SupplyFn = [slotTableSlots, slotTableLength](unsigned int requiredSlots) {
                return static_cast<unsigned int>(1 + std::ceil(requiredSlots / slotTableSlots) * slotTableLength);
            };

            printf("HOOK\n");

            for (int i=0; i<buffers->size(); i++) {
                (*buffers)[i].responseTime[0] = SupplyFn(SlotRequirementLC(i, buffers, SupplyFn));
                if ((*buffers)[i].highCriticality) {
                    (*buffers)[i].responseTime[1] = SupplyFn(SlotRequirementHC(i, buffers, SupplyFn));
                }
            }
        }
}
