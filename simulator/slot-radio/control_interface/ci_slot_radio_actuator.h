#ifndef CI_TDMA_RADIO_ACTUATOR_H
#define CI_TDMA_RADIO_ACTUATOR_H

#include <any>
#include <argos3/core/control_interface/ci_actuator.h>

namespace argos {
    class CI_SlotRadioActuator : virtual public CCI_Actuator {
    public:
        virtual void SetRobotConfiguration(TConfigurationNode &t_tree) = 0;
        virtual void QueueFrame(const std::string &, const std::any &) = 0;
    };
}

#endif //CI_TDMA_RADIO_ACTUATOR_H
