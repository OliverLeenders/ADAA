#pragma once

#include <JuceHeader.h>

namespace HighPass {
    static double b0 = 0.0;
    static double b1 = 0.0;
    static double b2 = 0.0;

    static double a0 = 0.0;
    static double a1 = 0.0;
    static double a2 = 0.0;

    static double x0[2] = {0.0};
    static double x1[2] = {0.0};
    static double x2[2] = {0.0};

    static double y1[2] = {0.0};
    static double y2[2] = {0.0};

    static void init(double sampleRate) {
        double w0 = 2 * juce::MathConstants<double>::pi * 5.0 / sampleRate;
        double alpha = sin(w0) / 2.0;
        b0 = (1 + cos(w0)) * 0.5;
        b1 = -(1 + cos(w0));
        b2 = b0;
        a0 = 1 + alpha;
        a1 = -2 * cos(w0);
        a2 = 1 - alpha;
    }

    static void processBlock(juce::AudioBuffer<float> &buffer) {
        for (int channel = 0; channel < buffer.getNumChannels(); channel++) {
            float *channelData = buffer.getWritePointer(channel);
            for (int i = 0; i < buffer.getNumSamples(); i++) {
                x2[channel] = x1[channel];
                x1[channel] = x0[channel];
                x0[channel] = channelData[i];
                // clang-format off
                channelData[i] = (b0 / a0) * x0[channel]
                                    + (b1 / a0) * x1[channel]
                                    + (b2 / a0) * x2[channel] 
                                    - (a1 / a0) * y1[channel]
                                    - (a2 / a0) * y2[channel];
                // clang-format on
                y2[channel] = y1[channel];
                y1[channel] = channelData[i];
            }
        }
    }
} // namespace HighPass