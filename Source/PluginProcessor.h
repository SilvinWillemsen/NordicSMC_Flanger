/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class NordicSMC_EffectAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    NordicSMC_EffectAudioProcessor();
    ~NordicSMC_EffectAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    // Limiter to keep the output signal between -1 and 1. Arguments are: input value, minimum value and maximum value
    float limit (float val, float min, float max);
    
    
    /* Adding parameter control [6]: Create a "setter" function
    
            Rather than making the member variables of your class public, it is good practice to make them private and access them with public "getter/setter" functions.
     
            In order for the slider in the Editor to change the private "gain" variable of this class, a "setter" function must be created.
     */
    void setGain (double gainToSet) { gain = gainToSet; };
    void setFrequency (double freqToSet) { freq = freqToSet; };
    void setLFOfreq (double LFOfreqToSet) { freqLFO = LFOfreqToSet; };
    void setLFOdepth (double LFOdepth) { depthLFO = LFOdepth; };

        
private:
    double fs; // to be obtained from the "prepareToPlay()" function
    
    
    /* Adding a parameter [1]: Define a variable
     
            Here is where you want to add variables which you will use in the "processBlock()" function.
     */
    double gain = 0.5;
    double freq = 440.0;

    // current phase
    double curPhase = 0;
        
    // ==== Flanger variables ==== //
    std::vector<float> delayLine; // the delay line
    int maxDelay = 1000;    // maximum delay (in samples)
    double curPhaseLFO = 0; // the current phase of the LFO
    double freqLFO = 2;     // frequency of the LFO
    double depthLFO = 0.5;  // depth of the LFO (between 0 and 1)
    int writeLoc = 0;       // write location
    int readLoc = 1;        // read location
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NordicSMC_EffectAudioProcessor)
};
