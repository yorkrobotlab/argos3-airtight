#include "dfrontier_opengl_functions.h"
#include "controllers/dfrontier/controller.h"
#include <argos3/core/simulator/entity/floor_entity.h>

namespace argos {

    void DFrontierOpenGLFunctions::EntityDeselected(CEntity &entity) {
        CQTOpenGLUserFunctions::EntityDeselected(entity);
        cMatrixOfSelected = nullptr;
    }

    void DFrontierOpenGLFunctions::EntitySelected(CEntity &entity) {
        CQTOpenGLUserFunctions::EntitySelected(entity);
        try {
            auto& puck = dynamic_cast<CPiPuckEntity &>(entity);
            const auto& controller = dynamic_cast<const CDFrontierController &>(puck.GetControllableEntity().GetController());
            cMatrixOfSelected = controller.cMatrix;
        }
        catch (const std::bad_cast &e) {
            cMatrixOfSelected = nullptr;
        }
    }

    void DFrontierOpenGLFunctions::DrawInWorld() {
        CQTOpenGLUserFunctions::DrawInWorld();
        if (cMatrixOfSelected != nullptr && cMatrixOfSelected->matrix != nullptr) {
            for(UInt32 x=0; x<cMatrixOfSelected->xDim; x++) {
                for(UInt32 y=0; y<cMatrixOfSelected->yDim; y++) {
                    CVector2 cellCentre = cMatrixOfSelected->CellCentre({x, y});
                    CVector3 position {cellCentre.GetX(), cellCentre.GetY(), 0.005};

                    CColor colour;
                    switch(cMatrixOfSelected->GetValue({x, y})) {
                        case CoverageMatrix::CellStatus::Unknown:
                            colour = CColor::GREEN;
                            break;
                        case CoverageMatrix::CellStatus::Free:
                            colour = CColor::WHITE;
                            break;
                        case CoverageMatrix::CellStatus::Occupied:
                            colour = CColor::BLACK;
                            break;
                        case CoverageMatrix::CellStatus::Frontier:
                            colour = CColor::RED;
                            break;
                    }

                    auto rad = 0.5 / cMatrixOfSelected->resolution;

                    DrawPolygon(position,
                               CQuaternion(),
                                {{-rad, -rad}, {rad, -rad}, {rad, rad}, {-rad, rad}},
                                colour);
                }
            }
        }
    }

    REGISTER_QTOPENGL_USER_FUNCTIONS(DFrontierOpenGLFunctions, "dfrontier_opengl_functions")
} // argos