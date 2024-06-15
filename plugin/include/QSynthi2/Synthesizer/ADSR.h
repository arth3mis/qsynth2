#pragma once
#include "QSynthi2/types.h"
#include "QSynthi2/Parameter/ModulatedParameterFloat.h"


class ADSR {
public:

    enum State {
        ATTACK, DECAY, SUSTAIN, RELEASE, OFF
    };

    ADSR();

    void setParameters(ModulatedParameterFloat *attackTimeParameter,
                       ModulatedParameterFloat *decayTimeParameter,
                       ModulatedParameterFloat *sustainLevelParameter,
                       ModulatedParameterFloat *releaseTimeParameter);

    void prepareToPlay(Decimal newSampleRate, int samplesPerBlock);

    void processBlock(Eigen::ArrayX<Decimal> *buffer, const ModulationData &modulationData, int startSample, int numSamples);

    void reset();

    void noteOn();

    void noteOff();

    [[nodiscard]] State getCurrentState() const;

    Eigen::ArrayX<Decimal> toGainFactor(const Eigen::ArrayX<Decimal>& buffer);


protected:

    State state = State::OFF;
    Decimal currentValue = 0;
    Decimal stateStart = 0;

    const Decimal LOWEST_VOLUME_DB = -50;
    const Decimal LOWEST_VOLUME_GAIN = std::pow(10.0, LOWEST_VOLUME_DB / 20.0);

    List<ModulatedParameterFloat*> stateParameters {};
    List<Eigen::ArrayX<Decimal>> stateValues = List<Eigen::ArrayX<Decimal>>(4);
    List<bool> stateValuesSet = List<bool>(4, false);

    Decimal sampleRate = -1;

};
