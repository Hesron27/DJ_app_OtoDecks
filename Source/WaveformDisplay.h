/*
  ==============================================================================

    WaveformDisplay.h
    Created: 9 Mar 2021 6:05:28pm
    Author:  Hesron

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class WaveformDisplay  : public juce::Component,
                         public juce::ChangeListener
{
public:
    //The waveformDisplay constructor definition
    WaveformDisplay(juce::AudioFormatManager& formatManagerToUse,
                    juce::AudioThumbnailCache& cachetoUse);

    ~WaveformDisplay() override;

    //The 2 functions that need to be implemented since WaveformDisplay is a component
    void paint (juce::Graphics&) override;
    void resized() override;

    //The pure virtual function that needs to be implemented since we inherit from
    // the ChangeListener class
    void changeListenerCallback (juce::ChangeBroadcaster *source) override;

    //A function that takes care of setting the position of the playhead
    void setPositionRelative(double pos);

    //A function that takes care of loading the source into an audioThumbnail
    void loadURL (juce::URL audioURL);

private:
    //An audioThumbnail that enables us to draw the waveform
    juce::AudioThumbnail audioThumb;

    //The variable used to store the position and be able to update it
    double position;

    //A bool variable to check if the file is loaded or not initialized as false
    bool fileLoaded;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveformDisplay)
};
