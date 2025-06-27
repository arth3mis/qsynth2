#pragma once

#include "QSynthi2/types.h"
#include "QSynthi2/Parameter/ParameterCollection.h"
#include "QSynthi2/Synthesizer/VoiceData.h"

class Scanner {
public:

    explicit Scanner(std::shared_ptr<VoiceData> voiceData);

    Eigen::ArrayXX<Decimal> getValuesAt(const Eigen::ArrayXX<Decimal> &position0to1, const std::function<Eigen::ArrayXX<Decimal>(const FrameList &frameBuffer,
                                                                                                                           const Eigen::ArrayXX<Decimal> &frameBufferTimestamps,
                                                                                                                           const Eigen::ArrayXX<Decimal> &y,
                                                                                                                           const Eigen::ArrayXX<Decimal> &x)>&, const ModulationData &modulationData);

    Eigen::ArrayXX<Decimal> getValuesLine(const Eigen::ArrayXX<Decimal> &position0to1, const std::function<Eigen::ArrayXX<Decimal>(const FrameList &frameBuffer,
                                                                                                                             const Eigen::ArrayXX<Decimal> &frameBufferTimestamps,
                                                                                                                             const Eigen::ArrayXX<Decimal> &y,
                                                                                                                             const Eigen::ArrayXX<Decimal> &x)>&, const ModulationData &modulationData);

    Eigen::ArrayXX<Decimal> getValuesCircle(const Eigen::ArrayXX<Decimal> &position0to1, const std::function<Eigen::ArrayXX<Decimal>(const FrameList &frameBuffer,
                                                                                                                               const Eigen::ArrayXX<Decimal> &frameBufferTimestamps,
                                                                                                                               const Eigen::ArrayXX<Decimal> &y,
                                                                                                                               const Eigen::ArrayXX<Decimal> &x)>&, const ModulationData &modulationData);

    static Eigen::ArrayXX<Decimal> noInterpolation(const FrameList &frameBuffer,
                                                   const Eigen::ArrayXX<Decimal> &frameBufferTimestamps,
                                                   const Eigen::ArrayXX<Decimal> &y,
                                                   const Eigen::ArrayXX<Decimal> &x);

    static Eigen::ArrayXX<Decimal> linearInterpolation(const FrameList &frameBuffer,
                                                   const Eigen::ArrayXX<Decimal> &frameBufferTimestamps,
                                                   const Eigen::ArrayXX<Decimal> &y,
                                                   const Eigen::ArrayXX<Decimal> &x);

    static Eigen::ArrayXX<Decimal> bicubicInterpolation(const FrameList &frameBuffer,
                                                        const Eigen::ArrayXX<Decimal> &frameBufferTimestamps,
                                                        const Eigen::ArrayXX<Decimal> &y,
                                                        const Eigen::ArrayXX<Decimal> &x);

    inline static Decimal getBicubicInterpolated(const FrameList &frameBuffer, Decimal frameBufferTimestamp, Decimal      y, Decimal x);
    inline static Decimal getBicubicInterpolated(const FrameList &frameBuffer, size_t  frameBufferTimestamp, Decimal      y, Decimal x);
    inline static Decimal getBicubicInterpolated(const FrameList &frameBuffer, size_t  frameBufferTimestamp, Eigen::Index y, Decimal x);

    // Optimized bicubic interpolation functions
    static Decimal getBicubicInterpolatedOptimized(const FrameList &frameBuffer, size_t frameBufferTimestamp, 
                                                   Decimal y, Decimal x, size_t height, size_t width);

    // Catmull-Rom spline interpolation (matches original algorithm)
    inline static Decimal catmullRomInterpolate(Decimal v0, Decimal v1, Decimal v2, Decimal v3, Decimal t) {
        // These coefficients match the original bicubicCoefficient function
        const Decimal c0 = v1;  // case(0): return v1;
        const Decimal c1 = -v0/3.0 - v1/2.0 + v2 - v3/6.0;  // case(1)
        const Decimal c2 = v0/2.0 - v1 + v2/2.0;  // case(2)
        const Decimal c3 = -v0/6.0 + v1/2.0 - v2/2.0 + v3/6.0;  // case(3)
        
        // Apply Catmull-Rom formula: c0 + c1*t + c2*t^2 + c3*t^3
        return c0 + c1*t + c2*t*t + c3*t*t*t;
    }

    // Inline version for maximum performance
    inline static Decimal getBicubicCoefficientOptimizedInline(int dy, int dx, Decimal yFrac, Decimal xFrac, 
                                                              Decimal yFrac2, Decimal yFrac3, Decimal xFrac2, Decimal xFrac3) {
        // Precomputed bicubic coefficients matrix
        static constexpr Decimal coeffs[4][4] = {
            {-0.5,  1.5, -1.5,  0.5},
            { 1.0, -2.5,  2.0, -0.5},
            {-0.5,  0.0,  0.5,  0.0},
            { 0.0,  1.0,  0.0,  0.0}
        };
        
        // Calculate basis functions
        const Decimal yBasis = coeffs[dy][0] + coeffs[dy][1] * yFrac + coeffs[dy][2] * yFrac2 + coeffs[dy][3] * yFrac3;
        const Decimal xBasis = coeffs[dx][0] + coeffs[dx][1] * xFrac + coeffs[dx][2] * xFrac2 + coeffs[dx][3] * xFrac3;
        
        return yBasis * xBasis;
    }

    template <int pointNumber>
    inline static Eigen::Index bicubicIndex(Decimal interpolationIndex, size_t size);

    template <int pointNumber>
    inline static Decimal bicubicCoefficient(Decimal v0, Decimal v1, Decimal v2, Decimal v3);

    void prepareToPlay(Decimal newSampleRate);

    void restart();

private:

    Decimal sampleRate = 0;

    std::shared_ptr<VoiceData> voiceData;

    static Eigen::ArrayXX<Decimal> toDecimal(const Eigen::ArrayXX<Complex> &simulationValues);
};