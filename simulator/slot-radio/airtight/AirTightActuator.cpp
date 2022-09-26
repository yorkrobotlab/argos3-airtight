#include "AirTightActuator.h"
#include <argos3/core/simulator/simulator.h>
#include <numeric>

namespace argos {

    void AirTightActuator::Init(TConfigurationNode& t_tree) {
        SlotRadioActuator::Init(t_tree);
        std::string mediumName;
        GetNodeAttribute(t_tree, "medium", mediumName);
        medium = &CSimulator::GetInstance().GetMedium<SlotRadioMedium>(mediumName);
    }

    void AirTightActuator::Update() {
        const auto& clock = CSimulator::GetInstance().GetSpace().GetSimulationClock();

        bool ack = medium->ReceiveAck(this);

        if (txBuffer == nullptr && ack) {
            THROW_ARGOSEXCEPTION("Got ack from empty tx buffer");
        }
        else if (txBuffer != nullptr && !ack) {
            // Handle failed TX
            ++failedTransmissions;

            if (!highCriticalityMode && (failedTransmissions > FaultLoadDYNLUT(false, busyPeriodDuration))) {
                LOG << "[" << robot->GetId() << "] High Criticality Mode\n";
                highCriticalityMode = true;

                for(auto& buffer : buffers) {
                    if (!buffer.highCriticality) {
                        buffer.buffer.clear();
                    }
                }
            }

            // Not an else if, because we might need both if we just switched above and F(Hi,X) == F(Lo,X)
            if (highCriticalityMode && (failedTransmissions > FaultLoadDYNLUT(true, busyPeriodDuration))) {
                LOG << "[" << robot->GetId() << "] High Criticality Fault Load Exceeded\n";

                // In best efforts mode, drop failed transmissions if they have exceeded their response time. We don't
                // want to send frames later than their response times as this can mess up the jitter removal next frame
                // times of forwarding flows
                // TODO: Check if this is: a) sane?  and b) the best option?
                if (clock - txBuffer->buffer.front().queueTime >= txBuffer->responseTime[1]) {
                    txBuffer->buffer.pop_front();
                }
            }
        }
        else if (ack) {
            txBuffer->buffer.pop_front();
        }

        // Reset txBuffer
        txBuffer = nullptr;

        // Choose next txBuffer if we are scheduled to transmit in this slot
        if (!txSlots.empty() && txSlots[(clock - 1) % txSlots.size()]) {
            for (auto &buffer: buffers) {
                if (!buffer.buffer.empty() && (buffer.highCriticality >= highCriticalityMode) && (buffer.buffer.front().queueTime <= clock)) {
                    txBuffer = &buffer;
                    break;
                }
            }
            if (txBuffer == nullptr) {
                if (highCriticalityMode) {
                    LOG << "[" << robot->GetId() << "] Low Criticality Mode\n";
                    highCriticalityMode = false;
                }
                busyPeriodDuration = 0;
                failedTransmissions = 0;
            }
            else {
                medium->PushFrame(txBuffer->buffer.front(), this);
                ++busyPeriodDuration;
            }
        }
    }

    void AirTightActuator::QueueFrame(const std::string& bufferName, const RadioMessage& message) {
        const auto clock = CSimulator::GetInstance().GetSpace().GetSimulationClock();
        const auto flowID = GetBufferID(bufferName);
        auto& buffer = buffers[flowID];

        RadioFrame frame = {robot->GetId(),
                            buffer.recipient,
                            buffer.name,
                            message,
                            std::max(buffer.nextFrameTime, clock)};
        if (!highCriticalityMode || buffer.highCriticality) {
            buffer.buffer.emplace_back(frame);
            buffer.nextFrameTime = std::max(buffer.nextFrameTime, clock) + buffer.period;
        }
    }

    // TODO: Anything else?
    void AirTightActuator::Reset() {
        for(auto& buffer : buffers) {
            buffer.buffer.clear();
        }
        txBuffer = nullptr;
    }

