#include <argos3/core/simulator/simulator.h>
#include "Protocol1Actuator.h"


namespace argos {

    void Protocol1Actuator::SwitchHC() {
        highCriticalityMode = true;
        LOG << "HIGH CRIT MODE\n";
        for(auto& buffer : buffers) {
            if (!buffer.highCriticality) {
                buffer.buffer.clear();
                buffer.receivedAcks.clear();
            }
        }
    }

    void Protocol1Actuator::HandleDelayedAck(const Protocol1Frame *frame) {
        auto clock = CSimulator::GetInstance().GetSpace().GetSimulationClock();
        auto slot = (clock - 1) % txSlots.size();

        // If this frame contained a message, set this actuator to ack it
        if (frame->frameMsg.has_value()) {
            pendingAcks[slot] = true;
        }

        for(ssize_t i=0; i<txSlots.size(); i++) {
            // Check it the received frame is ack-ing one of our frames
            if (txSlots[i] && frame->delayedAcks[i]) {
                if (txBuffer == nullptr) {
                    THROW_ARGOSEXCEPTION("Got unexpected ack?");
                }
                else {
                    txBuffer->receivedAcks.emplace(frame->frameFrom);
                }
            }
        }
    }

    void Protocol1Actuator::Update() {
        auto clock = CSimulator::GetInstance().GetSpace().GetSimulationClock();
        // Ensure txPort is null unless we explicitly set it
        txPort = nullptr;

        // Reset the pendingAck for this slot, the sensor executes after the
        // actuator and will set it again if applicable.
        pendingAcks[(clock - 1) % txSlots.size()] = false;

        // Choose next txBuffer if we are scheduled to transmit in this slot
        if (!txSlots.empty() && txSlots[(clock - 1) % txSlots.size()]) {
            // We transmitted during the last TX slot, check for acks now
            if (txBuffer != nullptr) {
                // We've received enough acks, treat this as delivered
                if (txBuffer->receivedAcks.size() >= txBuffer->targetAcks) {
                    if (txBuffer->buffer.empty()) {
                        THROW_ARGOSEXCEPTION("txBuffer unexpectedly empty after message delivery?");
                    }
                    txBuffer->buffer.pop_front();
                    txBuffer->receivedAcks.clear();
                }
                // Else we've not received enough acks yet
                else {
                    ++failedTransmissions;

                    // FIXME: There should be some fault model for this
                    if (!highCriticalityMode && (failedTransmissions > (5 + 0.5*busyPeriodDuration))) { //failedTransmissions > FaultModel(busyPeriodDuration, false))) {
                        SwitchHC();
                    }
                    if (highCriticalityMode) { //failedTransmissions > FaultModel(busyPeriodDuration, true))) {
                        // TODO: Same question here as for the vanilla AirTight implementation
                        if (!(txBuffer->buffer.empty())) {
                            if (clock - txBuffer->buffer.front().queueTime >= txBuffer->responseTime[1]) {
                                txBuffer->buffer.pop_front();
                                txBuffer->receivedAcks.clear();
                            }
                        }
                        /*if (clock - txBuffer->buffer.front().queueTime >= txBuffer->responseTime[1]) {
                            if (txBuffer->buffer.empty()) {
                                LOG << "Ignoring empty txBuffer, presumably cleared by HC mode change?\n";
                            }
                            else {
                                txBuffer->buffer.pop_front();
                                txBuffer->receivedAcks.clear();
                            }
                        }*/
                    }
                }
            }

            // Reset txBuffer
            txBuffer = nullptr;

            // Select next txBuffer
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
                txPort = nullptr;
                busyPeriodDuration = 0;
                failedTransmissions = 0;

                // If we don't have anything to send, we need to send an empty message so that we can ack
                dummyFrame->delayedAcks = pendingAcks;
                txPort = dummyFrame;
            }
            else {
                // Set the delayedAcks to be included in the frame
                txBuffer->buffer.front().delayedAcks = pendingAcks;
                txPort = &txBuffer->buffer.front();
                ++busyPeriodDuration;
            }
        }
    }

    void Protocol1Actuator::QueueFrame(const std::string& bufferName, const std::any& message) {
        const auto clock = CSimulator::GetInstance().GetSpace().GetSimulationClock();
        const auto flowID = GetBufferID(bufferName);
        auto& buffer = buffers[flowID];

        if (!message.has_value()) {
            THROW_ARGOSEXCEPTION("Frame must have a message");
        }

        if (!highCriticalityMode || buffer.highCriticality) {
            buffer.buffer.emplace_back(robot->GetId(), "", buffer.name, message, std::max(buffer.nextFrameTime, clock));
            buffer.nextFrameTime = clock;
            //buffer.nextFrameTime = std::max(buffer.nextFrameTime, clock) + buffer.period;
        }
    }

    // TODO: Anything else?
    void Protocol1Actuator::Reset() {
        for(auto& buffer : buffers) {
            buffer.buffer.clear();
            buffer.receivedAcks.clear();
        }
        txBuffer = nullptr;
    }

    void Protocol1Actuator::SetRobotConfiguration(TConfigurationNode &t_tree) {
        auto& config = GetNode(t_tree, "protocol1");

        // Parse slot table
        auto slotTableString = config.GetAttributeOrDefault("slotTable", "0");
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
                    [[fallthrough]];
                case ',':
                    break;
                default: THROW_ARGOSEXCEPTION("Couldn't parse node slot table");
            }
        }
        pendingAcks = std::vector<bool>(txSlots.size(), false);

        // Set buffers
        TConfigurationNodeIterator it;
        for(it = it.begin(&config); it != it.end(); ++it) {
            if (it->Value() != "buffer") {
                THROW_ARGOSEXCEPTION("Unknown configuration element: " + it->Value());
            }

            const auto& name = it->GetAttribute("name");
            const auto& alias = it->GetAttribute("alias");
            //const auto& responseTimeLC = it->GetAttribute("RLo");
            //const auto& responseTimeHC = it->GetAttributeOrDefault("RHi", "-1");
            const auto& period = it->GetAttribute("period");
            const auto& targetAcksS = it->GetAttribute("targetAcks");
            bool criticality = stoi(it->GetAttributeOrDefault("criticality", "0"));

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
            /*if (responseTimeLC.empty()) {
                THROW_ARGOSEXCEPTION("Buffer must have a low criticality response time: "+ToString(*it));
            }
            UInt32 RLo = stoi(responseTimeLC);
            UInt32 RHi = stoi(responseTimeHC);*/


            if (period.empty()) {
                THROW_ARGOSEXCEPTION("Buffer must have a period"+ ToString(*it));
            }
            UInt32 periodT = stoi(period);

            if (targetAcksS.empty()) {
                THROW_ARGOSEXCEPTION("Buffer must state targeted number of acknowledgements "+ToString(*it));
            }
            UInt32 targetAcks = stoi(targetAcksS);

            buffers.emplace_back(Protocol1Buffer {name,{},{},criticality, {periodT, periodT}, periodT, 0, targetAcks});
        }
    }

    void Protocol1Actuator::SetRobot(CComposableEntity &robot) {
        SlotRadioActuator::SetRobot(robot);
        dummyFrame = new Protocol1Frame(robot.GetId(), "", "", std::any(), 0);
    }

    REGISTER_ACTUATOR(Protocol1Actuator,
                      "slot_radio",
                      "protocol1",
                      "Sven Signer",
                      "1.0",
                      "Brief Description",
                      "Long Description",
                      "status")

}