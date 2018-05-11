/*
 ==============================================================================
 
 Device Simulation Plugin - PluginProcessor.cpp
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
    // Initialise AudioParameters with device options
    addParameter(outputGainParam = new AudioParameterFloat("OUTPUT", "Output Gain", {-40.f, 40.f, 0.f, 1.0f}, 0.f, "dB"));
    addParameter(categoryParam = new AudioParameterChoice("CATEGORY", "Device Category", {"Phone", "Laptop", "Television", "speaker Speaker"}, 0));
    addParameter(phoneTypeParam = new AudioParameterChoice("PHONETYPE", "Phone Type", {"iPhone 7 Plus"}, 0));
    addParameter(laptopTypeParam = new AudioParameterChoice("LAPTOPTYPE", "Laptop Type", {"MacBook Pro (2013)"}, 0));
    addParameter(tvTypeParam = new AudioParameterChoice("TVTYPE", "TV Type", {"Panasonic TX-L47ET5B"}, 0));
    addParameter(speakerTypeParam = new AudioParameterChoice("SPEAKERTYPE", "Other Speaker Type", {"Sony SRSX11", "Genelec 6010A (Pair)"}, 0));
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
    // Prepare DSP objects for playback
    auto channels = static_cast<uint32> (jmin (getMainBusNumInputChannels(), getMainBusNumOutputChannels()));
    dsp::ProcessSpec spec { sampleRate, static_cast<uint32> (samplesPerBlock), channels };

    convLL.prepare(spec);
    convLR.prepare(spec);
    convRL.prepare(spec);
    convRR.prepare(spec);
    outputGain.prepare(spec);
    
    // If no impulses have been loaded (ie the plugin has been opened for the first time) then load the default iPhone IRs
    if (!impulsesLoaded) {
        auto maxSize = static_cast<size_t> (roundToInt (getSampleRate() * (8192.0 / 44100.0)));
        convLL.loadImpulseResponse(BinaryData::iPhone7PLL_wav, BinaryData::iPhone7PLL_wavSize, false, false, maxSize, false);
        convLR.loadImpulseResponse(BinaryData::iPhone7PLR_wav, BinaryData::iPhone7PLR_wavSize, false, false, maxSize, false);
        convRL.loadImpulseResponse(BinaryData::iPhone7PRL_wav, BinaryData::iPhone7PRL_wavSize, false, false, maxSize, false);
        convRR.loadImpulseResponse(BinaryData::iPhone7PRR_wav, BinaryData::iPhone7PRR_wavSize, false, false, maxSize, false);
        impulsesLoaded = true;
    }
    
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

// ******************************************************
// Main processing function where convolution is executed
// ******************************************************
void DeviceSimulationPluginAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    // JUCE default code
    // ------------------------------------------------------------------------
    ScopedNoDenormals noDenormals;
    const int totalNumInputChannels  = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();
    
    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    // ------------------------------------------------------------------------
    
    updateParameters();
    
    // Make copies of current audio samples to be processed
    AudioBuffer<float> bufferA, bufferB;
    bufferA.makeCopyOf(buffer);
    bufferB.makeCopyOf(buffer);
    
    // Create AudioBlocks pointing to the separate copies of audio samples to be processed
    dsp::AudioBlock<float> blockA (bufferA);
    dsp::AudioBlock<float> blockB (bufferB);
    
    // Chop blocks into separate left/right channels
    dsp::AudioBlock<float> inLeftA = blockA.getSingleChannelBlock(LEFT);
    dsp::AudioBlock<float> inLeftB = blockB.getSingleChannelBlock(LEFT);
    dsp::AudioBlock<float> inRightA = blockA.getSingleChannelBlock(RIGHT);
    dsp::AudioBlock<float> inRightB = blockB.getSingleChannelBlock(RIGHT);
    
    // Perform two convolutions on each channel of input data
    convLL.process(dsp::ProcessContextReplacing<float> (inLeftA));
    convRL.process(dsp::ProcessContextReplacing<float> (inLeftB));
    convLR.process(dsp::ProcessContextReplacing<float> (inRightA));
    convRR.process(dsp::ProcessContextReplacing<float> (inRightB));
    
    // Sum the four convolved signals together to form two output channels
    // outBlock is pointing at the original input buffer, so changing its contents changes the plugin output
    dsp::AudioBlock<float> outBlock (buffer);
    dsp::AudioBlock<float> outLeft = outBlock.getSingleChannelBlock(LEFT);
    outLeft.copy(inLeftA.add(inRightA));
    dsp::AudioBlock<float> outRight = outBlock.getSingleChannelBlock(RIGHT);
    outRight.copy(inLeftB.add(inRightB));
    
    // Apply gain stage to output signal
    outputGain.process(dsp::ProcessContextReplacing<float> (outLeft));
    outputGain.process(dsp::ProcessContextReplacing<float> (outRight));
}

// This function is called frequently from the processBlock function and is used to check for changes to parameters
void DeviceSimulationPluginAudioProcessor::updateParameters() {
    
    // Update output gain setting
    auto outputdB = Decibels::decibelsToGain(outputGainParam->get());
    if (outputGain.getGainLinear() != outputdB) outputGain.setGainLinear(outputdB);
    
    size_t maxSize = static_cast<size_t> (roundToInt (getSampleRate() * (8192.0 / 44100.0)));
    
    auto currentCategory = categoryParam->getIndex();
    auto previousCategory = category.get();
    bool categoryChanged = false;
    
    // Check if the device category has been changed
    if (currentCategory != previousCategory) {
        categoryChanged = true;
        category.set(currentCategory);
    }
    
    // If there has been a category change or combo box selection change then new impulse responses will be loaded
    switch(currentCategory) {
        case PHONE:
            changePhone(maxSize, categoryChanged);
            break;
        case LAPTOP:
            changeLaptop(maxSize, categoryChanged);
            break;
        case TELEVISION:
            changeTelevision(maxSize, categoryChanged);
            break;
        case SPEAKER:
            changeSpeaker(maxSize, categoryChanged);
            break;
        default:
            changePhone(maxSize, categoryChanged);
            break;
    }
}

//******************************************************************
// Each of the following five functions loads impulse responses according to category and device selection
// If the category has just been changed, or the device selection within the same category has been changed
// then new impulse responses are loaded into the convolution objects accordingly
// If the category has been changed, the last selected device from that category is loaded (if it is the first time
// using that category then the default device is loaded)
//
// See changeSpeaker for example code where more than one option exists within a category
//******************************************************************
void DeviceSimulationPluginAudioProcessor::changePhone(size_t maxSize, bool categoryChanged) {
    auto newPhone = phoneTypeParam->getIndex();
    auto currentPhone = phoneType.get();
    if (newPhone != currentPhone || categoryChanged) {
        phoneType.set(newPhone);
        switch(newPhone) {
            case IPHONE_7_PLUS:
                convLL.loadImpulseResponse(BinaryData::iPhone7PLL_wav, BinaryData::iPhone7PLL_wavSize, false, false, maxSize, false);
                convLR.loadImpulseResponse(BinaryData::iPhone7PLR_wav, BinaryData::iPhone7PLR_wavSize, false, false, maxSize, false);
                convRL.loadImpulseResponse(BinaryData::iPhone7PRL_wav, BinaryData::iPhone7PRL_wavSize, false, false, maxSize, false);
                convRR.loadImpulseResponse(BinaryData::iPhone7PRR_wav, BinaryData::iPhone7PRR_wavSize, false, false, maxSize, false);
                break;
            default:
                convLL.loadImpulseResponse(BinaryData::iPhone7PLL_wav, BinaryData::iPhone7PLL_wavSize, false, false, maxSize, false);
                convLR.loadImpulseResponse(BinaryData::iPhone7PLR_wav, BinaryData::iPhone7PLR_wavSize, false, false, maxSize, false);
                convRL.loadImpulseResponse(BinaryData::iPhone7PRL_wav, BinaryData::iPhone7PRL_wavSize, false, false, maxSize, false);
                convRR.loadImpulseResponse(BinaryData::iPhone7PRR_wav, BinaryData::iPhone7PRR_wavSize, false, false, maxSize, false);
                break;
        }
    }
}

void DeviceSimulationPluginAudioProcessor::changeLaptop(size_t maxSize, bool categoryChanged) {
    auto newLaptop = laptopTypeParam->getIndex();
    auto currentLaptop = laptopType.get();
    if (newLaptop != currentLaptop || categoryChanged) {
        laptopType.set(newLaptop);
        switch(newLaptop) {
            case MACBOOK_PRO_2013:
                convLL.loadImpulseResponse(BinaryData::MacBook13LL_wav, BinaryData::MacBook13LL_wavSize, false, false, maxSize, false);
                convLR.loadImpulseResponse(BinaryData::MacBook13LR_wav, BinaryData::MacBook13LR_wavSize, false, false, maxSize, false);
                convRL.loadImpulseResponse(BinaryData::MacBook13RL_wav, BinaryData::MacBook13RL_wavSize, false, false, maxSize, false);
                convRR.loadImpulseResponse(BinaryData::MacBook13RR_wav, BinaryData::MacBook13RR_wavSize, false, false, maxSize, false);
                break;
            default:
                convLL.loadImpulseResponse(BinaryData::MacBook13LL_wav, BinaryData::MacBook13LL_wavSize, false, false, maxSize, false);
                convLR.loadImpulseResponse(BinaryData::MacBook13LR_wav, BinaryData::MacBook13LR_wavSize, false, false, maxSize, false);
                convRL.loadImpulseResponse(BinaryData::MacBook13RL_wav, BinaryData::MacBook13RL_wavSize, false, false, maxSize, false);
                convRR.loadImpulseResponse(BinaryData::MacBook13RR_wav, BinaryData::MacBook13RR_wavSize, false, false, maxSize, false);
                break;
        }
    }
}

void DeviceSimulationPluginAudioProcessor::changeTelevision(size_t maxSize, bool categoryChanged) {
    auto newTV = tvTypeParam->getIndex();
    auto currentTV = tvType.get();
    if (newTV != currentTV || categoryChanged) {
        tvType.set(newTV);
        switch(newTV) {
            case PANASONIC_TXL47ET5B:
                convLL.loadImpulseResponse(BinaryData::PanaTVLL_wav, BinaryData::PanaTVLL_wavSize, false, false, maxSize, false);
                convLR.loadImpulseResponse(BinaryData::PanaTVLR_wav, BinaryData::PanaTVLR_wavSize, false, false, maxSize, false);
                convRL.loadImpulseResponse(BinaryData::PanaTVRL_wav, BinaryData::PanaTVRL_wavSize, false, false, maxSize, false);
                convRR.loadImpulseResponse(BinaryData::PanaTVRR_wav, BinaryData::PanaTVRR_wavSize, false, false, maxSize, false);
                break;
            default:
                convLL.loadImpulseResponse(BinaryData::PanaTVLL_wav, BinaryData::PanaTVLL_wavSize, false, false, maxSize, false);
                convLR.loadImpulseResponse(BinaryData::PanaTVLR_wav, BinaryData::PanaTVLR_wavSize, false, false, maxSize, false);
                convRL.loadImpulseResponse(BinaryData::PanaTVRL_wav, BinaryData::PanaTVRL_wavSize, false, false, maxSize, false);
                convRR.loadImpulseResponse(BinaryData::PanaTVRR_wav, BinaryData::PanaTVRR_wavSize, false, false, maxSize, false);
                break;
        }
    }
}

void DeviceSimulationPluginAudioProcessor::changeSpeaker(size_t maxSize, bool categoryChanged) {
    auto newSpeaker = speakerTypeParam->getIndex();
    auto currentSpeaker = speakerType.get();
    if (newSpeaker != currentSpeaker || categoryChanged) {
        speakerType.set(newSpeaker);
        switch(newSpeaker) {
            case SONY_SRSX11:
                convLL.loadImpulseResponse(BinaryData::SonySRSX11LL_wav, BinaryData::SonySRSX11LL_wavSize, false, false, maxSize, false);
                convLR.loadImpulseResponse(BinaryData::SonySRSX11LR_wav, BinaryData::SonySRSX11LR_wavSize, false, false, maxSize, false);
                convRL.loadImpulseResponse(BinaryData::SonySRSX11RL_wav, BinaryData::SonySRSX11RL_wavSize, false, false, maxSize, false);
                convRR.loadImpulseResponse(BinaryData::SonySRSX11RR_wav, BinaryData::SonySRSX11RR_wavSize, false, false, maxSize, false);
                break;
            case GENELEC_6010_PAIR:
                convLL.loadImpulseResponse(BinaryData::Gen6010ALL_wav, BinaryData::Gen6010ALL_wavSize, false, false, maxSize, false);
                convLR.loadImpulseResponse(BinaryData::Gen6010ALR_wav, BinaryData::Gen6010ALR_wavSize, false, false, maxSize, false);
                convRL.loadImpulseResponse(BinaryData::Gen6010ARL_wav, BinaryData::Gen6010ARL_wavSize, false, false, maxSize, false);
                convRR.loadImpulseResponse(BinaryData::Gen6010ARR_wav, BinaryData::Gen6010ARR_wavSize, false, false, maxSize, false);
                break;
            default:
                convLL.loadImpulseResponse(BinaryData::SonySRSX11LL_wav, BinaryData::SonySRSX11LL_wavSize, false, false, maxSize, false);
                convLR.loadImpulseResponse(BinaryData::SonySRSX11LR_wav, BinaryData::SonySRSX11LR_wavSize, false, false, maxSize, false);
                convRL.loadImpulseResponse(BinaryData::SonySRSX11RL_wav, BinaryData::SonySRSX11RL_wavSize, false, false, maxSize, false);
                convRR.loadImpulseResponse(BinaryData::SonySRSX11RR_wav, BinaryData::SonySRSX11RR_wavSize, false, false, maxSize, false);
                break;
        }
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