    void AirTightActuator::SetRobotConfiguration(TConfigurationNode &t_tree) {
        auto& airTightConfig = GetNode(t_tree, "airtight");

        // Parse slot table
        auto slotTableString = airTightConfig.GetAttributeOrDefault("slotTable", "0");
        txSlots.clear();
        for(char c : slotTableString) {
            switch(c) {
                case '0':
                    txSlots.emplace_back(0);
                    break;
                case '1':
                    txSlots.emplace_back(1);
                    break;
                case ' ':
                    break;
                case ',':
                    break;
                default: THROW_ARGOSEXCEPTION("Couldn't parse node slot table");
            }
        }

        // Set buffers
        TConfigurationNodeIterator it;
        for(it = it.begin(&airTightConfig); it != it.end(); ++it) {
            if (it->Value() != "buffer") {
                THROW_ARGOSEXCEPTION("Unknown configuration element: " + it->Value());
            }

            const auto& name = it->GetAttribute("name");
            const auto& alias = it->GetAttribute("alias");
            const auto& recipient = it->GetAttribute("recipient");
            const auto& responseTimeLC = it->GetAttribute("RLo");
            const auto& responseTimeHC = it->GetAttributeOrDefault("RHi", "-1");
            const auto& period = it->GetAttribute("period");
            bool criticality = stoi(it->GetAttributeOrDefault("criticality", "0"));

            if (recipient.empty()) {
                THROW_ARGOSEXCEPTION("Buffer must have a recipient: "+ToString(*it));
            }
            if (name.empty()) {
                THROW_ARGOSEXCEPTION("Buffer must have a name: "+ToString(*it));
            }
            if (bufferNameIDLookup.contains(name)) {
                THROW_ARGOSEXCEPTION("Duplicate buffer name: "+name);
            }
            bufferNameIDLookup[name] = buffers.size();
            if (!alias.empty()) {
                if (bufferNameIDLookup.contains(alias)) {
                    THROW_ARGOSEXCEPTION("Alias duplicates buffer name: "+alias);
                }
                bufferNameIDLookup[alias] = buffers.size();
            }

            if (responseTimeLC.empty()) {
                THROW_ARGOSEXCEPTION("Buffer must have a low criticality response time: "+ToString(*it));
            }
            UInt32 RLo = stoi(responseTimeLC);
            UInt32 RHi = stoi(responseTimeHC);

            if (period.empty()) {
                THROW_ARGOSEXCEPTION("Buffer must have a period"+ ToString(*it));
            }
            UInt32 periodT = stoi(period);

            buffers.emplace_back(AirTightBuffer {name,recipient,{},criticality, {RLo, RHi}, periodT});
        }
    }

    UInt32 AirTightActuator::FaultLoad(bool critLevel, UInt32 slots) {
        /* TODO: These need to match what the configuration was generated with. Probably better to just make all the
         *       analysis happen dynamically at load time. We're likely to need dynamic re-analysis later anyway? */
        double pdr, targetConfidence;
        if (critLevel == 1) {
            pdr = medium->PDR(3.0);
            targetConfidence = 0.99999;
        }
        else {
            pdr = medium->PDR(2.0);
            targetConfidence = 0.999;
        }

        UInt32 m = 0;
        double achievedConfidence = 0;
        do {
            ++m;
            achievedConfidence = 0;
            for(UInt32 k = 0; k < m; k++) {
                achievedConfidence += binomial(slots, k) * pow(1-(pdr*pdr), k) * pow(pdr*pdr, slots-k);
            }
        } while (achievedConfidence < targetConfidence);

        return --m;
    }

    REGISTER_ACTUATOR(AirTightActuator,
                      "slot_radio",
                      "airtight",
                      "Sven Signer",
                      "1.0",
                      "Brief Description",
                      "Long Description",
                      "status")

}