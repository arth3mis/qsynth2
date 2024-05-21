//
// Created by art on 14.05.24.
//

#ifndef TYPES_H
#define TYPES_H

#include "QSynthi2/Juce.h"
#include "QSynthi2/List/List.h"
#include "QSynthi2/List/Vector.h"
#include "QSynthi2/Eigen.h"

#define SIM_W 128
#define SIM_H 128

typedef double num;
typedef Vec2<num> V2;

typedef std::complex<num> cnum;

typedef List<cnum> CList;
typedef List<num> RList;

typedef Eigen::Array<cnum, Eigen::Dynamic, Eigen::Dynamic> CSimMatrix;
typedef Eigen::Array<num, Eigen::Dynamic, Eigen::Dynamic> RSimMatrix;

typedef CSimMatrix SimFrame;


typedef std::unordered_map<juce::String, num> ModulationData;

#endif //TYPES_H