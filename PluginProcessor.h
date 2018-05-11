/*
 ==============================================================================
 
 Device Simulation Plugin - PluginProcessor.h
 Author: Jason Loveridge
 Date: 05/2018
 BBC Research & Development
 
 Built upon JUCE plugin framework
 
 ==============================================================================
 */

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"


//==============================================================================
/**
 */
class DeviceSimulationPluginAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    DeviceSimulationPluginAudioProcessor();
    ~DeviceSimulationPluginAudioProcessor();
    
    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    
#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
#endif
    
    void processBlock (AudioSampleBuffer&, MidiBuffer&) override;
    
    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
    
    //==============================================================================
    const String getName() const override;
    
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect () const override;
    double getTailLengthSeconds() const override;
    
    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;
    
    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    void updateParameters();
    
    //******************************************************************************
    // Above is default JUCE framework code
    // Below are functions and variables unique to this plugin
    //******************************************************************************

    void changePhone(size_t maxSize, bool categoryChanged);
    void changeLaptop(size_t maxSize, bool categoryChanged);
    void changeTelevision(size_t maxSize, bool categoryChanged);
    void changeSpeaker(size_t maxSize, bool categoryChanged);
    
    AudioParameterFloat* outputGainParam;
    AudioParameterChoice* categoryParam;
    AudioParameterChoice* phoneTypeParam;
    AudioParameterChoice* laptopTypeParam;
    AudioParameterChoice* tvTypeParam;
    AudioParameterChoice* speakerTypeParam;

    bool impulsesLoaded = false;
    
private:
    enum Channels {
        LEFT = 0,
        RIGHT = 1
    };
    
    dsp::Convolution convLL, convLR, convRL, convRR;
    dsp::Gain<float> outputGain;
    
    Atomic<int> category;
    Atomic<int> phoneType;
    Atomic<int> laptopType;
    Atomic<int> tvType;
    Atomic<int> speakerType;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DeviceSimulationPluginAudioProcessor)
};

