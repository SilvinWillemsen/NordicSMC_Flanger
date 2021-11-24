/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
NordicSMC_EffectAudioProcessor::NordicSMC_EffectAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

NordicSMC_EffectAudioProcessor::~NordicSMC_EffectAudioProcessor()
{
}

//==============================================================================
const juce::String NordicSMC_EffectAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool NordicSMC_EffectAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool NordicSMC_EffectAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool NordicSMC_EffectAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double NordicSMC_EffectAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int NordicSMC_EffectAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int NordicSMC_EffectAudioProcessor::getCurrentProgram()
{
    return 0;
}

void NordicSMC_EffectAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String NordicSMC_EffectAudioProcessor::getProgramName (int index)
{
    return {};
}

void NordicSMC_EffectAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void NordicSMC_EffectAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    
    fs = sampleRate; // Obtain the sample rate from the plugin host (or DAW) when the application starts
    delayLine.resize (maxDelay);
}

void NordicSMC_EffectAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool NordicSMC_EffectAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
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

void NordicSMC_EffectAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
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
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        // This is our input buffer. (To be used later)
        auto input = buffer.getReadPointer (channel);

        // This is the pointer our output buffer.
        // We will use this variable to send data to our speakers.
        auto* output = buffer.getWritePointer (channel);
                
        
        // Extra variables for clarity.
        float inputSignal = 0.0;
        float nonLimitedOutput = 0.0;
        
        // Here, we only do processing on the left channel and copy everything to the right channel
        if (channel == 0)
        {
            // Loop over all the samples in our buffer
            for (int i = 0; i < buffer.getNumSamples(); ++i)
            {
                /* Adding a parameter [2]: Apply to a signal
                 
                        Here, we're applying the gain (controlled by the slider) to a sinusoid.
                 */
                double phaseInc = 2.0 * double_Pi * freq / fs;
                curPhase += phaseInc;
                
                // ==== Comment out one of the below ==== //
                
                // Use a sinewave
//                inputSignal = gain * sin (curPhase);
                
                // Use external input
                inputSignal = gain * input[i];
                
                // ====================================== //
                
                // Write the input signal to the delay line at the write location
                delayLine[writeLoc] = inputSignal;

                // Calculate the current phase of the LFO
                double phaseIncLFO = 2.0 * double_Pi * freqLFO / fs;
                curPhaseLFO += phaseIncLFO;
                
                /*
                 Convert values of the LFO (sinewave) to a value between 0 and maxDelay.
                 DepthLFO is between 0 and 1 and is controlled by a slider
                 */
                double delay = depthLFO * maxDelay * (1.0 + sin (curPhaseLFO)) * 0.5;
                
                // Fractional part of the delay to be used for fractional delay
                double frac = delay - floor(delay);

                /*
                 Normally we would simply subtract the delay from the write location to get the read location. This might, however, result in negative indices.
                 In order to prevent this, we add maxDelay (the length of our delay line) before applying the modulo (%) operator.
                 */
                readLoc = (writeLoc - static_cast<int> (floor (delay)) + maxDelay) % maxDelay;
                
                // 2nd read location is used for the fractional delay
                int readLoc2 = (writeLoc - static_cast<int> (floor (delay + 1)) + maxDelay) % maxDelay;

                // Add the direct input signal to (fractional) output of the delayline
                nonLimitedOutput = inputSignal + (1.0 - frac) * delayLine[readLoc] + frac * delayLine[readLoc2];
                
                // "Implementing a limiter is the single most important
                // thing in real-time audio development" - Willemsen, 2021
                output[i] = limit (nonLimitedOutput, -1.0, 1.0);
                
                // Increment the write location and wrap around the length of the delay line
                writeLoc = (writeLoc + 1) % maxDelay;

            }
        } else {
            
            // Copy the left channel to the right channel
            for (int i = 0; i < buffer.getNumSamples(); ++i)
                output[i] = buffer.getWritePointer (channel-1)[i];
        }
    }
}

//==============================================================================
bool NordicSMC_EffectAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* NordicSMC_EffectAudioProcessor::createEditor()
{
    return new NordicSMC_EffectAudioProcessorEditor (*this);
}

//==============================================================================
void NordicSMC_EffectAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void NordicSMC_EffectAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new NordicSMC_EffectAudioProcessor();
}

// Implementation of the limiter
float NordicSMC_EffectAudioProcessor::limit (float val, float min, float max)
{
    if (val < min)
        return min;
    else if (val > max)
        return max;
    else
        return val;
}
