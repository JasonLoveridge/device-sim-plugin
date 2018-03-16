/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
DeviceSimulationPluginAudioProcessorEditor::DeviceSimulationPluginAudioProcessorEditor (DeviceSimulationPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    addAndMakeVisible(deviceTypeBox);
    auto i = 1;
    for (auto choice:processor.deviceTypeParam->choices)
        deviceTypeBox.addItem(choice, i++);
    
    deviceTypeBox.setSelectedId(processor.deviceTypeParam->getIndex() + 1);
    
    deviceTypeBox.addListener(this);
    deviceTypeBox.setJustificationType(Justification::horizontallyCentred);
    
    addAndMakeVisible(deviceTypeLabel);
    deviceTypeLabel.setJustificationType(Justification::centredLeft);
    deviceTypeLabel.attachToComponent(&deviceTypeBox, true);
    
    addAndMakeVisible(outputVolumeSlider = new ParameterSlider(*processor.outputVolumeParam));
    
    addAndMakeVisible(outputVolumeLabel);
    outputVolumeLabel.setJustificationType(Justification::centredLeft);
    outputVolumeLabel.attachToComponent(outputVolumeSlider, true);
    
    addAndMakeVisible(otherButton);
    otherButton.setButtonText("Other IR");
    otherButton.addListener(this);
    
    addAndMakeVisible(otherIRLabel);
    otherIRLabel.setJustificationType(Justification::horizontallyCentred);
    otherIRLabel.setText("No other IR selected", NotificationType::dontSendNotification);
    
    setSize (400, 300);
}

DeviceSimulationPluginAudioProcessorEditor::~DeviceSimulationPluginAudioProcessorEditor()
{
}

void DeviceSimulationPluginAudioProcessorEditor::comboBoxChanged(ComboBox* box) {
    processor.deviceTypeParam->operator=(box->getSelectedItemIndex());
    
    if (otherIRLabel.getText().contains("Using:")) {
        otherIRLabel.setText("Using IR from drop down menu", NotificationType::dontSendNotification);
    }
}

void DeviceSimulationPluginAudioProcessorEditor::buttonClicked(Button* button) {
    
    if (button == &otherButton) {
        FileChooser chooser("Select folder containing impulse responses", File::nonexistent, "*.wav");
        if (chooser.browseForFileToOpen()) {
            File file (chooser.getResult());
            processor.otherIRFile = file;
            processor.fileChanged = true;
            otherIRLabel.setText("Using: " + file.getFileName(), NotificationType::dontSendNotification);
        }
    }
}

//==============================================================================
void DeviceSimulationPluginAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
}

void DeviceSimulationPluginAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto bounds = getLocalBounds().reduced(10);
    
    deviceTypeBox.setBounds(bounds.removeFromTop(30));
    bounds.removeFromTop(15);
    
    outputVolumeSlider->setBounds(bounds.removeFromTop(30));
    bounds.removeFromTop(15);
    
    otherButton.setBounds(bounds.removeFromTop(30));
    bounds.removeFromTop(15);
    
    otherIRLabel.setBounds(bounds.removeFromTop(30));
    bounds.removeFromTop(15);
}
