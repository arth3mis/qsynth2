#include "QSynthi2/Synthesizer/ADSR.h"


ADSR::ADSR() {

}

void ADSR::prepareToPlay(Decimal newSampleRate, int samplesPerBlock) {
    jassert(newSampleRate > 0); // Invalid sample rate
    sampleRate = newSampleRate;

    stateValues = List<Eigen::ArrayX<Decimal>>(4, Eigen::ArrayX<Decimal>(samplesPerBlock));
    stateValuesSet = List<bool>(4, false);

    reset();
}

void ADSR::processBlock(Eigen::ArrayX<Decimal> *buffer, const ModulationData &modulationData, int startSample, int numSamples) {
    jassert(sampleRate > 0); // Call prepareToPlay before AudioProcessing
    jassert(stateParameters.size() == 4);

    if (startSample == 0) {
        stateValuesSet = List<bool>(4, false);
    }

    if (state != State::OFF && !stateValuesSet[state]) {
        stateValues[state] = stateParameters[state]->getModulated(modulationData);
        stateValuesSet[state] = true;
    }

    switch (state) {
        case ATTACK:
            for (int i = startSample; i < startSample + numSamples; i++) {
                currentValue += 1 / (sampleRate * stateValues[State::ATTACK](i)); // When attack should not be shorter on re-trigger, replace 1 with stateStart
                buffer->operator()(i) = currentValue;

                if (currentValue >= 1) {
                    currentValue = 1;
                    buffer->operator()(i) = currentValue;
                    state = State::DECAY;
                    stateStart = currentValue;
                    if (i < startSample + numSamples) processBlock(buffer, modulationData, i+1, startSample + numSamples - (i+1));
                    break;
                }
            }
            break;

        case DECAY:
            if (!stateValuesSet[State::SUSTAIN]) {
                stateValues[State::SUSTAIN] = stateParameters[State::SUSTAIN]->getModulated(modulationData);
                stateValuesSet[State::SUSTAIN] = true;
            }

            for (int i = startSample; i < startSample + numSamples; i++) {
                currentValue += (stateValues[State::SUSTAIN](i)-stateStart) / (sampleRate * stateValues[State::DECAY](i));
                buffer->operator()(i) = currentValue;

                if (currentValue <= stateValues[State::SUSTAIN](i)) {
                    currentValue = stateValues[State::SUSTAIN](i);
                    state = State::SUSTAIN;
                    stateStart = currentValue;
                    if (i < startSample + numSamples) processBlock(buffer, modulationData, i+1, startSample + numSamples - (i+1));
                    break;
                }
            }

            break;

        case SUSTAIN:
            for (int i = startSample; i < startSample + numSamples; i++) {
                currentValue = stateValues[State::SUSTAIN](i);
                buffer->operator()(i) = currentValue;
            }
            break;

        case RELEASE:
            for (int i = startSample; i < startSample + numSamples; i++) {
                currentValue -= stateStart / (sampleRate * stateValues[State::RELEASE](i));
                buffer->operator()(i) = currentValue;

                if (currentValue <= 0) {
                    reset();
                    buffer->operator()(i) = currentValue;
                    if (i < startSample + numSamples) processBlock(buffer, modulationData, i+1, startSample + numSamples - (i+1));
                    break;
                }
            }
            break;

        case OFF:
            currentValue = 0;
            buffer->operator()(Eigen::seqN(startSample, numSamples)).setZero();
            break;
    }
}


void ADSR::reset() {
    state = State::OFF;
    currentValue = 0;
}

void ADSR::noteOn() {
    state = State::ATTACK;
    stateStart = currentValue;
}

void ADSR::noteOff() {
    state = State::RELEASE;
    stateStart = currentValue;
}


// See: https://www.desmos.com/calculator/wmf37crmss
Eigen::ArrayX<Decimal> ADSR::toGainFactor(const Eigen::ArrayX<Decimal>& buffer) {
    return (1 + LOWEST_VOLUME_GAIN) * Eigen::pow(10.0, std::log10(LOWEST_VOLUME_GAIN / (1 + LOWEST_VOLUME_GAIN)) * (1 - buffer)) - LOWEST_VOLUME_GAIN;
}


ADSR::State ADSR::getCurrentState() const {
    return state;
}

void ADSR::setParameters(ModulatedParameterFloat *attackTimeParameter,
                         ModulatedParameterFloat *decayTimeParameter,
                         ModulatedParameterFloat *sustainLevelParameter,
                         ModulatedParameterFloat *releaseTimeParameter) {

    stateParameters = List<ModulatedParameterFloat*>({attackTimeParameter, decayTimeParameter, sustainLevelParameter, releaseTimeParameter});
}
