//
// Created by art on 14.05.24.
//

#ifndef TYPES_H
#define TYPES_H

#include "juce_audio_processors/juce_audio_processors.h"
#include "QSynthi2/List/list.h"
#include "QSynthi2/List/Vector.h"
#include "Eigen/Dense"


#define SIM_W 128
#define SIM_H 128

typedef double num;
typedef Vec2<num> V2;

typedef std::complex<num> cnum;

typedef List<cnum> CList;
typedef List<num> RList;

typedef Eigen::Array<cnum, SIM_H, SIM_W> CSimMatrix;
typedef Eigen::Array<num, SIM_H, SIM_W> RSimMatrix;

typedef CSimMatrix SimFrame;


typedef std::unordered_map<juce::String, num> ModulationData;

#endif //TYPES_H