#include "QSynthi2/Synthesizer/Voice.h"
#include "QSynthi2/Data.h"


extern Data sharedData;


Voice::Voice(const std::shared_ptr<VoiceData>& voiceData) :
voiceData(voiceData),
sonifier(voiceData) {
    sharedData.voiceData.push_back(voiceData);

}

void Voice::noteStarted() {
    jassert (currentlyPlayingNote.isValid());
    jassert (currentlyPlayingNote.keyState == juce::MPENote::keyDown
             || currentlyPlayingNote.keyState == juce::MPENote::sustained
             || currentlyPlayingNote.keyState == juce::MPENote::keyDownAndSustained);


    velocity.setCurrentAndTargetValue(static_cast<Decimal>(currentlyPlayingNote.noteOnVelocity.asUnsignedFloat()));

    notePitchbendChanged();
    frequency.setCurrentAndTargetValue(static_cast<Decimal>(currentlyPlayingNote.getFrequencyInHertz())); // TODO: Remove when implementing Portamento
    y.setCurrentAndTargetValue(static_cast<Decimal>(currentlyPlayingNote.timbre.asUnsignedFloat()));
    z.setCurrentAndTargetValue(static_cast<Decimal>(currentlyPlayingNote.pressure.asUnsignedFloat()));

    envelope1.noteOn();

    sonifier.restart();
}

void Voice::noteStopped(bool allowTailOff) {
    jassert (currentlyPlayingNote.keyState == juce::MPENote::off);

    velocity.setTargetValue(static_cast<Decimal>(currentlyPlayingNote.noteOnVelocity.asUnsignedFloat()));

    envelope1.noteOff();

    if (!allowTailOff) {
        envelope1.reset();
        return;
    }

}



void Voice::notePitchbendChanged() {
    frequency.setTargetValue(static_cast<Decimal>(currentlyPlayingNote.getFrequencyInHertz()));
}



void Voice::noteTimbreChanged() {
    y.setTargetValue(static_cast<Decimal>(currentlyPlayingNote.timbre.asUnsignedFloat()));
}



void Voice::notePressureChanged() {
    z.setTargetValue(static_cast<Decimal>(currentlyPlayingNote.pressure.asUnsignedFloat()));
}



void Voice::prepareToPlay(Decimal sampleRate, int samplesPerBlock) {
    noteStopped(false);
    juce::MPESynthesiserVoice::setCurrentSampleRate(static_cast<double>(sampleRate));

    velocity.reset(sampleRate, 0.005);
    frequency.reset(sampleRate, 0.030);
    y.reset(sampleRate, 0.100);
    z.reset(sampleRate, 0.100);

    modulationData.prepareToPlay(samplesPerBlock);

    envelope1.setParameters(sharedData.parameters->envelope1Attack, sharedData.parameters->envelope1Decay, sharedData.parameters->envelope1Sustain, sharedData.parameters->envelope1Release);
    envelope1.prepareToPlay(sampleRate, samplesPerBlock);

    sonifier.prepareToPlay(sampleRate, samplesPerBlock);

}



void Voice::renderNextBlock(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples) {
    juce::ignoreUnused(outputBuffer);

    activeThisBlock = true;

    modulationData.write(ModulationData::Sources::VELOCITY, velocity, startSample, numSamples);
    modulationData.write(ModulationData::Sources::PITCH, frequency, startSample, numSamples);
    modulationData.write(ModulationData::Sources::Y, y, startSample, numSamples);
    modulationData.write(ModulationData::Sources::Z, z, startSample, numSamples);

    envelope1.processBlock(&modulationData.at(ModulationData::Sources::ENVELOPE1.id), modulationData, startSample, numSamples);

    if (envelope1.getCurrentState() == ADSR::State::OFF) {
        clearCurrentNote();
    }
}



Eigen::ArrayX<Decimal> Voice::generateNextBlock() {

    activeThisBlock = false;

    if (envelope1.getCurrentState() == ADSR::State::OFF) {
        voiceData->gain = 0;
    } else {
        voiceData->gain = modulationData.atSource(ModulationData::Sources::ENVELOPE1)(Eigen::last);
        voiceData->frequency = currentlyPlayingNote.getFrequencyInHertz();
    }

    //juce::Logger::writeToLog(juce::String(modulationData.atSource(ModulationData::Sources::ENVELOPE1)(Eigen::last)) + " -> " + juce::String(envelope1.toGainFactor(modulationData.atSource(ModulationData::Sources::ENVELOPE1))(Eigen::last)));

    return envelope1.toGainFactor(modulationData.atSource(ModulationData::Sources::ENVELOPE1)) * envelope1.toGainFactor(sharedData.parameters->volume->getModulated(modulationData)) * sonifier.generateNextBlock(modulationData);
}



bool Voice::isActiveThisBlock() const {
    return activeThisBlock;
}



