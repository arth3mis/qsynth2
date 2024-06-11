#include "QSynthi2/Synthesizer/Voice.h"



Voice::Voice() = default;

void Voice::noteStarted() {
    jassert (currentlyPlayingNote.isValid());
    jassert (currentlyPlayingNote.keyState == juce::MPENote::keyDown
             || currentlyPlayingNote.keyState == juce::MPENote::sustained
             || currentlyPlayingNote.keyState == juce::MPENote::keyDownAndSustained);


    velocity.setTargetValue(static_cast<Decimal>(currentlyPlayingNote.noteOnVelocity.asUnsignedFloat()));

    notePitchbendChanged();
    frequency.setCurrentAndTargetValue(static_cast<Decimal>(currentlyPlayingNote.getFrequencyInHertz())); // TODO: Remove when implementing Portamento
    noteTimbreChanged();
    notePressureChanged();

    gain = 0.25;
    // TODO: Start playing

    sonifier.restart();
}

void Voice::noteStopped(bool allowTailOff) {
    jassert (currentlyPlayingNote.keyState == juce::MPENote::off);

    velocity.setTargetValue(static_cast<Decimal>(currentlyPlayingNote.noteOnVelocity.asUnsignedFloat()));

    gain = 0.0;

    // TODO: Note off behaviour

    if (!allowTailOff) {
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
    juce::Logger::writeToLog("Note Pressure changed: " + juce::String(currentlyPlayingNote.pressure.asUnsignedFloat()));
    z.setTargetValue(static_cast<Decimal>(currentlyPlayingNote.pressure.asUnsignedFloat()));
}



void Voice::prepareToPlay(Decimal sampleRate, int samplesPerBlock) {
    noteStopped(false);
    juce::MPESynthesiserVoice::setCurrentSampleRate(static_cast<double>(sampleRate));

    velocity.reset(sampleRate, 0.005);
    frequency.reset(sampleRate, 0.030);
    y.reset(sampleRate, 0.020);
    z.reset(sampleRate, 0.020);

    modulationData.prepareToPlay(samplesPerBlock);

    sonifier.prepareToPlay(sampleRate, samplesPerBlock);

}



void Voice::renderNextBlock(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples) {
    juce::ignoreUnused(outputBuffer);

    activeThisBlock = true;

    modulationData.write(ModulationData::Sources::VELOCITY, velocity, startSample, numSamples);
    modulationData.write(ModulationData::Sources::PITCH, frequency, startSample, numSamples);
    modulationData.write(ModulationData::Sources::Y, y, startSample, numSamples);
    modulationData.write(ModulationData::Sources::Z, z, startSample, numSamples);


    // TODO: use Envelopes
    if (gain == 0.0) {
        clearCurrentNote(); // Important
    }
}



Eigen::ArrayX<Decimal> Voice::generateNextBlock() {
    // TODO: replace with envelope
    activeThisBlock = false;

    return gain * sonifier.generateNextBlock(modulationData);
}



bool Voice::isActiveThisBlock() {
    return activeThisBlock;
}



