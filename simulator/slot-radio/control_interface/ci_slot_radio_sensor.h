#ifndef CI_TDMA_RADIO_SENSOR_H
#define CI_TDMA_RADIO_SENSOR_H

#include <argos3/core/control_interface/ci_sensor.h>
#include "../radio_frame.h"

namespace argos {
    class CI_SlotRadioSensor : virtual public CCI_Sensor {
    public:
        virtual void SetRobotConfiguration(TConfigurationNode &t_tree) = 0;
        virtual bool HasFrame() = 0;
        virtual const RadioMessage& GetMessage() = 0;
    };
}

#endif //CI_TDMA_RADIO_SENSOR_H
