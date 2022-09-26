#include "circle_opengl_functions.h"

namespace argos {

    CircleOpenGLFunctions::CircleOpenGLFunctions() {
        RegisterUserFunction<CircleOpenGLFunctions, CPiPuckEntity>(&CircleOpenGLFunctions::Draw);
    }

    void CircleOpenGLFunctions::Draw(CPiPuckEntity &c_entity) {
        auto position = c_entity.GetEmbodiedEntity().GetOriginAnchor().Position;
        auto actuator = c_entity.GetControllableEntity().GetController().GetActuator<BigLEDActuator>("big_leds");

        // Draw Big LED
        if (actuator->GetLEDOn()) {
            DrawCircle(position + CVector3(0.0, 0.0, 0.01),
                       CQuaternion(),
                       0.25,
                       actuator->GetColour(),
                       true,
                       1 << 6);
        }

        // Draw Radius Circle
        DrawCircle(CVector3(0.0, 0.0, 0.005),
                   CQuaternion(),
                   position.Length(),
                   CColor(0, 0, 0),
                   false,
                   1 << 6);
    }

    REGISTER_QTOPENGL_USER_FUNCTIONS(CircleOpenGLFunctions, "circle_opengl_functions")
}