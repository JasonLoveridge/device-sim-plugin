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
: AudioProcessorEditor (&p), processor (p),
outputVolumeLabel({}, processor.outputVolumeParam->name)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    addAndMakeVisible(titleLabel);
    titleLabel.setText("Device Simulator", NotificationType::dontSendNotification);
    titleLabel.setJustificationType(Justification::horizontallyCentred);
    
    addAndMakeVisible(phoneButton);
    phoneButton.setRadioGroupId(1);
    phoneButton.addListener(this);
    
    addAndMakeVisible(laptopButton);
    laptopButton.setRadioGroupId(phoneButton.getRadioGroupId());
    laptopButton.addListener(this);
    
    addAndMakeVisible(tvButton);
    tvButton.setRadioGroupId(phoneButton.getRadioGroupId());
    tvButton.addListener(this);
    
    addAndMakeVisible(bluetoothButton);
    bluetoothButton.setRadioGroupId(phoneButton.getRadioGroupId());
    bluetoothButton.addListener(this);
    
    addAndMakeVisible(phoneButtonLabel);
    phoneButtonLabel.setText("Phone", NotificationType::dontSendNotification);
    phoneButtonLabel.setJustificationType(Justification::horizontallyCentred);
    
    addAndMakeVisible(laptopButtonLabel);
    laptopButtonLabel.setText("Laptop", NotificationType::dontSendNotification);
    laptopButtonLabel.setJustificationType(Justification::horizontallyCentred);
    
    addAndMakeVisible(tvButtonLabel);
    tvButtonLabel.setText("Television", NotificationType::dontSendNotification);
    tvButtonLabel.setJustificationType(Justification::horizontallyCentred);
    
    addAndMakeVisible(bluetoothButtonLabel);
    bluetoothButtonLabel.setText("Bluetooth Speaker", NotificationType::dontSendNotification);
    bluetoothButtonLabel.setJustificationType(Justification::horizontallyCentred);
    
    addAndMakeVisible(deviceTypeBox);
    auto currentCategory = processor.categoryParam->getIndex();
    auto i = 1;
    switch (currentCategory) {
        case 0:
            phoneButton.setToggleState(true, NotificationType::dontSendNotification);
            for (auto choice:processor.phoneTypeParam->choices)
                deviceTypeBox.addItem(choice, i++);
            
            deviceTypeBox.setSelectedId(processor.phoneTypeParam->getIndex() + 1);
            break;
        case 1:
            laptopButton.setToggleState(true, NotificationType::dontSendNotification);
            for (auto choice:processor.laptopTypeParam->choices)
                deviceTypeBox.addItem(choice, i++);
            
            deviceTypeBox.setSelectedId(processor.laptopTypeParam->getIndex() + 1);
            break;
        case 2:
            tvButton.setToggleState(true, NotificationType::dontSendNotification);
            for (auto choice:processor.tvTypeParam->choices)
                deviceTypeBox.addItem(choice, i++);
            
            deviceTypeBox.setSelectedId(processor.tvTypeParam->getIndex() + 1);
            break;
        case 3:
            bluetoothButton.setToggleState(true, NotificationType::dontSendNotification);
            for (auto choice:processor.bluetoothTypeParam->choices)
                deviceTypeBox.addItem(choice, i++);
            
            deviceTypeBox.setSelectedId(processor.bluetoothTypeParam->getIndex() + 1);
            break;
        default:
            phoneButton.setToggleState(true, NotificationType::dontSendNotification);
            for (auto choice:processor.phoneTypeParam->choices)
                deviceTypeBox.addItem(choice, i++);
            
            deviceTypeBox.setSelectedId(processor.phoneTypeParam->getIndex() + 1);
            break;
    }
    
    deviceTypeBox.addListener(this);
    deviceTypeBox.setJustificationType(Justification::horizontallyCentred);
    
    
    addAndMakeVisible(deviceTypeLabel);
    deviceTypeLabel.setJustificationType(Justification::centredLeft);
    deviceTypeLabel.attachToComponent(&deviceTypeBox, true);
    
    addAndMakeVisible(otherButton);
    otherButton.setButtonText("Other IR$");
    otherButton.addListener(this);
    
    addAndMakeVisible(otherIRLabel);
    otherIRLabel.setJustificationType(Justification::horizontallyCentred);
    otherIRLabel.setText("No other IR selected", NotificationType::dontSendNotification);
    
    addAndMakeVisible(outputVolumeSlider = new ParameterSlider(*processor.outputVolumeParam));
    
    addAndMakeVisible(outputVolumeLabel);
    outputVolumeLabel.setJustificationType(Justification::centredLeft);
    outputVolumeLabel.attachToComponent(outputVolumeSlider, true);
    
    setSize (500, 350);
}

DeviceSimulationPluginAudioProcessorEditor::~DeviceSimulationPluginAudioProcessorEditor()
{
}

