/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
DeviceSimulationPluginAudioProcessor::DeviceSimulationPluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    addParameter(outputVolumeParam = new AudioParameterFloat("OUTPUT", "Output Volume", {-40.f, 40.f, 0.f, 1.0f}, 0.f, "dB"));
    addParameter(deviceTypeParam = new AudioParameterChoice("DEVICETYPE", "Device Type", {"iPhone 4s", "iPad 2"}, 0));
}

DeviceSimulationPluginAudioProcessor::~DeviceSimulationPluginAudioProcessor()
{
}

//==============================================================================
const String DeviceSimulationPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DeviceSimulationPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DeviceSimulationPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DeviceSimulationPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DeviceSimulationPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DeviceSimulationPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DeviceSimulationPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DeviceSimulationPluginAudioProcessor::setCurrentProgram (int index)
{
}

const String DeviceSimulationPluginAudioProcessor::getProgramName (int index)
{
    return {};
}

void DeviceSimulationPluginAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void DeviceSimulationPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    auto channels = static_cast<uint32> (jmin (getMainBusNumInputChannels(), getMainBusNumOutputChannels()));
    dsp::ProcessSpec spec { sampleRate, static_cast<uint32> (samplesPerBlock), channels };
    convolution.prepare(spec);
    outputVolume.prepare(spec);
    updateParameters();
}

void DeviceSimulationPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DeviceSimulationPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void DeviceSimulationPluginAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    updateParameters();
    
    dsp::AudioBlock<float> block (buffer);
    convolution.process(dsp::ProcessContextReplacing<float> (block));
    outputVolume.process(dsp::ProcessContextReplacing<float> (block));
}

void DeviceSimulationPluginAudioProcessor::updateParameters() {
    auto outputdB = Decibels::decibelsToGain(outputVolumeParam->get());
    if (outputVolume.getGainLinear() != outputdB) outputVolume.setGainLinear(outputdB);
    
    auto maxSize = static_cast<size_t> (roundToInt (getSampleRate() * (8192.0 / 44100.0)));
    
    auto type = deviceTypeParam->getIndex();
    auto currentType = deviceType.get();
    if (type != currentType) {
        deviceType.set(type);

        if (type == 0) {
            convolution.loadImpulseResponse(BinaryData::iPhoneIR_wav, BinaryData::iPhoneIR_wavSize, false, true, maxSize);
        }
        else if (type == 1) {
            convolution.loadImpulseResponse(BinaryData::iPadIR_wav, BinaryData::iPadIR_wavSize, false, true, maxSize);
        }
    }
    
    if (fileChanged) {
        fileChanged = false;
        impulsesLoaded = true;
        convolution.loadImpulseResponse(otherIRFile, false, true, maxSize);
    }
}

//==============================================================================
bool DeviceSimulationPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* DeviceSimulationPluginAudioProcessor::createEditor()
{
    return new DeviceSimulationPluginAudioProcessorEditor (*this);
}

//==============================================================================
void DeviceSimulationPluginAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void DeviceSimulationPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DeviceSimulationPluginAudioProcessor();
}
