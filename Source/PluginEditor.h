/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class NordicSMC_EffectAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                              public Slider::Listener
                                           // Slider functionality setup [1]: Inherit from Slider::Listener class

{
public:
    NordicSMC_EffectAudioProcessorEditor (NordicSMC_EffectAudioProcessor&);
    ~NordicSMC_EffectAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    /* Slider functionality setup [2a]: Override the sliderValueChanged() function.
     
            sliderValueChanged (Slider* slider) is a pure virtual function of the Slider::Listener class. This means that it needs to be overridden by any class inheriting from Slider::Listener.
     */
    void sliderValueChanged (Slider* slider) override;
    
private:
    
    // Adding parameter control [1]: Add a slider
    Slider gainSlider;
    Slider frequencySlider;
    
    Slider LFOdepth;
    Slider LFOfreq;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    NordicSMC_EffectAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NordicSMC_EffectAudioProcessorEditor)
};
