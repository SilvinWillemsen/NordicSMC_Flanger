/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
NordicSMC_EffectAudioProcessorEditor::NordicSMC_EffectAudioProcessorEditor (NordicSMC_EffectAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    /* Adding parameter control [2]: Set up slider
     
            A slider instance has many properties. The most important ones to set up are:
            - its range: setRange (min, max, stepSize)
            - its start value: setValue (defaultValue)
    */
    gainSlider.setRange (0, 1, 0.01);
    gainSlider.setValue (0.5, dontSendNotification);
    
    frequencySlider.setRange (20, 2000, 0.01);
    frequencySlider.setValue (440.0, dontSendNotification);

    LFOdepth.setRange (0.0, 1.0, 0.01);
    LFOdepth.setValue (0.5, dontSendNotification);

    LFOfreq.setRange (0.0, 10.0, 0.01);
    LFOfreq.setValue (2.0, dontSendNotification);
    /* Adding parameter control [3]: Add the Editor as a listener to the slider
            
            Once the slider is moved, it will send a message to all listeners, which will call their implementation of the "sliderValueChanged()" function.
     */
    gainSlider.addListener (this);
    frequencySlider.addListener (this);
    LFOdepth.addListener (this);
    LFOfreq.addListener (this);

    /* Adding parameter control [4]: Make the slider visible
     
            The "addAndMakeVisible" function adds the slider to the JUCE application. Without this, the slider can not be used
     */
    addAndMakeVisible (gainSlider);
    addAndMakeVisible (frequencySlider);
    addAndMakeVisible (LFOdepth);
    addAndMakeVisible (LFOfreq);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
}

NordicSMC_EffectAudioProcessorEditor::~NordicSMC_EffectAudioProcessorEditor()
{
}

//==============================================================================
void NordicSMC_EffectAudioProcessorEditor::paint (juce::Graphics& g)
{
}

void NordicSMC_EffectAudioProcessorEditor::resized()
{
    /* Adding parameter control [5]: Give the slider "bounds"
     
            Using the "addAndMakeVisible" function is not enough to make a slider usable in your application. It also needs to be given a location and a size in the GUI. This is done in the resized() function, which is called when a component is resized (or setSize() is called, as in the constructor of this class).
     
            To give the slider bounds, we use the "setBounds(x, y, w, h)" function, taking x-location, y-location, width and height as arguments. Here, the "getLocalBounds()" function is used. This returns a rectangle with the following properties: x = 0, y = 0, w = getWidth(), h = getHeight(). Alternatively, you can set x, y, width and height manually:
                    gainSlider.setBounds (0, 0, getWidth(), getHeight());

     */
    
    // ==== code from the workshop ==== //
//    gainSlider.setBounds (0, 0, getWidth(), getHeight() * 0.5);
//    frequencySlider.setBounds (0, getHeight() * 0.5, getWidth(), getHeight() * 0.5);
    // ================================ //
    
    /*
     A smarter way to locate your sliders in your app is to use a Rectangle<int> instance and call its removeFrom..() functions.
     */
    Rectangle<int> area = getLocalBounds();
    // We have 4 sliders the height of one is 1/4 the height of the app
    int sliderHeight = getHeight() * 0.25;
    gainSlider.setBounds (area.removeFromTop (sliderHeight));
    frequencySlider.setBounds (area.removeFromTop (sliderHeight));
    LFOfreq.setBounds (area.removeFromTop (sliderHeight));
    LFOdepth.setBounds (area.removeFromTop (sliderHeight));


}


/* Slider functionality setup [2b]: Implement the sliderValueChanged() function.
        
        Simply define the function without content in the .cpp file as:
        
        =============
 
        void NordicSMC_EffectAudioProcessorEditor::sliderValueChanged (Slider* slider)
        {
         
        }
 
        =============
 
 */
void NordicSMC_EffectAudioProcessorEditor::sliderValueChanged (Slider* slider)
{
    /* Adding parameter control [7]: Apply the slider value to the Processor parameter
     
            (Note: if you're looking for step [6], it's in the PluginProcessor.h file :) )
     
            Now that we created a function to set the gain of the Processor, let's apply the slider value to the gain.
            Obtaining the slider value happens through the Slider class' "getValue()" function.
     
     */
    if (slider == &gainSlider)
    {
        audioProcessor.setGain (gainSlider.getValue());
    }
    else if (slider == &frequencySlider)
    {
        audioProcessor.setFrequency (frequencySlider.getValue());
    }
    else if (slider == &LFOfreq)
    {
        audioProcessor.setLFOfreq (LFOfreq.getValue());
    }
    else if (slider == &LFOdepth)
    {
        audioProcessor.setLFOdepth (LFOdepth.getValue());
    }
}


