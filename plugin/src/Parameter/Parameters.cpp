#include "QSynthi2/Parameter/Parameters.h"
#include "QSynthi2/Parameter/LambdaListener.h"


Parameters::Parameters() = default;



void Parameters::connectTo(juce::AudioProcessor &processorToConnectTo) {
    jassert(layoutInCreation);
    layoutInCreation = false;

    treeState = std::make_shared<juce::AudioProcessorValueTreeState>(
            processorToConnectTo,
            nullptr,
            "Parameters",
            std::move(layout));

}



List<Modulation> &Parameters::addModulationSlots(int number) {
    jassert(layoutInCreation); // Call this before calling connectTo()
    jassert(number > 0); // number must be greater 0
    jassert(availableModulations.empty()); // Call this method only once, or change implementation to support multiple calls

    const juce::String modulationTargetName = "Modulation Target";
    const juce::String modulationSourceName = "Modulation Source";
    const juce::String modulationAmountName = "Modulation Amount";

    // Get a List of all existing modulatable parameters for possible modulation targets
    List<juce::String> modulationTargetIds = {"None"};
    for (auto& [id, parameter]: modulatedParameters) modulationTargetIds.push_back(id);

    // Add all Parameter amounts as targets as well
    for (int i = 0; i < number; i++) modulationTargetIds.push_back(modulationAmountName + " " + juce::String(i + 1));

    // Create the Parameters for Modulation
    for (int i = 0; i < number; i++) {
        auto* modulationTargetParameter = add<juce::AudioParameterChoice>(juce::ParameterID(modulationTargetName + " " + juce::String(i + 1), Parameters::VERSION), modulationTargetName + " " + juce::String(i + 1), modulationTargetIds.toStringArray(), 0);
        modulationTargetParameter->addListener(new LambdaListener([=](float newValue) {
            juce::Logger::writeToLog(modulationTargetName + " " + juce::String(i + 1) + ": " + modulationTargetIds[static_cast<size_t>(round(newValue * static_cast<float>(modulationTargetIds.size())))]);
        }));
        // TODO: Create Modulation objects
        // TODO: Link parameters to the Modulation with listeners

        auto* modulationSourceParameter = add<juce::AudioParameterChoice>(juce::ParameterID(modulationSourceName + " " + juce::String(i + 1), Parameters::VERSION), modulationSourceName + " " + juce::String(i + 1), Modulation::Sources::ALL.toStringArray(), 0);

        auto* modulationAmountParameter = add<ModulatedParameterFloat>(modulationAmountName + " " + juce::String(i + 1), juce::NormalisableRange<float>(-1, 1, 0, 0.66f, true), 0);
    }

    return availableModulations;
}



void Parameters::getStateInformation(juce::MemoryBlock &destData) {
    juce::MemoryOutputStream memoryStream(destData, true);
    treeState->state.writeToStream(memoryStream);
}



void Parameters::setStateInformation(const void *data, int sizeInBytes) {
    auto inputTree = juce::ValueTree::readFromData(data, static_cast<size_t>(sizeInBytes));
    if (inputTree.isValid()) {
        treeState->replaceState(inputTree);
    }

}



void Parameters::prepareToPlay(Decimal sampleRate, int samplesPerBlock) {
    jassert(!layoutInCreation); // Call connectTo() before any audio processing

    for (auto& [id, parameter]: modulatedParameters) {
        parameter->prepareToPlay(sampleRate, samplesPerBlock);
    }
}



void Parameters::processBlock() {
    jassert(!layoutInCreation); // Call connectTo() before any audio processing

    for (auto& [id, parameter]: modulatedParameters) {
        parameter->processBlock();
    }
}
