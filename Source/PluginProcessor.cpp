/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ADAAAudioProcessor::ADAAAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
                         ),
#endif
      parameters(*this, nullptr, juce::Identifier("ADAA"),
                 {std::make_unique<juce::AudioParameterFloat>("drive", "Drive", 1.0f, 100.0f, 1.0f),
                  std::make_unique<juce::AudioParameterFloat>("bias", "Bias", 0.0f, 1.0f, 0.0f)}) {

    driveParameter = parameters.getRawParameterValue("drive");
    biasParameter = parameters.getRawParameterValue("bias");

    TanHSpline::RANGE = RANGE;
    TanHLerp::RANGE = RANGE;
    std::vector<double> v(NUM_POINTS);
    std::vector<double> w(NUM_POINTS);
    for (int i = 0; i < NUM_POINTS; i++) {
        v[i] = std::tanh(RANGE * (i / NUM_POINTS * 2.0 - 1.0));
        w[i] = std::log(std::cosh(RANGE * (i / NUM_POINTS * 2.0 - 1.0)));
        TanHLerp::tanh_arr[i] = v[i];
        TanHLerp::log_cosh_arr[i] = w[i];
    }
    Interpolator inter(v);
    Interpolator inter_(w);

    TanHSpline::smooth_tanh = inter;
    TanHSpline::smooth_log_cosh = inter_;
}

ADAAAudioProcessor::~ADAAAudioProcessor() {}

//==============================================================================
const juce::String ADAAAudioProcessor::getName() const { return JucePlugin_Name; }

bool ADAAAudioProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool ADAAAudioProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool ADAAAudioProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double ADAAAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int ADAAAudioProcessor::getNumPrograms() {
    return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
              // so this should be at least 1, even if you're not really implementing programs.
}

int ADAAAudioProcessor::getCurrentProgram() { return 0; }

void ADAAAudioProcessor::setCurrentProgram(int index) {}

const juce::String ADAAAudioProcessor::getProgramName(int index) { return {}; }

void ADAAAudioProcessor::changeProgramName(int index, const juce::String &newName) {}

//==============================================================================
void ADAAAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void ADAAAudioProcessor::releaseResources() {
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ADAAAudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const {
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
        layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

        // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

void ADAAAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                      juce::MidiBuffer &midiMessages) {
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    double drive = *driveParameter;
    double bias = std::exp(*biasParameter / 100.0) - 1.0;

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    if (AA_TYPE == ADAA) {
        if (LUT == LINEAR_LUT) {
            processDriveBlock(buffer, TanHLerp::F, TanHLerp::f, FirstOrderADAA::processBlock, drive,
                              bias);
        } else if (LUT == CUBIC_LUT) {
            processDriveBlock(buffer, TanHSpline::F, TanHSpline::f, FirstOrderADAA::processBlock,
                              drive, bias);
        } else {
            processDriveBlock(buffer, TanH::F, TanH::f, FirstOrderADAA::processBlock, drive, bias);
        }
    } else if (AA_TYPE == NO_AA) {
        if (LUT == LINEAR_LUT) {
            processDriveBlock(buffer, TanHLerp::F, TanHLerp::f, Compute::processBlock, drive, bias);
        } else if (LUT == CUBIC_LUT) {
            processDriveBlock(buffer, TanHSpline::F, TanHSpline::f, Compute::processBlock, drive,
                              bias);
        } else if (LUT == COMPUTE) {
            processDriveBlock(buffer, TanH::F, TanH::f, Compute::processBlock, drive, bias);
        }
    } else if (AA_TYPE == TRANSPARENT_ADAA) {
        if (LUT == COMPUTE) {
            processDriveBlock(buffer, TanH::F, TanH::f, FirstOrderTransparentADAA::processBlock,
                              drive, bias);
        } else if (LUT == LINEAR_LUT) {
            processDriveBlock(buffer, TanHLerp::F, TanHLerp::f,
                              FirstOrderTransparentADAA::processBlock, drive, bias);
        } else if (LUT == CUBIC_LUT) {
            processDriveBlock(buffer, TanHSpline::F, TanHSpline::f,
                              FirstOrderTransparentADAA::processBlock, drive, bias);
        }
    }
}

void ADAAAudioProcessor::processDriveBlock(juce::AudioBuffer<float> &buffer, double (*F)(double),
                                           double (*f)(double),
                                           void (*saturate)(juce::AudioBuffer<float> &,
                                                            double (*)(double), double (*)(double)),
                                           double drive, double bias) {

    Utility::add(&buffer, bias);
    buffer.applyGain(drive * SCALE);

    saturate(buffer, F, f);

    buffer.applyGain(1 / (*driveParameter * SCALE));
    Utility::add(&buffer, -TanH::f(bias * (*driveParameter) * SCALE) / (*driveParameter * SCALE));
}

//==============================================================================
bool ADAAAudioProcessor::hasEditor() const {
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *ADAAAudioProcessor::createEditor() {
    return new ADAAAudioProcessorEditor(*this, parameters);
}

//==============================================================================
void ADAAAudioProcessor::getStateInformation(juce::MemoryBlock &destData) {
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void ADAAAudioProcessor::setStateInformation(const void *data, int sizeInBytes) {
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr) {
        if (xmlState->hasTagName(parameters.state.getType())) {
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() { return new ADAAAudioProcessor(); }
