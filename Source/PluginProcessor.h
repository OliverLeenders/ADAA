/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once
#include "Compute.h"
#include "FirstOrderADAA.h"
#include "FirstOrderTransparentADAA.h"
#include "HighPass.h"
#include "Interpolator.h"
#include "TanHLerp.h"
#include "TanHSpline.h"
#include "Tanh.h"
#include "Util.h"
#include <JuceHeader.h>

//==============================================================================
/**
 */
class ADAAAudioProcessor : public juce::AudioProcessor {
  public:
    //==============================================================================
    ADAAAudioProcessor();
    ~ADAAAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout &layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;

    //==============================================================================
    juce::AudioProcessorEditor *createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String &newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock &destData) override;
    void setStateInformation(const void *data, int sizeInBytes) override;

    //==============================================================================
    // USER DEFINED SPACE:
    //==============================================================================

    static enum { COMPUTE = 1, LINEAR_LUT, CUBIC_LUT };
    static enum { NO_AA = 1, ADAA, TRANSPARENT_ADAA };
    int LUT = COMPUTE;
    int AA_TYPE = NO_AA;

  private:
    const double SCALE = 0.1;
    const double RANGE = 10.0;
    const double NUM_POINTS = 2048;
    const double ILL_CONDITIONED = 0.00000001;

    juce::AudioProcessorValueTreeState parameters;
    std::atomic<float> *driveParameter = nullptr;
    std::atomic<float> *biasParameter = nullptr;

    std::vector<double> tanh_v;
    double tanh_arr[2048] = {0.0};
    double log_cosh_arr[2048] = {0.0};

    double prev_sample[2] = {0.0};
    double prev_prev_sample[2] = {0.0};

    void processDriveBlock(juce::AudioBuffer<float> &buffer, double (*f)(double),
                           double (*F)(double),
                           void (*saturate)(juce::AudioBuffer<float> &, double (*)(double),
                                            double (*)(double)),
                           double drive, double bias);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ADAAAudioProcessor)
};
