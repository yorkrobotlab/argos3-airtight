#ifndef ARGOS3_AIRTIGHT_AIRTIGHTANALYSIS_H
#define ARGOS3_AIRTIGHT_AIRTIGHTANALYSIS_H

#include <functional>
#include <vector>
#include "AirTightBuffer.h"


namespace argos {

    double BinomialCoefficient(const double n, const double k) ;

    unsigned int FaultModel(unsigned int slots, bool highCrit, unsigned int hint = 0) ;

    unsigned int SlotRequirementLC(int i, std::vector<AirTightBuffer> *buffers, std::function<unsigned int(unsigned int)> SupplyFn) ;
    unsigned int SlotRequirementHC(int i, std::vector<AirTightBuffer> *buffers, std::function<unsigned int(unsigned int)> SupplyFn) ;

    void ComputeResponseTimes(std::vector<AirTightBuffer> *buffers, const std::vector<bool> &slotTable);
}

#endif //ARGOS3_AIRTIGHT_AIRTIGHTANALYSIS_H
