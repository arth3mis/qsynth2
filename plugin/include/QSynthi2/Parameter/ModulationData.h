#ifndef QSYNTHI2_MODULATIONDATA_H
#define QSYNTHI2_MODULATIONDATA_H

#include "QSynthi2/types.h"

class ModulationData : public std::unordered_map<juce::String, Eigen::ArrayX<Decimal>> {

public:
    ModulationData() : std::unordered_map<juce::String, Eigen::ArrayX<Decimal>>() {

    }



    Eigen::ArrayX<Decimal> at(const juce::String &key) const {
        return std::unordered_map<juce::String, Eigen::ArrayX<Decimal>>::at(key);
    }



    Eigen::ArrayX<Decimal> at(const juce::String &key, long number) const {
        auto values = std::unordered_map<juce::String, Eigen::ArrayX<Decimal>>::at(key);

        jassert(number <= values.size()); // current implementation only works for numbers <= the original size
        if (number == values.size()) return values;

        auto trimmedValues = Eigen::ArrayX<Decimal>(number);

        for (long i = 0; i < number; i++) {
            auto index = static_cast<long>(round(static_cast<Decimal>(i + 1) / static_cast<Decimal>(number) * static_cast<Decimal>(values.size() - 1)));
            jassert(index >= 0 && index < values.size());
            trimmedValues[i] = values[index];
        }

        return trimmedValues;
    }

};



#endif //QSYNTHI2_MODULATIONDATA_H
