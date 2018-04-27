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
    addParameter(categoryParam = new AudioParameterChoice("CATEGORY", "Device Category", {"Phone", "Laptop", "Television", "Bluetooth Speaker"}, 0));
    addParameter(phoneTypeParam = new AudioParameterChoice("PHONETYPE", "Phone Type", {"iPhone 7 Plus"}, 0));
    addParameter(laptopTypeParam = new AudioParameterChoice("LAPTOPTYPE", "Laptop Type", {"MacBook Pro (2013)"}, 0));
    addParameter(tvTypeParam = new AudioParameterChoice("TVTYPE", "TV Type", {"Panasonic TX-L47ET5B"}, 0));
    addParameter(bluetoothTypeParam = new AudioParameterChoice("BLUETOOTHTYPE", "Bluetooth Speaker Type", {"Sony SRSX11", "Genelec 6010A (Pair)"}, 0));
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
    convLL.prepare(spec);
    convLR.prepare(spec);
    convRL.prepare(spec);
    convRR.prepare(spec);
    outputVolume.prepare(spec);
    auto maxSize = static_cast<size_t> (roundToInt (getSampleRate() * (8192.0 / 44100.0)));
    //convolution.loadImpulseResponse(BinaryData::iPhoneIR_wav, BinaryData::iPhoneIR_wavSize, false, true, maxSize);
//    convLL.loadImpulseResponse(BinaryData::iPhone7PLL_wav, BinaryData::iPhone7PLL_wavSize, false, false, maxSize, false);
//    convLR.loadImpulseResponse(BinaryData::iPhone7PLR_wav, BinaryData::iPhone7PLR_wavSize, false, false, maxSize, false);
//    convRL.loadImpulseResponse(BinaryData::iPhone7PRL_wav, BinaryData::iPhone7PRL_wavSize, false, false, maxSize, false);
//    convRR.loadImpulseResponse(BinaryData::iPhone7PRR_wav, BinaryData::iPhone7PRR_wavSize, false, false, maxSize, false);
    
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

void DeviceSimulationPluginAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
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
    
    updateParameters();
    
    // Make copies of audio samples to be processed
    AudioBuffer<float> bufferA, bufferB;
    bufferA.makeCopyOf(buffer);
    bufferB.makeCopyOf(buffer);
    
    // Blocks A and B are pointing to separate copies of the input audio
    dsp::AudioBlock<float> blockA (bufferA);
    dsp::AudioBlock<float> blockB (bufferB);
    // Chop blocks into separate left/right channels
    dsp::AudioBlock<float> inLeftA = blockA.getSingleChannelBlock(0);
    dsp::AudioBlock<float> inLeftB = blockB.getSingleChannelBlock(0);
    dsp::AudioBlock<float> inRightA = blockA.getSingleChannelBlock(1);
    dsp::AudioBlock<float> inRightB = blockB.getSingleChannelBlock(1);
    
    // Perform two convolutions on each channel of input data
    convLL.process(dsp::ProcessContextReplacing<float> (inLeftA));
    convRL.process(dsp::ProcessContextReplacing<float> (inLeftB));
    convLR.process(dsp::ProcessContextReplacing<float> (inRightA));
    convRR.process(dsp::ProcessContextReplacing<float> (inRightB));
    
    // Sum the four convolved signals together to form two output channels
    // outBlock is pointing at the input buffer so changing its contents generates the plugin output
    dsp::AudioBlock<float> outBlock (buffer);
    dsp::AudioBlock<float> outLeft = outBlock.getSingleChannelBlock(0);
    outLeft.copy(inLeftA.add(inRightA));
    dsp::AudioBlock<float> outRight = outBlock.getSingleChannelBlock(1);
    outRight.copy(inLeftB.add(inRightB));
    
    // Apply gain stage to output signal
    outputVolume.process(dsp::ProcessContextReplacing<float> (outLeft));
    outputVolume.process(dsp::ProcessContextReplacing<float> (outRight));
}

