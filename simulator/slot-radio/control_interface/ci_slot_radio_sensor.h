#ifndef CI_TDMA_RADIO_SENSOR_H
#define CI_TDMA_RADIO_SENSOR_H

#include <any>
#include <argos3/core/control_interface/ci_sensor.h>

namespace argos {
    class CI_SlotRadioSensor : virtual public CCI_Sensor {
    public:
        virtual void SetRobotConfiguration(TConfigurationNode &t_tree) = 0;
        virtual bool HasFrame() = 0;
        virtual const std::any& GetMessage() = 0;
    };
}

#endif //CI_TDMA_RADIO_SENSOR_H
