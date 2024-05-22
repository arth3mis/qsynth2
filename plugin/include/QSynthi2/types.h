#ifndef TYPES_H
#define TYPES_H

// suppress warnings
#pragma GCC diagnostic ignored "-Wshadow"


#include "QSynthi2/Juce.h"
#include "QSynthi2/Container/List.h"    // needed by file that include types.h
#include "QSynthi2/Container/Vector.h"
#include "QSynthi2/Eigen.h"


typedef double Decimal;
typedef Vec2<Decimal> V2;

typedef std::complex<Decimal> Complex;

typedef Eigen::Array<Complex, Eigen::Dynamic, Eigen::Dynamic> CSimMatrix;
typedef Eigen::Array<Decimal, Eigen::Dynamic, Eigen::Dynamic> RSimMatrix;

typedef CSimMatrix SimFrame;


typedef std::unordered_map<juce::String, Decimal> ModulationData;

#endif //TYPES_H