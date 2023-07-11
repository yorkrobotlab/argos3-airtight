#ifndef ARGOS3_AIRTIGHT_DFRONTIER_OPENGL_FUNCTIONS_H
#define ARGOS3_AIRTIGHT_DFRONTIER_OPENGL_FUNCTIONS_H

#include <argos3/plugins/robots/pi-puck/simulator/pipuck_entity.h>
#include <argos3/plugins/simulator/visualizations/qt-opengl/qtopengl_user_functions.h>
#include "controllers/dfrontier/coverage_matrix.h"

namespace argos {

    class DFrontierOpenGLFunctions : public CQTOpenGLUserFunctions {
    public:
        DFrontierOpenGLFunctions() {};
        virtual ~DFrontierOpenGLFunctions() {};

        void EntitySelected(CEntity &entity) override;

        void EntityDeselected(CEntity &entity) override;

        void DrawInWorld() override;

    private:
        CoverageMatrix *cMatrixOfSelected = nullptr;
    };

} // argos

#endif //ARGOS3_AIRTIGHT_DFRONTIER_OPENGL_FUNCTIONS_H
