#ifndef TYPES_H
#define TYPES_H

// suppress warnings: https://stackoverflow.com/a/28166605
#ifdef __GNUC__
#ifndef __clang__
#pragma GCC diagnostic ignored "-Wshadow"
#else
#pragma clang diagnostic ignored "-Wshadow"
#endif
#endif

#include "QSynthi2/Juce.h"
#include "QSynthi2/Container/List.h"    // needed by file that include types.h
#include "QSynthi2/Container/Vector.h"
#include "QSynthi2/Eigen.h"

typedef double Decimal;
typedef Vec2<Decimal> V2;

typedef std::complex<Decimal> Complex;

typedef Eigen::ArrayXX<Complex> ComplexMatrix;
typedef Eigen::ArrayXX<Decimal> RealMatrix;

class SimulationFrame {
public:
    virtual ~SimulationFrame() = default;
    [[nodiscard]] virtual SimulationFrame* clone() = 0;
    [[nodiscard]] virtual RealMatrix toDecimal() const = 0;
    [[nodiscard]] virtual RealMatrix toPhase() const = 0;
    [[nodiscard]] virtual Decimal toDecimal(long row, long col) const = 0;
    [[nodiscard]] virtual Decimal toPhase(long row, long col) const = 0;
    [[nodiscard]] virtual size_t cols() const = 0;
    [[nodiscard]] virtual size_t rows() const = 0;
};

typedef std::shared_ptr<SimulationFrame> SimulationFramePointer;
typedef List<SimulationFramePointer> FrameList;

#endif //TYPES_H