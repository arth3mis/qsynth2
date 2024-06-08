#ifndef QSYNTHI2_MODULATIONDATA_H
#define QSYNTHI2_MODULATIONDATA_H

#include <utility>

#include "QSynthi2/types.h"

class ModulationData : public List<Eigen::ArrayX<Decimal>> {

public:

    class Source {
    public:

        Source(juce::String newName) : name(std::move(newName)) { }

        const juce::String& getName() const { return name; }
        size_t getId() const { return id; }
        void setId(size_t newId) { id = newId; }

    protected:

        const juce::String name;
        size_t id = static_cast<size_t>(-1);

    };

    class Sources {
    public:
        inline static const Source VELOCITY{"Velocity"};
        inline static const Source PITCH{"Pitch"};
        inline static const Source Y{"Keyboard Y"};
        inline static const Source Z{"Keyboard Z"};

        inline static List<Source> ALL{VELOCITY, PITCH, Y, Z};
    };


    ModulationData() : List<Eigen::ArrayX<Decimal>>() {
        Sources::ALL.forEachIndexed([](size_t i, Source source){
            source.setId(i);
        });
    }

    void prepareToPlay(int samplesPerBlock) {
        this->clear();

        for (size_t i = 0; i < Sources::ALL.size(); i++) {
            this->push_back(Eigen::ArrayX<Decimal>(samplesPerBlock).setZero());
        }
    }

    void write(const Source& source, juce::SmoothedValue<Decimal> smoothedValue) {
        // TODO: check if the mutable thing works
        Eigen::ArrayX<Decimal> data = this->at(source.getId());
        Decimal currentValue = smoothedValue.getCurrentValue();
        Decimal targetValue = smoothedValue.getTargetValue();

        if (juce::approximatelyEqual(currentValue, targetValue) && juce::approximatelyEqual(data[0], targetValue)) {
            return;
        }

        for (int i = 0; i < data.size(); ++i) {
            data[i] = smoothedValue.getNextValue();
        }
    }

    bool isSourceValid (const Source& source) const {
        return source.getId() >= 0 && source.getId() < this->size() && this->at(source.getId()).size() > 0;
    }

};



#endif //QSYNTHI2_MODULATIONDATA_H
