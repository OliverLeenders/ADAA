#include "FirstOrderTransparentADAA.h"

void FirstOrderTransparentADAA::processBlock(juce::AudioBuffer<float> &buffer, double (*F)(double),
                                             double (*f)(double)) {
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
        auto *channelData = buffer.getWritePointer(channel);

        for (int i = 0; i < buffer.getNumSamples(); i++) {
            double original_sample = channelData[i];
            double first_summand;
            double second_summand;
            if (std::abs(original_sample - prev_sample[channel]) < ILL_CONDITIONED) {
                first_summand = 0.5 * tanh(prev_sample[channel]);
            } else {
                first_summand =
                    (std::log(std::cosh(0.5 * (original_sample + prev_sample[channel]))) -
                     std::log(std::cosh(prev_sample[channel]))) /
                    (original_sample - prev_sample[channel]);
            }

            if (std::abs(prev_sample[channel] - prev_prev_sample[channel]) < ILL_CONDITIONED) {
                second_summand = 0.5 * tanh(prev_sample[channel]);
            } else {
                second_summand = (std::log(std::cosh(prev_sample[channel])) -
                                  std::log(std::cosh(
                                      0.5 * (prev_sample[channel] + prev_prev_sample[channel])))) /
                                 (prev_sample[channel] - prev_prev_sample[channel]);
            }
            channelData[i] = first_summand + second_summand;
            prev_prev_sample[channel] = prev_sample[channel];
            prev_sample[channel] = original_sample;
        }
    }
}

double FirstOrderTransparentADAA::prev_sample[2] = {0.0};
double FirstOrderTransparentADAA::prev_prev_sample[2] = {0.0};
const double FirstOrderTransparentADAA::ILL_CONDITIONED = 0.00001;
