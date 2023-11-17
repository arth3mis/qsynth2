#pragma once

#include "juce_audio_processors/juce_audio_processors.h"
#include "ProcessingBlock.h"

class AudioProcessor {
public:

    AudioProcessor() {

    }

    virtual void prepareToPlay() = 0;

    virtual void processBlock(ProcessingBlock& processingBlock) = 0;

protected:
};