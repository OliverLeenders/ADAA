#pragma once

#include <JuceHeader.h>
class Utility {
  public:
    static void scale(juce::AudioSampleBuffer *buffer, double lambda) {
        for (int channel = 0; channel < buffer->getNumChannels(); channel++) {
            float *channelData = buffer->getWritePointer(channel);
            for (int i = 0; i < buffer->getNumSamples(); i++) {
                channelData[i] *= lambda;
            }
        }
    }

    static void add(juce::AudioSampleBuffer *buffer, double x) {
        for (int channel = 0; channel < buffer->getNumChannels(); channel++) {
            float *channelData = buffer->getWritePointer(channel);
            for (int i = 0; i < buffer->getNumSamples(); i++) {
                channelData[i] += x;
            }
        }
    }
};
