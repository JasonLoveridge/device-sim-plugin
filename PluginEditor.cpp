/*
 ==============================================================================
 
 Device Simulation Plugin - PluginEditor.cpp
 Author: Jason Loveridge
 Date: 05/2018
 BBC Research & Development
 
 Built upon JUCE plugin framework
 
 ==============================================================================
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "DeviceEnumValues.h"


//==============================================================================
DeviceSimulationPluginAudioProcessorEditor::DeviceSimulationPluginAudioProcessorEditor (DeviceSimulationPluginAudioProcessor& p)
: AudioProcessorEditor (&p), processor (p),
outputVolumeLabel({}, processor.outputVolumeParam->name)
{
    //Add and initialise labels and radio button components
    addAndMakeVisible(titleLabel);
    titleLabel.setText("Device Simulator", NotificationType::dontSendNotification);
    titleLabel.setJustificationType(Justification::horizontallyCentred);
    titleLabel.setFont(25.0f);
    
    addAndMakeVisible(phoneButton);
    phoneButton.setRadioGroupId(1);
    phoneButton.addListener(this);
    
    addAndMakeVisible(laptopButton);
    laptopButton.setRadioGroupId(phoneButton.getRadioGroupId());
    laptopButton.addListener(this);
    
    addAndMakeVisible(tvButton);
    tvButton.setRadioGroupId(phoneButton.getRadioGroupId());
    tvButton.addListener(this);
    
    addAndMakeVisible(speakerButton);
    speakerButton.setRadioGroupId(phoneButton.getRadioGroupId());
    speakerButton.addListener(this);
    
    addAndMakeVisible(phoneButtonLabel);
    phoneButtonLabel.setText("Phone", NotificationType::dontSendNotification);
    phoneButtonLabel.setJustificationType(Justification::horizontallyCentred);
    
    addAndMakeVisible(laptopButtonLabel);
    laptopButtonLabel.setText("Laptop", NotificationType::dontSendNotification);
    laptopButtonLabel.setJustificationType(Justification::horizontallyCentred);
    
    addAndMakeVisible(tvButtonLabel);
    tvButtonLabel.setText("Television", NotificationType::dontSendNotification);
    tvButtonLabel.setJustificationType(Justification::horizontallyCentred);
    
    addAndMakeVisible(speakerButtonLabel);
    speakerButtonLabel.setText("Other Speaker", NotificationType::dontSendNotification);
    speakerButtonLabel.setJustificationType(Justification::horizontallyCentred);
    
    // Fill dropdown menu with device choices from relevant category
    addAndMakeVisible(deviceTypeBox);
    fillDeviceTypeBox(processor.categoryParam->getIndex());
    switch(processor.categoryParam->getIndex()) {
        case PHONE:
            phoneButton.setToggleState(true, NotificationType::dontSendNotification);
            break;
        case LAPTOP:
            laptopButton.setToggleState(true, NotificationType::dontSendNotification);
            break;
        case TELEVISION:
            tvButton.setToggleState(true, NotificationType::dontSendNotification);
            break;
        case SPEAKER:
            speakerButton.setToggleState(true, NotificationType::dontSendNotification);
            break;
        default:
            phoneButton.setToggleState(true, NotificationType::dontSendNotification);
            break;
    }
    
    // Add drop down menu, label and also output volume control and label
    deviceTypeBox.addListener(this);
    deviceTypeBox.setJustificationType(Justification::horizontallyCentred);
    
    addAndMakeVisible(deviceTypeLabel);
    deviceTypeLabel.setJustificationType(Justification::centredLeft);
    deviceTypeLabel.attachToComponent(&deviceTypeBox, true);
    
    addAndMakeVisible(outputVolumeSlider = new ParameterSlider(*processor.outputVolumeParam));
    
    addAndMakeVisible(outputVolumeLabel);
    outputVolumeLabel.setJustificationType(Justification::centredLeft);
    outputVolumeLabel.attachToComponent(outputVolumeSlider, true);
    
    // Initialise device image to iPhone
    deviceImage = ImageCache::getFromMemory(Images::iPhoneCropped_png, Images::iPhoneCropped_pngSize);
    
    setSize (500, 450);
}

DeviceSimulationPluginAudioProcessorEditor::~DeviceSimulationPluginAudioProcessorEditor()
{
}

// If the combo box is changed, change to the corresponding device in the processor
void DeviceSimulationPluginAudioProcessorEditor::comboBoxChanged(ComboBox* box) {
    switch(processor.categoryParam->getIndex()) {
        case PHONE:
            processor.phoneTypeParam->operator=(box->getSelectedItemIndex());
            break;
        case LAPTOP:
            processor.laptopTypeParam->operator=(box->getSelectedItemIndex());
            break;
        case TELEVISION:
            processor.tvTypeParam->operator=(box->getSelectedItemIndex());
            break;
        case SPEAKER:
            processor.speakerTypeParam->operator=(box->getSelectedItemIndex());            
            break;
        default:
            processor.phoneTypeParam->operator=(box->getSelectedItemIndex());
            break;
    }

    // Repaint GUI so that device image is updated
    DeviceSimulationPluginAudioProcessorEditor::repaint();
}

void DeviceSimulationPluginAudioProcessorEditor::buttonClicked(Button* button) {
    if (button == &phoneButton) {
        fillDeviceTypeBox(PHONE);
    }
    else if (button == &laptopButton) {
        fillDeviceTypeBox(LAPTOP);
    }
    else if (button == &tvButton) {
        fillDeviceTypeBox(TELEVISION);
    }
    else if (button == &speakerButton) {
        fillDeviceTypeBox(SPEAKER);
    }
    DeviceSimulationPluginAudioProcessorEditor::repaint();
}

// Change the combo box options according to the category that has been selected
void DeviceSimulationPluginAudioProcessorEditor::fillDeviceTypeBox(int category) {
    deviceTypeBox.clear();
    auto i = 1;
    switch (category) {
        case PHONE:
            for (auto choice:processor.phoneTypeParam->choices) {
                deviceTypeBox.addItem(choice, i++);
            }
            
            deviceTypeBox.setSelectedId(processor.phoneTypeParam->getIndex() + 1);
            break;
        case LAPTOP:
            for (auto choice:processor.laptopTypeParam->choices) {
                deviceTypeBox.addItem(choice, i++);
            }
            
            deviceTypeBox.setSelectedId(processor.laptopTypeParam->getIndex() + 1);
            break;
        case TELEVISION:
            for (auto choice:processor.tvTypeParam->choices) {
                deviceTypeBox.addItem(choice, i++);
            }
            
            deviceTypeBox.setSelectedId(processor.tvTypeParam->getIndex() + 1);
            break;
        case SPEAKER:
            for (auto choice:processor.speakerTypeParam->choices) {
                deviceTypeBox.addItem(choice, i++);
            }
            
            deviceTypeBox.setSelectedId(processor.speakerTypeParam->getIndex() + 1);
            break;
        default:
            for (auto choice:processor.phoneTypeParam->choices) {
                deviceTypeBox.addItem(choice, i++);
            }
            
            deviceTypeBox.setSelectedId(processor.phoneTypeParam->getIndex() + 1);
            break;
    }
    processor.categoryParam->operator=(category);
}

//==============================================================================
void DeviceSimulationPluginAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    switch(processor.categoryParam->getIndex()) {
        case PHONE:
            //https://mockuphone.com/iphone7plusgold
            deviceImage = ImageCache::getFromMemory(Images::iPhoneCropped_png, Images::iPhoneCropped_pngSize);
            g.drawImage(deviceImage, 125, 120, 250, 250, 0, 0, 750, 750);
            break;
        case LAPTOP:
            //https://www.ifixit.com/Device/MacBook_Pro_13%22_Retina_Display_Early_2013
            deviceImage = ImageCache::getFromMemory(Images::macBookCropped2_png, Images::macBookCropped2_pngSize);
            g.drawImage(deviceImage, 140, 175, 200, 200, 0, 0, 561, 562);
            break;
        case TELEVISION:
            //https://www.cnet.com/uk/products/panasonic-tx-l47e5b/review/
            deviceImage = ImageCache::getFromMemory(Images::tvcropped_png, Images::tvcropped_pngSize);
            g.drawImage(deviceImage, 135, 180, 239, 156, 0, 0, 716, 468);
            break;
        case SPEAKER:
            if (processor.speakerTypeParam->getIndex() == SONY_SRSX11) {
                //https://www.davistv.co.uk/sony-srsx11-portable-speaker-loudspeaker-black-3901-p.asp
                deviceImage = ImageCache::getFromMemory(Images::cubeCropped_png, Images::cubeCropped_pngSize);
                g.drawImage(deviceImage, 180, 200, 150, 152, 0, 0, 341, 348);
            }
            else if (processor.speakerTypeParam->getIndex() == GENELEC_6010_PAIR) {
                //https://www.genelec.com/support-technology/previous-models/6010a-studio-monitor
                deviceImage = ImageCache::getFromMemory(Images::genelec6010_png, Images::genelec6010_pngSize);
                g.drawImage(deviceImage, 155, 200, 200, 140, 0, 0, 800, 560);
            }
            break;
        default:
            deviceImage = ImageCache::getFromMemory(Images::iPhoneCropped_png, Images::iPhoneCropped_pngSize);
            g.drawImage(deviceImage, 125, 120, 250, 250, 0, 0, 750, 750);
            break;
    }
    
}

void DeviceSimulationPluginAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds().reduced(10);
    const int titleHeight = 30;
    titleLabel.setBounds(bounds.removeFromTop(titleHeight));
    
    auto buttonBounds = bounds.removeFromTop(80).reduced(10);
    auto buttonLabelBounds = buttonBounds.removeFromTop(30);
    phoneButtonLabel.setBounds(buttonLabelBounds.removeFromLeft(buttonLabelBounds.getWidth() / 4));
    laptopButtonLabel.setBounds(buttonLabelBounds.removeFromLeft(buttonLabelBounds.getWidth() / 3));
    tvButtonLabel.setBounds(buttonLabelBounds.removeFromLeft(buttonLabelBounds.getWidth() / 2));
    speakerButtonLabel.setBounds(buttonLabelBounds);
    
    const int buttonSize = 14;
    const int inset = (buttonBounds.getWidth() / 8) + buttonSize;
    phoneButton.setBounds(buttonBounds.removeFromLeft(buttonBounds.getWidth() / 4));
    phoneButton.setBounds(phoneButton.getBounds().removeFromRight(inset));
    laptopButton.setBounds(buttonBounds.removeFromLeft(buttonBounds.getWidth() / 3));
    laptopButton.setBounds(laptopButton.getBounds().removeFromRight(inset));
    tvButton.setBounds(buttonBounds.removeFromLeft(buttonBounds.getWidth() / 2));
    tvButton.setBounds(tvButton.getBounds().removeFromRight(inset));
    speakerButton.setBounds(buttonBounds);
    speakerButton.setBounds(speakerButton.getBounds().removeFromRight(inset));
    
    auto typeBoxBounds = bounds.removeFromTop(40);
    typeBoxBounds.removeFromLeft(40);
    typeBoxBounds.removeFromRight(40);
    deviceTypeBox.setBounds(typeBoxBounds);
    bounds.removeFromTop(15);
    
    bounds.removeFromLeft(125);
    outputVolumeSlider->setBounds(bounds.removeFromBottom(65));
    bounds.removeFromTop(15);
}

