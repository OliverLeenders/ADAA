#pragma once
#include <JuceHeader.h>

namespace Compute {
    static void processBlock(juce::AudioBuffer<float> &buffer, double (*F)(double),
                             double (*f)(double)) {
        for (int channel = 0; channel < buffer.getNumChannels(); channel++) {
            float *channelData = buffer.getWritePointer(channel);
            for (int i = 0; i < buffer.getNumSamples(); i++) {
                channelData[i] = f(channelData[i]);
            }
        }
    }
} // namespace Compute