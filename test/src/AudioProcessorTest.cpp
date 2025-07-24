#include <gtest/gtest.h>
#include <QSynthi2/PluginProcessor.h>
#include "QSynthi2/AudioProcessing/AJAudioProcessor.h"
#include <malloc.h>

#include "QSynthi2/Data.h"

extern Data sharedData;

namespace audio_plugin_test {
    TEST(AudioProcessor, Synth) {
        juce::MPEInstrument instrument { juce::MPEZone (juce::MPEZone::Type::lower, 15) };
        Synthesiser synth { instrument };

        synth.prepareToPlay(44100, 512);

        size_t before = mallinfo().uordblks;
        synth.generateNextBlock();
        size_t after = mallinfo().uordblks;
        std::cout << "Allocated: " << (after - before) << " bytes\n";

        // AJAudioProcessor processor{};
    }

    TEST(AudioProcessor, Listener) {
        AudioPluginAudioProcessor processor{};
        sharedData.parameters->lineOfInterestShape->setValueNotifyingHost(1.0f);
    }
} // namespace audio_plugin_test