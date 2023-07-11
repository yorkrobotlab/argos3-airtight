#ifndef COVERAGEMATRIX_H
#define COVERAGEMATRIX_H

#include <array>
#include <set>

#include <argos3/core/utility/math/range.h>
#include <argos3/core/utility/math/vector3.h>

namespace argos {
    class CoverageMatrix {
    public:
        typedef std::pair<UInt32, UInt32> Cell;

        enum class CellStatus : unsigned char {
            Unknown = 0, Frontier, Free, Occupied
        };

        CoverageMatrix(const CRange<CVector3> arenaLimits, Real gridResolution);

        virtual ~CoverageMatrix() {
            free(matrix);
        }

        CellStatus GetValue(const Cell cell) const;
        void SetValue(const Cell cell, CellStatus value);

        bool CellValid(const Cell& cell) const;
        Cell PositionToCell(const CVector2 position) const;
        CVector2 CellCentre(const Cell cell) const;
        Real FrontierDistance(const Cell cell) const;

    private:
        friend class DFrontierOpenGLFunctions;
        friend class DFrontierLoopFunctions;

        const Real resolution;
        const SInt32 xOffset, yOffset;
        const UInt32 xDim, yDim;
        CellStatus* const matrix;
        CellStatus* matrixValue(Cell cell);

        std::set<Cell> frontierCells;
    };

    // Number of "moves" between two cells, allowing for diagonal moves
    inline int CellDistance(const CoverageMatrix::Cell a, const CoverageMatrix::Cell b) {
        auto d1 = abs(SInt32(a.first) - SInt32(b.first));
        auto d2 = abs(SInt32(a.second) - SInt32(b.second));
        return std::max(d1, d2);
    }

}

#endif //COVERAGEMATRIX_H
