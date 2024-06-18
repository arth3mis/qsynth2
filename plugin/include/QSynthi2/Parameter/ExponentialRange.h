#ifndef QSYNTHI2_EXPONENTIALRANGE_H
#define QSYNTHI2_EXPONENTIALRANGE_H


class ExponentialRange : public juce::NormalisableRange<float> {

public:
    ExponentialRange(float rangeStart, float rangeEnd) : juce::NormalisableRange<float>(
            rangeStart, rangeEnd,
            [](float rangeStart, float rangeEnd, float valueToRemap){
                return rangeStart * pow(rangeEnd / rangeStart, valueToRemap);
            },
            [](float rangeStart, float rangeEnd, float valueToRemap){
                return log(valueToRemap / rangeStart) / log(rangeEnd / rangeStart);
            },
            [](float rangeStart, float rangeEnd, float valueToRemap){
                return std::max(rangeStart, std::min(rangeEnd, valueToRemap));
            }) {
        jassert(rangeStart > 0 && rangeEnd > 0);
    }
};


#endif //QSYNTHI2_EXPONENTIALRANGE_H
