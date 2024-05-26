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
    z.setTargetValue(static_cast<Decimal>(currentlyPlayingNote.pressure.asUnsignedFloat()));
}



void Voice::prepareToPlay(Decimal sampleRate, int samplesPerBlock) {
    noteStopped(false);
    juce::MPESynthesiserVoice::setCurrentSampleRate(static_cast<double>(sampleRate));

    velocity.reset(sampleRate, 0.005);
    frequency.reset(sampleRate, 0.030);
    y.reset(sampleRate, 0.020);
    z.reset(sampleRate, 0.020);

    Modulation::Sources::ALL.forEach([&](const juce::String& modulationSource){
        modulationData[modulationSource] = Eigen::ArrayX<Decimal>(samplesPerBlock);
    });

    sonifier.prepareToPlay(sampleRate, samplesPerBlock);

}



void Voice::renderNextBlock(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples) {

    for (int i = startSample; i < startSample + numSamples; i++) {
        // TODO: Optimize this, maybe pointers to Arrays?
        modulationData[Modulation::Sources::VELOCITY][i] = velocity.getNextValue();
        modulationData[Modulation::Sources::PITCH][i] = frequency.getNextValue();
        modulationData[Modulation::Sources::Y][i] = y.getNextValue();
        modulationData[Modulation::Sources::Z][i] = z.getNextValue();
    }


    if (gain == 0.0) {
        clearCurrentNote(); // Important
    }
}



Eigen::ArrayX<Decimal> Voice::generateNextBlock() {
    // TODO: replace with envelope
    return gain * sonifier.generateNextBlock(modulationData);
}



