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
outputGainLabel({}, processor.outputGainParam->name)
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
    
    // Fill combobox with device choices from relevant category
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
    
    // Add combobox, label and also output gain control and label
    deviceTypeBox.addListener(this);
    deviceTypeBox.setJustificationType(Justification::horizontallyCentred);
    
    addAndMakeVisible(deviceTypeLabel);
    deviceTypeLabel.setJustificationType(Justification::centredLeft);
    deviceTypeLabel.attachToComponent(&deviceTypeBox, true);
    
    addAndMakeVisible(outputGainSlider = new ParameterSlider(*processor.outputGainParam));
    
    addAndMakeVisible(outputGainLabel);
    outputGainLabel.setJustificationType(Justification::centredLeft);
    outputGainLabel.attachToComponent(outputGainSlider, true);
    
    // Initialise device image to iPhone
    deviceImage = ImageCache::getFromMemory(Images::iPhoneCropped_png, Images::iPhoneCropped_pngSize);
    
    setSize (500, 450);
}

DeviceSimulationPluginAudioProcessorEditor::~DeviceSimulationPluginAudioProcessorEditor()
{
}

// If the combobox is changed, change the corresponding device parameter in the processor
// The current device category is switched on such that the correct device parameter is changed
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

// If one of the radio buttons is clicked, call a function to update the combobox accordingly
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
// Options for each device category and taken from the corresponding AudioParameter, which is
// initialised in the processor
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
    
    // Change image according to device selected
    // Fixed values are used for each image's size and location such that they are all centred
    // This is not ideal and could be improved by using source images that all have the same dimensions
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

// The layout of the plugin is done by dividing down the GUI area into rectangles
// See JUCE tutorials for more on this
void DeviceSimulationPluginAudioProcessorEditor::resized()
{
    const int border = 10;
    const int titleHeight = 30;
    const int categoriesHeight = 80;
    const int catLabelHeight = 30;
    const int buttonSize = 14;
    const int comboBoxBoundsHeight = 40;
    const int comboBoxInset = 40;
    const int gainSliderWidth = 375;
    const int gainSliderHeight = 65;
    
    // Add the title in a rectangle at the top of the plugin window
    auto bounds = getLocalBounds().reduced(border);
    titleLabel.setBounds(bounds.removeFromTop(titleHeight));
    
    // catButtonBounds is the rectangle that contains the label and radio button for each device category
    auto catButtonBounds = bounds.removeFromTop(categoriesHeight).reduced(border);
    
    // catLabelBounds is a sub-rectangle within catButtonBounds for the labels only
    // Labels are spaced out evenly across the width available
    auto catLabelBounds = catButtonBounds.removeFromTop(catLabelHeight);
    phoneButtonLabel.setBounds(catLabelBounds.removeFromLeft(catLabelBounds.getWidth() / 4));
    laptopButtonLabel.setBounds(catLabelBounds.removeFromLeft(catLabelBounds.getWidth() / 3));
    tvButtonLabel.setBounds(catLabelBounds.removeFromLeft(catLabelBounds.getWidth() / 2));
    speakerButtonLabel.setBounds(catLabelBounds);
    
    // The rest of catButtonBounds is used to add the buttons
    // Buttons are spaced out evenly across the width available, using inset to ensure correct spacing
    const int inset = (catButtonBounds.getWidth() / 8) + buttonSize;
    phoneButton.setBounds(catButtonBounds.removeFromLeft(catButtonBounds.getWidth() / 4));
    phoneButton.setBounds(phoneButton.getBounds().removeFromRight(inset));
    laptopButton.setBounds(catButtonBounds.removeFromLeft(catButtonBounds.getWidth() / 3));
    laptopButton.setBounds(laptopButton.getBounds().removeFromRight(inset));
    tvButton.setBounds(catButtonBounds.removeFromLeft(catButtonBounds.getWidth() / 2));
    tvButton.setBounds(tvButton.getBounds().removeFromRight(inset));
    speakerButton.setBounds(catButtonBounds);
    speakerButton.setBounds(speakerButton.getBounds().removeFromRight(inset));
    
    // Device selection combobox is added next, using its own inset to reduce its width
    auto comboBoxBounds = bounds.removeFromTop(comboBoxBoundsHeight);
    comboBoxBounds.removeFromLeft(comboBoxInset);
    comboBoxBounds.removeFromRight(comboBoxInset);
    deviceTypeBox.setBounds(comboBoxBounds);
    
    // Taking only a portion of the available width for the slider leaves space for the
    // label and value to be displayed to the left of the slider
    auto sliderBounds = bounds.removeFromRight(gainSliderWidth);
    outputGainSlider->setBounds(sliderBounds.removeFromBottom(gainSliderHeight));
}

