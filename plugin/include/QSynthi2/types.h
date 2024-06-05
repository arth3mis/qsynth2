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

typedef Eigen::Array<Complex, Eigen::Dynamic, Eigen::Dynamic> ComplexMatrix;
typedef Eigen::Array<Decimal, Eigen::Dynamic, Eigen::Dynamic> RealMatrix;

typedef ComplexMatrix SimulationFrame;
typedef List<std::shared_ptr<SimulationFrame>> FrameList;

typedef std::unordered_map<juce::String, Eigen::ArrayX<Decimal>> ModulationData;

#endif //TYPES_H