void DeviceSimulationPluginAudioProcessorEditor::comboBoxChanged(ComboBox* box) {
    switch(deviceCategory) {
        case 0:
            processor.phoneTypeParam->operator=(box->getSelectedItemIndex());
            break;
        case 1:
            processor.laptopTypeParam->operator=(box->getSelectedItemIndex());
            break;
        case 2:
            processor.tvTypeParam->operator=(box->getSelectedItemIndex());
            break;
        case 3:
            processor.bluetoothTypeParam->operator=(box->getSelectedItemIndex());
            break;
        default:
            processor.phoneTypeParam->operator=(box->getSelectedItemIndex());
            break;
    }
    if (otherIRLabel.getText().contains("Using:")) {
        otherIRLabel.setText("Using IR from drop down menu", NotificationType::dontSendNotification);
    }
}

void DeviceSimulationPluginAudioProcessorEditor::buttonClicked(Button* button) {
    
    if (button == &otherButton) {
        FileChooser chooser("Select folder containing impulse responses", File::nonexistent, "*.wav");
        if (chooser.browseForDirectory()) {
            File folder (chooser.getResult());
            processor.llIR = folder.getChildFile("LLIR-T.wav");
            processor.lrIR = folder.getChildFile("LRIR-T.wav");
            processor.rlIR = folder.getChildFile("RLIR-T.wav");
            processor.rrIR = folder.getChildFile("RRIR-T.wav");
            processor.fileChanged = true;
            otherIRLabel.setText("Using: " + folder.getFileName(), NotificationType::dontSendNotification);
        }
    }
    else {
        if (button == &phoneButton) {
            deviceCategory = 0;
            auto i = 1;
            deviceTypeBox.clear();
            for (auto choice:processor.phoneTypeParam->choices)
                deviceTypeBox.addItem(choice, i++);
            
            processor.categoryParam->operator=(0);
            deviceTypeBox.setSelectedId(processor.phoneTypeParam->getIndex() + 1);
        }
        else if (button == &laptopButton) {
            deviceCategory = 1;
            auto i = 1;
            deviceTypeBox.clear();
            for (auto choice:processor.laptopTypeParam->choices)
                deviceTypeBox.addItem(choice, i++);
            
            processor.categoryParam->operator=(1);
            deviceTypeBox.setSelectedId(processor.laptopTypeParam->getIndex() + 1);
        }
        else if (button == &tvButton) {
            deviceCategory = 2;
            auto i = 1;
            deviceTypeBox.clear();
            for (auto choice:processor.tvTypeParam->choices)
                deviceTypeBox.addItem(choice, i++);
            
            processor.categoryParam->operator=(2);
            deviceTypeBox.setSelectedId(processor.tvTypeParam->getIndex() + 1);
        }
        else if (button == &bluetoothButton) {
            deviceCategory = 3;
            auto i = 1;
            deviceTypeBox.clear();
            for (auto choice:processor.bluetoothTypeParam->choices)
                deviceTypeBox.addItem(choice, i++);
            
            processor.categoryParam->operator=(3);
            deviceTypeBox.setSelectedId(processor.bluetoothTypeParam->getIndex() + 1);
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
    const int titleHeight = 20;
    titleLabel.setBounds(bounds.removeFromTop(titleHeight));
    
    auto buttonBounds = bounds.removeFromTop(80).reduced(10);
    auto buttonLabelBounds = buttonBounds.removeFromTop(30);
    phoneButtonLabel.setBounds(buttonLabelBounds.removeFromLeft(buttonLabelBounds.getWidth() / 4));
    laptopButtonLabel.setBounds(buttonLabelBounds.removeFromLeft(buttonLabelBounds.getWidth() / 3));
    tvButtonLabel.setBounds(buttonLabelBounds.removeFromLeft(buttonLabelBounds.getWidth() / 2));
    bluetoothButtonLabel.setBounds(buttonLabelBounds);
    
    const int buttonSize = 14;
    const int inset = (buttonBounds.getWidth() / 8) + buttonSize;
    phoneButton.setBounds(buttonBounds.removeFromLeft(buttonBounds.getWidth() / 4));
    phoneButton.setBounds(phoneButton.getBounds().removeFromRight(inset));
    laptopButton.setBounds(buttonBounds.removeFromLeft(buttonBounds.getWidth() / 3));
    laptopButton.setBounds(laptopButton.getBounds().removeFromRight(inset));
    tvButton.setBounds(buttonBounds.removeFromLeft(buttonBounds.getWidth() / 2));
    tvButton.setBounds(tvButton.getBounds().removeFromRight(inset));
    bluetoothButton.setBounds(buttonBounds);
    bluetoothButton.setBounds(bluetoothButton.getBounds().removeFromRight(inset));
    
    auto typeBoxBounds = bounds.removeFromTop(40);
    typeBoxBounds.removeFromLeft(40);
    typeBoxBounds.removeFromRight(40);
    deviceTypeBox.setBounds(typeBoxBounds);
    bounds.removeFromTop(15);
    
    auto otherButtonBounds = bounds.removeFromTop(30);
    otherButtonBounds.removeFromLeft(50);
    otherButtonBounds.removeFromRight(50);
    otherButton.setBounds(otherButtonBounds);
    bounds.removeFromTop(15);
    
    otherIRLabel.setBounds(bounds.removeFromTop(30));
    bounds.removeFromTop(15);
    
    bounds.removeFromLeft(125);
    outputVolumeSlider->setBounds(bounds.removeFromTop(30));
    bounds.removeFromTop(15);
}

