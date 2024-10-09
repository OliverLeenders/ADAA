/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginEditor.h"
#include "PluginProcessor.h"

//==============================================================================
ADAAAudioProcessorEditor::ADAAAudioProcessorEditor(ADAAAudioProcessor &p,
                                                   juce::AudioProcessorValueTreeState &vts)
    : AudioProcessorEditor(&p), audioProcessor(p), valueTreeState(vts) {
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(400, 300);

    drive.setSliderStyle(juce::Slider::LinearVertical);
    drive.setRange(1.0, 100.0, 0.1);
    drive.setTitle("Drive");
    drive.setPopupDisplayEnabled(true, false, this);
    drive.setTextValueSuffix("% Drive");
    drive.setValue(1.0);

    //// this function adds the slider to the editor
    addAndMakeVisible(&drive);
    driveAttachment.reset(
        new juce::AudioProcessorValueTreeState::SliderAttachment(valueTreeState, "drive", drive));

    addAndMakeVisible(&driveLabel);
    driveLabel.setText("Drive", juce::dontSendNotification);
    driveLabel.setJustificationType(juce::Justification::centred);

    bias.setSliderStyle(juce::Slider::LinearVertical);
    bias.setRange(0.0, 1.0, 0.001);
    bias.setTitle("Bias");
    bias.setTextValueSuffix(" Bias");
    bias.setPopupDisplayEnabled(true, false, this);
    bias.setValue(0.0);

    addAndMakeVisible(&bias);
    biasAttachment.reset(
        new juce::AudioProcessorValueTreeState::SliderAttachment(valueTreeState, "bias", bias));

    addAndMakeVisible(&biasLabel);
    biasLabel.setText("Bias", juce::dontSendNotification);
    biasLabel.setJustificationType(juce::Justification::centred);
    
    addAndMakeVisible(&lookupTableCbox);
    lookupTableCbox.addItem("compute", ADAAAudioProcessor::COMPUTE);
    lookupTableCbox.addItem("linear LUT", ADAAAudioProcessor::LINEAR_LUT);
    lookupTableCbox.addItem("cubic LUT", ADAAAudioProcessor::CUBIC_LUT);

    lookupTableCbox.onChange = [this] { comboBoxStateChanged(&lookupTableCbox); };
    lookupTableCbox.setSelectedId(ADAAAudioProcessor::COMPUTE);

    addAndMakeVisible(&AACbox);
    AACbox.addItem("No AntiAliasing", ADAAAudioProcessor::NO_AA);
    AACbox.addItem("ADAA x1", ADAAAudioProcessor::ADAA);
    AACbox.addItem("transparent ADAA x1", ADAAAudioProcessor::TRANSPARENT_ADAA);

    AACbox.onChange = [this] { comboBoxStateChanged(&AACbox); };
    AACbox.setSelectedId(ADAAAudioProcessor::NO_AA);
}

ADAAAudioProcessorEditor::~ADAAAudioProcessorEditor() {}

//==============================================================================
void ADAAAudioProcessorEditor::paint(juce::Graphics &g) {
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(15.0f));
}

void ADAAAudioProcessorEditor::resized() {
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    drive.setBounds(40, 30, 20, getHeight() - 60);
    driveLabel.setBounds(20, getHeight() - 30, 60, 30);
    bias.setBounds(80, 30, 20, getHeight() - 60);
    biasLabel.setBounds(60, getHeight() - 30, 60, 30);
    AACbox.setBounds(160, 30, 150, 20);
    lookupTableCbox.setBounds(160, 60, 150, 20);

}

void ADAAAudioProcessorEditor::sliderValueChanged(juce::Slider *slider) {
    // audioProcessor.drive->setValueNotifyingHost(this->drive.getValue());
}

void ADAAAudioProcessorEditor::comboBoxStateChanged(juce::ComboBox *comboBox) {
    audioProcessor.LUT = lookupTableCbox.getSelectedId();
    audioProcessor.AA_TYPE = AACbox.getSelectedId();
}