void DeviceSimulationPluginAudioProcessor::updateParameters() {
    auto outputdB = Decibels::decibelsToGain(outputVolumeParam->get());
    if (outputVolume.getGainLinear() != outputdB) outputVolume.setGainLinear(outputdB);
    
    size_t maxSize = static_cast<size_t> (roundToInt (getSampleRate() * (8192.0 / 44100.0)));
    
    auto newCategory = categoryParam->getIndex();
    auto currentCategory = category.get();
    bool categoryChanged = false;
    
    // If the category has just changed, the default device from that category will be loaded
    // Otherwise the category will be checked and if a different device has been selected using the dropdown menu
    // then the simulation will change accordingly
    if (newCategory != currentCategory) {
        categoryChanged = true;
        category.set(newCategory);
        
        switch(newCategory) {
            case 0:
                changePhone(maxSize, categoryChanged);
                break;
            case 1:
                changeLaptop(maxSize, categoryChanged);
                break;
            case 2:
                changeTelevision(maxSize, categoryChanged);
                break;
            case 3:
                changeBluetooth(maxSize, categoryChanged);
                break;
            default:
                changePhone(maxSize, categoryChanged);
                break;
        }
    }
    
    if (fileChanged) {
        fileChanged = false;
        impulsesLoaded = true;
        // convolution.loadImpulseResponse(otherIRFile, false, true, maxSize);
        convLL.loadImpulseResponse(llIR, false, false, maxSize, false);
        convLR.loadImpulseResponse(lrIR, false, false, maxSize, false);
        convRL.loadImpulseResponse(rlIR, false, false, maxSize, false);
        convRR.loadImpulseResponse(rrIR, false, false, maxSize, false);
    }
}

