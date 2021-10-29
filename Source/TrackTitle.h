/*
  ==============================================================================

    TrackTitle.h
    Created: 6 Mar 2021 3:28:07pm
    Author:  Hesron

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class TrackTitle : public juce::Component

{
public:
    TrackTitle();
    ~TrackTitle() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    //A function to set the text to be displayed as a title of 
    void setTitle(juce::String _title, juce::String _length);

private:

    //A DrawableText variable
    juce::DrawableText title;
    
    //A rectangle and a parallelogram that are needed to draw the bounds of the title text
    juce::Rectangle<float> displayTitle{ 0,0,0,0 };
    juce::Parallelogram<float> titleBox;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackTitle)
};
