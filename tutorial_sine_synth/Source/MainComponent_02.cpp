#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"


class MainContentComponent   : public AudioAppComponent,
                               public Slider::Listener
{
public:
    MainContentComponent()
    :   currentSampleRate (0.0),
        currentAngle (0.0),
        angleDelta (0.0),
        currentFrequency (500.0),                                           // [6]
        targetFrequency (currentFrequency)                                  // [7]
    {
        addAndMakeVisible (frequencySlider);
        frequencySlider.setRange (50.0, 5000.0);
        frequencySlider.setSkewFactorFromMidPoint (500.0);
        frequencySlider.setValue (currentFrequency, dontSendNotification);  // [8]
        frequencySlider.addListener (this);
        
        setSize (600, 100);
        setAudioChannels (0, 1); // no inputs, one output
    }
    
    ~MainContentComponent()
    {
        shutdownAudio();
    }
    
    void resized() override
    {
        frequencySlider.setBounds (10, 10, getWidth() - 20, 20);
    }
    
    void sliderValueChanged (Slider* slider) override
    {
        if (slider == &frequencySlider)
            targetFrequency = frequencySlider.getValue();
    }
    
    inline void updateAngleDelta()
    {
        const double cyclesPerSample = currentFrequency / currentSampleRate;
        angleDelta = cyclesPerSample * 2.0 * double_Pi;
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
        const float level = 0.125f;
        float* const buffer = bufferToFill.buffer->getWritePointer (0, bufferToFill.startSample);
        
        const double localTargetFrequency = targetFrequency;
        
        if (localTargetFrequency != currentFrequency)                                                              // [9]
        {
            const double frequencyIncrement = (localTargetFrequency - currentFrequency) / bufferToFill.numSamples; // [10]
            
            for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
            {
                const float currentSample = (float) std::sin (currentAngle);
                currentFrequency += frequencyIncrement;                                                       // [11]
                updateAngleDelta();                                                                           // [12]
                currentAngle += angleDelta;
                buffer[sample] = currentSample * level;
            }
            
            currentFrequency = localTargetFrequency;
        }
        else                                                                                                  // [13]
        {
            for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
            {
                const float currentSample = (float) std::sin (currentAngle);
                currentAngle += angleDelta;
                buffer[sample] = currentSample * level;
            }
        }
    }
    
    
private:
    Slider frequencySlider;
    double currentSampleRate, currentAngle, angleDelta;
    double currentFrequency, targetFrequency;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};

Component* createMainContentComponent()     { return new MainContentComponent(); }


#endif  // MAINCOMPONENT_H_INCLUDED
