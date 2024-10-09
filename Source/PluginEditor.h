/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "PluginProcessor.h"
#include <JuceHeader.h>

//==============================================================================
/**
 */
class ADAAAudioProcessorEditor : public juce::AudioProcessorEditor, private juce::Slider::Listener {
  public:
    ADAAAudioProcessorEditor(ADAAAudioProcessor &, juce::AudioProcessorValueTreeState &vts);
    ~ADAAAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics &) override;
    void resized() override;


    juce::ComboBox lookupTableCbox;
    juce::ComboBox AACbox;

  private:
    void sliderValueChanged(juce::Slider *slider) override;
    void comboBoxStateChanged(juce::ComboBox *comboBox);

    juce::AudioProcessorValueTreeState& valueTreeState;

    juce::Slider drive;
    juce::Label driveLabel;
    juce::Slider bias;
    juce::Label biasLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> biasAttachment;
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ADAAAudioProcessor &audioProcessor;
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ADAAAudioProcessorEditor)
};
