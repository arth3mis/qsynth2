#ifndef EIGEN_H
#define EIGEN_H

#ifdef __GNUC__
#ifndef __clang__
#pragma GCC system_header
#else
#pragma clang system_header
#endif
#endif

#include "Eigen/Dense"

namespace Eigen {

    static juce::String toString(Eigen::ArrayX<double> array) {
        std::ostringstream oss;
        oss << array;
        return juce::String(oss.str());
    }

    static juce::String toString(Eigen::ArrayXX<double> array) {
        std::ostringstream oss;
        oss << array;
        return juce::String(oss.str());
    }

}


#endif //EIGEN_H
