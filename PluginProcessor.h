/*
 ==============================================================================
 
 This file was auto-generated!
 
 It contains the basic framework code for a JUCE plugin processor.
 
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
    
    AudioParameterFloat* outputVolumeParam;
    AudioParameterChoice* categoryParam;
    AudioParameterChoice* phoneTypeParam;
    AudioParameterChoice* laptopTypeParam;
    AudioParameterChoice* tvTypeParam;
    
    File otherIRFile;
    File llIR, lrIR, rlIR, rrIR;
    bool fileChanged = false;
    bool impulsesLoaded = false;
    
private:
    dsp::Convolution convolution;
    dsp::Convolution convLL, convLR, convRL, convRR;
    dsp::Gain<float> outputVolume;
    
    Atomic<int> deviceType;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DeviceSimulationPluginAudioProcessor)
};

