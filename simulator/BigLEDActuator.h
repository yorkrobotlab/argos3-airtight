#ifndef ARGOS3_AIRTIGHT_BIGLED_H
#define ARGOS3_AIRTIGHT_BIGLED_H

#include <argos3/core/utility/datatypes/color.h>
#include <argos3/core/control_interface/ci_actuator.h>
#include <argos3/core/simulator/actuator.h>

namespace argos {

    class BigLEDActuator: public CSimulatedActuator, public CCI_Actuator {

    public:
        void SetLEDOn(bool state);
        bool GetLEDOn();
        void SetColour(CColor colour);
        CColor GetColour();
        virtual void SetRobot(CComposableEntity &c_entity);

        virtual void Update() {};

    private:
        CColor ringColour;
        bool on;
    };

}

#endif
