/*
 ==============================================================================
 
 This file was auto-generated!
 
 It contains the basic framework code for a JUCE plugin editor.
 
 ==============================================================================
 */

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

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
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    
    void comboBoxChanged(ComboBox*) override;
    void buttonClicked(Button*) override;
    
    bool launchingForFirstTime = true;
    int deviceCategory = 0;
    
    DeviceSimulationPluginAudioProcessor& processor;
    Label outputVolumeLabel, deviceTypeLabel, otherIRLabel, titleLabel;
    Label phoneButtonLabel, laptopButtonLabel, tvButtonLabel;
    ScopedPointer<ParameterSlider> outputVolumeSlider;
    ComboBox deviceTypeBox;
    TextButton otherButton;
    ToggleButton phoneButton, laptopButton, tvButton;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DeviceSimulationPluginAudioProcessorEditor)
};

