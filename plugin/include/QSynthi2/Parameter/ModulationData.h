#ifndef QSYNTHI2_MODULATIONDATA_H
#define QSYNTHI2_MODULATIONDATA_H

#include <utility>

#include "QSynthi2/types.h"

class ModulationData : public List<Eigen::ArrayX<Decimal>> {

public:

    class Source {
    public:

        explicit Source(juce::String newName, size_t newId) : name(std::move(newName)), id(newId) { }

        const juce::String name;
        const size_t id;

    };

    class Sources {
    public:
        inline static const Source VELOCITY{"Velocity", 0};
        inline static const Source PITCH{"Pitch", 1};
        inline static const Source Y{"Keyboard Y", 2};
        inline static const Source Z{"Keyboard Z", 3};
        inline static const Source ENVELOPE1{"Envelope 1 (amp)", 4};

        inline static List<Source> ALL = List<Source>({VELOCITY, PITCH, Y, Z, ENVELOPE1});
    };


    ModulationData() : List<Eigen::ArrayX<Decimal>>() {
    }

    void prepareToPlay(int samplesPerBlock) {
        this->clear();

        for (size_t i = 0; i < Sources::ALL.size(); i++) {
            this->push_back(Eigen::ArrayX<Decimal>(samplesPerBlock).setZero());
        }
    }

    void write(const Source& source, juce::SmoothedValue<Decimal> &smoothedValue, int startSample, int numSamples) {
        jassert(isSourceValid(source)); // ModulationData::Source is not valid

        // TODO: check if the mutable thing works
        Eigen::ArrayX<Decimal> data = this->at(source.id);
        Decimal currentValue = smoothedValue.getCurrentValue();
        Decimal targetValue = smoothedValue.getTargetValue();

        if (juce::approximatelyEqual(currentValue, targetValue) && juce::approximatelyEqual(data(0), targetValue)) {
            return;
        }

        for (int i = startSample; i < startSample + numSamples; i++) {
            data(i) = smoothedValue.getNextValue();
        }

        this->at(source.id) = data;
    }

    [[nodiscard]] bool isSourceValid (const Source& source) const {
        return source.id >= 0 && source.id < this->size() && this->at(source.id).size() > 0;
    }



    Eigen::ArrayX<Decimal> &atSource(const Source &source) {
        return this->at(source.id);
    }

};



#endif //QSYNTHI2_MODULATIONDATA_H
