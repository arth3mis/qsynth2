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
#include <unsupported/Eigen/CXX11/Tensor>

namespace Eigen {


    [[nodiscard]] static juce::String toString(const Eigen::ArrayXX<double>& array) {
        std::ostringstream oss;
        oss << array;
        return juce::String(oss.str());
    }

    [[nodiscard]] static juce::String shapeAsString(const Eigen::ArrayXX<double>& array, const juce::String &name = "") {
        return "Eigen::Array " + name + "(" + juce::String(array.cols()) + ", " + juce::String(array.rows()) + ")";
    }

}


#endif //EIGEN_H
