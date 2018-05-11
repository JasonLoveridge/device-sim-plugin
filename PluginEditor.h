/*
 ==============================================================================
 
 Device Simulation Plugin - PluginEditor.h
 Author: Jason Loveridge
 Date: 05/2018
 BBC Research & Development
 
 Built upon JUCE plugin framework
 
 ==============================================================================
 */

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "Images.h"

// ParameterSlider object - code taken from "DSP module plugin demo" provided by JUCE
struct ParameterSlider    : public Slider,
public Timer
{
    ParameterSlider (AudioProcessorParameter& p)
    : Slider (p.getName (256)), param (p)
    {
        setRange (0.0, 1.0, 0.0);
        startTimerHz (30);
        updateSliderPos();
    }
    
    void valueChanged() override
    {
        if (isMouseButtonDown())
            param.setValueNotifyingHost ((float) Slider::getValue());
        else
            param.setValue ((float) Slider::getValue());
    }
    
    void timerCallback() override       { updateSliderPos(); }
    
    void startedDragging() override     { param.beginChangeGesture(); }
    void stoppedDragging() override     { param.endChangeGesture();   }
    
    double getValueFromText (const String& text) override   { return param.getValueForText (text); }
    String getTextFromValue (double value) override         { return param.getText ((float) value, 1024) + " " + param.getLabel(); }
    
    void updateSliderPos()
    {
        const float newValue = param.getValue();
        
        if (newValue != (float) Slider::getValue() && ! isMouseButtonDown())
            Slider::setValue (newValue);
    }
    
    AudioProcessorParameter& param;
};

//==============================================================================
/**
 */
class DeviceSimulationPluginAudioProcessorEditor  : public AudioProcessorEditor,
private ComboBox::Listener,
private Button::Listener
{
public:
    DeviceSimulationPluginAudioProcessorEditor (DeviceSimulationPluginAudioProcessor&);
    ~DeviceSimulationPluginAudioProcessorEditor();
    
    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
private:
    
    // Declare functions and GUI components required for plugin
    Image deviceImage;
    
    void comboBoxChanged(ComboBox*) override;
    void buttonClicked(Button*) override;
    void fillDeviceTypeBox(int category);
    
    DeviceSimulationPluginAudioProcessor& processor;
    Label outputGainLabel, deviceTypeLabel, titleLabel;
    Label phoneButtonLabel, laptopButtonLabel, tvButtonLabel, speakerButtonLabel;
    ScopedPointer<ParameterSlider> outputGainSlider;
    ComboBox deviceTypeBox;
    ToggleButton phoneButton, laptopButton, tvButton, speakerButton;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DeviceSimulationPluginAudioProcessorEditor)
};

