#ifndef LED_USER_FUNCTIONS_H
#define LED_USER_FUNCTIONS_H

#include <argos3/plugins/simulator/visualizations/qt-opengl/qtopengl_user_functions.h>
#include <argos3/plugins/robots/pi-puck/simulator/pipuck_entity.h>
#include "../simulator/BigLEDActuator.h"

namespace argos {

    class CircleOpenGLFunctions : public CQTOpenGLUserFunctions {
    public:
        CircleOpenGLFunctions();
        virtual ~CircleOpenGLFunctions() {}
        void Draw(CPiPuckEntity &c_entity);
    };
}
#endif
