#pragma once
#include <JuceHeader.h>

class FirstOrderADAA {
  public:
    static void processBlock(juce::AudioSampleBuffer &buffer, double (*F)(double), double (*f)(double));

  private:
    static double prev_sample[2];
    static const double ILL_CONDITIONED;
};