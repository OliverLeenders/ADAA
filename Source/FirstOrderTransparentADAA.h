#pragma once

#include <JuceHeader.h>
class FirstOrderTransparentADAA {
    public:
    static void processBlock(juce::AudioBuffer<float> &buffer, double(*F)(double), double(*f)(double));
    private:
    static double prev_sample[2];
      static double prev_prev_sample[2];
      static const double ILL_CONDITIONED;
};