void DeviceSimulationPluginAudioProcessor::changePhone(size_t maxSize, bool categoryChanged) {
    if (categoryChanged) {
        convLL.loadImpulseResponse(BinaryData::iPhone7PLL_wav, BinaryData::iPhone7PLL_wavSize, false, false, maxSize, false);
        convLR.loadImpulseResponse(BinaryData::iPhone7PLR_wav, BinaryData::iPhone7PLR_wavSize, false, false, maxSize, false);
        convRL.loadImpulseResponse(BinaryData::iPhone7PRL_wav, BinaryData::iPhone7PRL_wavSize, false, false, maxSize, false);
        convRR.loadImpulseResponse(BinaryData::iPhone7PRR_wav, BinaryData::iPhone7PRR_wavSize, false, false, maxSize, false);
    }
    else {
        auto newPhone = phoneTypeParam->getIndex();
        auto currentPhone = phoneType.get();
        if (newPhone != currentPhone) {
            switch(newPhone) {
                case 0:
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
}

void DeviceSimulationPluginAudioProcessor::changeLaptop(size_t maxSize, bool categoryChanged) {
    if (categoryChanged) {
        convLL.loadImpulseResponse(BinaryData::MacBookProLL_wav, BinaryData::MacBookProLL_wavSize, false, false, maxSize, false);
        convLR.loadImpulseResponse(BinaryData::MacBookProLR_wav, BinaryData::MacBookProLR_wavSize, false, false, maxSize, false);
        convRL.loadImpulseResponse(BinaryData::MacBookProRL_wav, BinaryData::MacBookProRL_wavSize, false, false, maxSize, false);
        convRR.loadImpulseResponse(BinaryData::MacBookProRR_wav, BinaryData::MacBookProRR_wavSize, false, false, maxSize, false);
    }
    else {
        auto newLaptop = laptopTypeParam->getIndex();
        auto currentLaptop = laptopType.get();
        if (newLaptop != currentLaptop) {
            switch(newLaptop) {
                case 0:
                    convLL.loadImpulseResponse(BinaryData::MacBookProLL_wav, BinaryData::MacBookProLL_wavSize, false, false, maxSize, false);
                    convLR.loadImpulseResponse(BinaryData::MacBookProLR_wav, BinaryData::MacBookProLR_wavSize, false, false, maxSize, false);
                    convRL.loadImpulseResponse(BinaryData::MacBookProRL_wav, BinaryData::MacBookProRL_wavSize, false, false, maxSize, false);
                    convRR.loadImpulseResponse(BinaryData::MacBookProRR_wav, BinaryData::MacBookProRR_wavSize, false, false, maxSize, false);
                    break;
                default:
                    convLL.loadImpulseResponse(BinaryData::MacBookProLL_wav, BinaryData::MacBookProLL_wavSize, false, false, maxSize, false);
                    convLR.loadImpulseResponse(BinaryData::MacBookProLR_wav, BinaryData::MacBookProLR_wavSize, false, false, maxSize, false);
                    convRL.loadImpulseResponse(BinaryData::MacBookProRL_wav, BinaryData::MacBookProRL_wavSize, false, false, maxSize, false);
                    convRR.loadImpulseResponse(BinaryData::MacBookProRR_wav, BinaryData::MacBookProRR_wavSize, false, false, maxSize, false);
                    break;
            }
        }
    }
}

void DeviceSimulationPluginAudioProcessor::changeTelevision(size_t maxSize, bool categoryChanged) {
    if (categoryChanged) {
        convLL.loadImpulseResponse(BinaryData::PanaTVLL_wav, BinaryData::PanaTVLL_wavSize, false, false, maxSize, false);
        convLR.loadImpulseResponse(BinaryData::PanaTVLR_wav, BinaryData::PanaTVLR_wavSize, false, false, maxSize, false);
        convRL.loadImpulseResponse(BinaryData::PanaTVRL_wav, BinaryData::PanaTVRL_wavSize, false, false, maxSize, false);
        convRR.loadImpulseResponse(BinaryData::PanaTVRR_wav, BinaryData::PanaTVRR_wavSize, false, false, maxSize, false);
    }
    else {
        auto newTV = tvTypeParam->getIndex();
        auto currentTV = tvType.get();
        if (newTV != currentTV) {
            switch(newTV) {
                case 0:
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
}

void DeviceSimulationPluginAudioProcessor::changeBluetooth(size_t maxSize, bool categoryChanged) {
    if (categoryChanged) {
        convLL.loadImpulseResponse(BinaryData::SRSX11LL_wav, BinaryData::SRSX11LL_wavSize, false, false, maxSize, false);
        convLR.loadImpulseResponse(BinaryData::SRSX11LR_wav, BinaryData::SRSX11LR_wavSize, false, false, maxSize, false);
        convRL.loadImpulseResponse(BinaryData::SRSX11RL_wav, BinaryData::SRSX11RL_wavSize, false, false, maxSize, false);
        convRR.loadImpulseResponse(BinaryData::SRSX11RR_wav, BinaryData::SRSX11RR_wavSize, false, false, maxSize, false);
    }
    else {
        auto newBluetooth = bluetoothTypeParam->getIndex();
        auto currentBluetooth = bluetoothType.get();
        if (newBluetooth != currentBluetooth) {
            switch(newBluetooth) {
                case 0:
                    convLL.loadImpulseResponse(BinaryData::SRSX11LL_wav, BinaryData::SRSX11LL_wavSize, false, false, maxSize, false);
                    convLR.loadImpulseResponse(BinaryData::SRSX11LR_wav, BinaryData::SRSX11LR_wavSize, false, false, maxSize, false);
                    convRL.loadImpulseResponse(BinaryData::SRSX11RL_wav, BinaryData::SRSX11RL_wavSize, false, false, maxSize, false);
                    convRR.loadImpulseResponse(BinaryData::SRSX11RR_wav, BinaryData::SRSX11RR_wavSize, false, false, maxSize, false);
                    break;
                case 1:
                    convLL.loadImpulseResponse(BinaryData::GenelecLL_wav, BinaryData::GenelecLL_wavSize, false, false, maxSize, false);
                    convLR.loadImpulseResponse(BinaryData::GenelecLR_wav, BinaryData::GenelecLR_wavSize, false, false, maxSize, false);
                    convRL.loadImpulseResponse(BinaryData::GenelecRL_wav, BinaryData::GenelecRL_wavSize, false, false, maxSize, false);
                    convRR.loadImpulseResponse(BinaryData::GenelecRR_wav, BinaryData::GenelecRR_wavSize, false, false, maxSize, false);
                    break;
                default:
                    convLL.loadImpulseResponse(BinaryData::SRSX11LL_wav, BinaryData::SRSX11LL_wavSize, false, false, maxSize, false);
                    convLR.loadImpulseResponse(BinaryData::SRSX11LR_wav, BinaryData::SRSX11LR_wavSize, false, false, maxSize, false);
                    convRL.loadImpulseResponse(BinaryData::SRSX11RL_wav, BinaryData::SRSX11RL_wavSize, false, false, maxSize, false);
                    convRR.loadImpulseResponse(BinaryData::SRSX11RR_wav, BinaryData::SRSX11RR_wavSize, false, false, maxSize, false);
                    break;
            }
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

