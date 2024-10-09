#include "FirstOrderADAA.h"

double FirstOrderADAA::prev_sample[2] = {0.0};
const double FirstOrderADAA::ILL_CONDITIONED = 0.00001;

void FirstOrderADAA::processBlock(juce::AudioSampleBuffer &buffer, double (*F)(double),
                                  double (*f)(double)) {
    for (int channel = 0; channel < buffer.getNumChannels(); channel++) {
        float *channelData = buffer.getWritePointer(channel);
        for (int i = 0; i < buffer.getNumSamples(); i++) {
            double x = channelData[i];
            double x1 = prev_sample[channel];

            if (std::abs(x - x1) < ILL_CONDITIONED) {
                channelData[i] = f(0.5 * (x + x1));
            } else {
                channelData[i] = (F(x) - F(x1)) / (x - x1);
            }

            prev_sample[channel] = x;
        }
    }
}