cmake_minimum_required(VERSION 3.22)

# Version is needed by JUCE.
project(AudioPlugin VERSION 0.1.0)

# Adding a directory with the library/application name as a subfolder of the
# include folder is a good practice. It helps avoid name clashes later on.
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/QSynthi2")

IF (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(PROD_NAME "QSynthi2-Debug")
ELSE()
    set(PROD_NAME "QSynthi2")
ENDIF()

# Adds a plugin target (that's basically what the Projucer does).
juce_add_plugin(${PROJECT_NAME}
        COMPANY_NAME AJ
        IS_SYNTH TRUE
        NEEDS_MIDI_INPUT TRUE
        NEEDS_MIDI_OUTPUT TRUE
        COPY_PLUGIN_AFTER_BUILD FALSE
        PLUGIN_MANUFACTURER_CODE Ajfm
        PLUGIN_CODE Qsy2
        FORMATS AU VST3 Standalone
        PRODUCT_NAME ${PROD_NAME}
)

# Sets the source files of the plugin project.
target_sources(${PROJECT_NAME}
        PRIVATE
        src/PluginEditor.cpp
        src/PluginProcessor.cpp
        src/Data.cpp
        src/Display/SimulationDisplay.cpp
        src/Simulation/Simulation.cpp
        src/Simulation/SimulationThread.cpp
        src/Simulation/QuantumSimulation.cpp
        src/Parameter/Modulation.cpp
        src/Parameter/ModulatedParameterFloat.cpp
        src/Parameter/ModulationData.cpp
        src/Parameter/Parameters.cpp
        src/Synthesizer/ADSR.cpp
        src/Synthesizer/Scanner.cpp
        src/Synthesizer/Voice.cpp
        src/Synthesizer/Sonifier.cpp
        src/AudioProcessing/AJAudioProcessor.cpp
)

# Sets the include directories of the plugin project.
target_include_directories(${PROJECT_NAME}
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Links to all necessary dependencies. The present ones are recommended by JUCE.
# If you use one of the additional modules, like the DSP module, you need to specify it here.
target_link_libraries(${PROJECT_NAME}
        PRIVATE
        juce::juce_audio_utils
        PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
        juce::juce_dsp # @Arthur soll das public oder nicht?
)

# These definitions are recommended by JUCE.
target_compile_definitions(${PROJECT_NAME}
        PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

# Silences some deprecation warnings that (hopefully) will be fixed in a future release of JUCE.
#if (MSVC)
#    target_compile_definitions(${PROJECT_NAME}
#            PRIVATE
#            _SILENCE_CXX23_ALIGNED_STORAGE_DEPRECATION_WARNING)
#endif()

# In Visual Studio this command provides a nice grouping of source files in "filters".
#source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/..)
