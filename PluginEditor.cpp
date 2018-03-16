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
    
    setSize (400, 300);
}

DeviceSimulationPluginAudioProcessorEditor::~DeviceSimulationPluginAudioProcessorEditor()
{
}

void DeviceSimulationPluginAudioProcessorEditor::comboBoxChanged(ComboBox* box) {
    processor.deviceTypeParam->operator=(box->getSelectedItemIndex());
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
}
