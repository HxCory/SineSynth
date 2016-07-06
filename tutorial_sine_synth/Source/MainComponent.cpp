#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"


class MainContentComponent   : public AudioAppComponent,
                               public Slider::Listener
{
public:
    MainContentComponent()
    :   currentSampleRate (0.0),
        angleDelta (0.0),
        currentPhase (-3*double_Pi/4),
        currentFrequency(346.0),
        targetFrequency(currentFrequency)
    {
        addAndMakeVisible (frequencySlider);
        frequencySlider.setRange (50.0, 5000.0);
        frequencySlider.setSkewFactorFromMidPoint (500.0); // [4]
        frequencySlider.setValue(currentFrequency, dontSendNotification);
        frequencySlider.addListener (this);
        
        
        //adds slider for phase between L/R channels
        
        addAndMakeVisible (phaseSlider);
        phaseSlider.setRange (-double_Pi, double_Pi);
        phaseSlider.setSkewFactorFromMidPoint (0.0); // [4]
        phaseSlider.setValue(currentPhase, dontSendNotification);
        phaseSlider.addListener (this);
        
        setSize (600, 100);
        setAudioChannels (0, 2); // no inputs, one output
    }
    
    ~MainContentComponent()
    {
        shutdownAudio();
    }
    
    void resized() override
    {
        frequencySlider.setBounds (10, 10, getWidth() - 20, 20);
        phaseSlider.setBounds (10, getHeight() - 30, getWidth() - 20, 20);
    }
    
    void sliderValueChanged (Slider* slider) override
    {
        if (slider == &frequencySlider)
        {
            if (currentSampleRate > 0.0)
            {
                updateAngleDelta();
                getCurrentPhase();
            }
        }
        
        else if (slider == &phaseSlider)
        {
            getCurrentPhase();
        }
    }
    
    double getCurrentPhase()
    {
        currentPhase = phaseSlider.getValue();
        return currentPhase;
        
    }
    
    void updateAngleDelta()
    {
        const double cyclesPerSample = frequencySlider.getValue() / currentSampleRate; // [2]
        angleDelta = cyclesPerSample * 2.0 * double_Pi;                                // [3]
    }
    
    void prepareToPlay (int /*samplesPerBlockExpected*/, double sampleRate) override
    {
        currentSampleRate = sampleRate;
        updateAngleDelta();
    }
    
    void releaseResources() override
    {
    }
    

    
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override
    {
        for(int j = 0; j < bufferToFill.buffer->getNumChannels(); j++)
        {
            const float level = 0.125f;
            const double localTargetFrequency = targetFrequency;
            float* const buffer = bufferToFill.buffer->getWritePointer (j, bufferToFill.startSample);
            
            if(localTargetFrequency != currentFrequency)
            {
                const double frequencyIncrement = (localTargetFrequency - currentFrequency) / bufferToFill.numSamples;
                for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
                {
                    const auto offset = j == 1
                        ? getCurrentPhase()
                        : 0.0;
                    const float currentSample = (float) std::sin (currentAngles[j] + offset);
                    currentFrequency += frequencyIncrement;
                    updateAngleDelta();
                    currentAngles[j] += angleDelta;
                    buffer[sample] = currentSample * level;
                }
                    
                currentFrequency = localTargetFrequency;
            }
                
            else{
                for(int sample = 0; sample < bufferToFill.numSamples; sample++)
                {
                    const auto offset = j == 1
                        ? getCurrentPhase()
                        : 0.0;
                 
                    const float currentSample = (float) std::sin(currentAngles[j] + offset);
                    currentAngles[j] += angleDelta;
                    buffer[sample] = currentSample * level;
                }
            }
        }
    }
    
private:
    Slider frequencySlider;
    Slider phaseSlider;
    double currentSampleRate, angleDelta, currentPhase; // [1]
    double currentFrequency, targetFrequency;
    std::vector<double> currentAngles = {0, 0};
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};

Component* createMainContentComponent()     { return new MainContentComponent(); }


#endif  // MAINCOMPONENT_H_INCLUDED
