#include "BigLEDActuator.h"

namespace argos {

    void BigLEDActuator::SetRobot(CComposableEntity &c_entity) {
        ringColour = CColor(255.0, 0.0, 0.0);
        on = false;
    }

    void BigLEDActuator::SetColour(CColor colour) {
        ringColour = colour;
    }

    CColor BigLEDActuator::GetColour() {
        return ringColour;
    }

    void BigLEDActuator::SetLEDOn(bool state) {
        on = state;
    }

    bool BigLEDActuator::GetLEDOn() {
        return on;
    }

    REGISTER_ACTUATOR(BigLEDActuator,
                     "big_leds", "default",
                     "Sven Signer",
                     "0.1",
                     "A very big LED",
                     "When running GUI simulations the real pipuck LEDs are far to small to be easily visible. Add \n"
                     "a new LED actuator that renders a huge circle of colour on the floor around a pipuck so that \n"
                     "we can easily see what is happening (requires circle_opengl_functions to be loaded).",
                     "status");
}