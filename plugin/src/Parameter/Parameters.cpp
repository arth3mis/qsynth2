#include "QSynthi2/Parameter/Parameters.h"


Parameters::Parameters() {

}



void Parameters::connectTo(juce::AudioProcessor &processorToConnectTo) {
    jassert(layoutInCreation);
    layoutInCreation = false;

    treeState = std::make_shared<juce::AudioProcessorValueTreeState>(
            processorToConnectTo,
            nullptr,
            "Parameters",
            std::move(layout));

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
    modulatedParameters.forEach([&](ModulatedParameterFloat* p){ p->prepareToPlay(sampleRate, samplesPerBlock); });
}



void Parameters::processBlock() {
    modulatedParameters.forEach([&](ModulatedParameterFloat* p){ p->processBlock(); });
}
