#include "coverage_matrix.h"

#include <queue>
#include <set>

namespace argos {

    CoverageMatrix::CoverageMatrix(const CRange<CVector3> arenaLimits, Real gridResolution) :
    resolution(gridResolution),
    xOffset(-round(arenaLimits.GetMin().GetX()*gridResolution)),
    yOffset(-round(arenaLimits.GetMin().GetY()*gridResolution)),
    xDim(1+round(arenaLimits.GetMax().GetX()*gridResolution)+xOffset),
    yDim(1+round(arenaLimits.GetMax().GetY()*gridResolution)+yOffset),
    matrix(reinterpret_cast<CellStatus *>(calloc(xDim*yDim, sizeof(CellStatus)))) {
        if (matrix == nullptr) {
            THROW_ARGOSEXCEPTION("Failed to allocate coverage matrix");
        }
    }

    CoverageMatrix::Cell CoverageMatrix::PositionToCell(const CVector2 position) const {
        UInt32 xCell = lround(position.GetX()*resolution) + xOffset;
        UInt32 yCell = lround(position.GetY()*resolution) + yOffset;

        if (!CellValid({xCell, yCell})) {
            THROW_ARGOSEXCEPTION("PositionToCell out of range")
        }

        return {xCell, yCell};
    }

    CVector2 CoverageMatrix::CellCentre(const Cell cell) const {
        Real xPos = (static_cast<Real>(cell.first) - static_cast<Real>(xOffset)) / resolution;
        Real yPos = (static_cast<Real>(cell.second) - static_cast<Real>(yOffset)) / resolution;

        // TODO: Is this check even necessary?
        if (!CellValid(cell)) {
            THROW_ARGOSEXCEPTION("CellToPosition out of range");
        }

        return {xPos, yPos};
    }

    bool CoverageMatrix::CellValid(const CoverageMatrix::Cell &cell) const {
        // Since coordinates are unsigned, we don't need to check the smaller 0 case: "negative" values wrap around
        // to become very large, and hence will be outside the dimension check
        return (cell.first < xDim) && (cell.second < yDim);
    }

    CoverageMatrix::CellStatus CoverageMatrix::GetValue(const Cell cell) const {
        return matrix[cell.first*xDim + cell.second];
    }

    void CoverageMatrix::SetValue(const Cell cell, CellStatus value) {
        CellStatus* cellValue = matrixValue(cell);

        // Neighbours might become frontiers
        if (value == CellStatus::Free) {
            Cell neighbours[8] = {{cell.first-1, cell.second},
                                  {cell.first-1, cell.second-1},
                                  {cell.first-1,cell.second+1},
                                  {cell.first, cell.second-1},
                                  {cell.first, cell.second+1},
                                  {cell.first+1, cell.second},
                                  {cell.first+1, cell.second-1},
                                  {cell.first+1, cell.second+1}};

            for(auto neighbourCell : neighbours) {
                if (!CellValid(neighbourCell)) continue;

                auto neighbourAddress = matrixValue(neighbourCell);

                if (*neighbourAddress == CellStatus::Unknown) {
                    *neighbourAddress = CellStatus::Frontier;
                    frontierCells.insert(neighbourCell);
                }
            }
        }

        // TODO: Cleanup frontiers here?
        if (*cellValue == CellStatus::Frontier && value != CellStatus::Frontier) {
            frontierCells.erase(cell);
        }

        *matrixValue(cell) = value;
    }

    CoverageMatrix::CellStatus *CoverageMatrix::matrixValue(const CoverageMatrix::Cell cell) {
        return &matrix[cell.first*xDim + cell.second];
    }

    Real CoverageMatrix::FrontierDistance(const Cell cell) const {
        auto minDistance = std::numeric_limits<Real>::max();

        for(const auto& fCell: frontierCells) {
            // auto d = Distance(CellCentre(cell), CellCentre(fCell));
            auto d = CellDistance(cell, fCell);
            if (d < minDistance) {
                minDistance = d;
            }
        }

        return minDistance;
    }

}