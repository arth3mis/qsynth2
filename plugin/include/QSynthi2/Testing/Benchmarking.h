#pragma once

#include "juce_audio_processors/juce_audio_processors.h"
#include "QSynthi2/List/list.h"




static List<double> benchmarkSamples = List<double>(1024);
static float floatVariable = 3;
static double doubleVariable = 3;

static long total_0 = 0;
static long total_1 = 0;
static long total_2 = 0;

static void markTheBench() {
    for (size_t benchmarkAmount = 80000; benchmarkAmount >= 625; benchmarkAmount /= 2) {
        auto start_time = std::chrono::high_resolution_clock::now();

        std::vector<double> samples = std::vector<double>(benchmarkSamples.size());
        float sum = 0;

        for (size_t j = 0; j < benchmarkAmount; j++) {
            for (size_t i = 0; i < benchmarkSamples.size(); i++) {
                samples[i] = benchmarkSamples[i];


                if (std::rand() > 0.5 * RAND_MAX) samples[i] *= floatVariable;
                else samples[i] -= floatVariable;

                if (std::abs(samples[i]) > 2e16) samples[i] = 1;
                else if (std::abs(samples[i]) < 2e-16) samples[i] = 1;

                sum += samples[i];
            }
        }


        auto end_time = std::chrono::high_resolution_clock::now();
        auto time = (end_time - start_time) / std::chrono::milliseconds(1);
        juce::Logger::writeToLog(
                "iterations: " + juce::String(benchmarkAmount) + "\t Double vector: " + juce::String(time) +
                "ms  \t sum: " + juce::String(sum));
        total_0 += time;






        auto d_start_time = std::chrono::high_resolution_clock::now();

        List<double> d_samples = List<double>(benchmarkSamples.size());


        for (size_t j = 0; j < benchmarkAmount; j++) {
            d_samples = d_samples.map<double>([&](double x) {
                double y = 0;
                if (std::rand() > 0.5 * RAND_MAX) y = x * doubleVariable;
                else y = x - doubleVariable;

                if (std::abs(y) > 2e16) y = 1;
                else if (std::abs(y) < 2e-16) y = 1;
                return y;
            });

        }

        double d_sum = List<double>::sumOf(d_samples);


        auto d_end_time = std::chrono::high_resolution_clock::now();
        auto time_1 = (d_end_time - d_start_time) / std::chrono::milliseconds(1);
        juce::Logger::writeToLog(
                "iterations: " + juce::String(benchmarkAmount) + "\t Double list : " + juce::String(time_1) +
                "ms \t sum: " + juce::String(d_sum));
        total_1 += time_1;






        auto start_time_2 = std::chrono::high_resolution_clock::now();

        List<double> samples_2 = List<double>(benchmarkSamples.size());


        for (size_t j = 0; j < benchmarkAmount; j++) {
            samples_2.mapMutable([&](double x) {
                double y = 0;
                if (std::rand() > 0.5 * RAND_MAX) y = x * doubleVariable;
                else y = x - doubleVariable;

                if (std::abs(y) > 2e16) y = 1;
                else if (std::abs(y) < 2e-16) y = 1;
                return y;
            });

        }

        double sum_2 = List<double>::sumOf(samples_2);


        auto end_time_2 = std::chrono::high_resolution_clock::now();
        auto time_2 = (end_time_2 - start_time_2) / std::chrono::milliseconds(1);
        juce::Logger::writeToLog(
                "iterations: " + juce::String(benchmarkAmount) + "\t Double list mutable : " + juce::String(time_2) +
                "ms \t sum: " + juce::String(sum_2));
        total_2 += time_2;

    }

    juce::Logger::writeToLog("TOTAL:");
    juce::Logger::writeToLog("    Double vector:         " + juce::String((100 * total_0 /std::min(total_0, std::min(total_1, total_2)))) + "%");
    juce::Logger::writeToLog("    Double list immutable: " + juce::String((100 * total_1 /std::min(total_0, std::min(total_1, total_2)))) + "%");
    juce::Logger::writeToLog("    Double list mutable:   " + juce::String((100 * total_2 /std::min(total_0, std::min(total_1, total_2)))) + "%");

}