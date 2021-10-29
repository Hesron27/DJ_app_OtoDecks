/*
  ==============================================================================

    WaveformDisplay.cpp
    Created: 9 Mar 2021 6:05:28pm
    Author:  Hesron

  ==============================================================================
*/

#include <JuceHeader.h>
#include "WaveformDisplay.h"

//==============================================================================
//The constructor with the initialization list
WaveformDisplay::WaveformDisplay(juce::AudioFormatManager& formatManagerToUse,
    juce::AudioThumbnailCache& cachetoUse)
    : audioThumb(1000, formatManagerToUse, cachetoUse),
    position(0),
    fileLoaded(false)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

    //registering a change listener to the waveformDisplay object
    audioThumb.addChangeListener(this);

}

WaveformDisplay::~WaveformDisplay()
{
}

void WaveformDisplay::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

     //Drawing an outline around the component
    g.setColour (juce::Colours::darkorange);
    g.drawRect (getLocalBounds(), 1);  
    
    //Setting the colour of the waveform
    g.setColour(juce::Colours::lightseagreen);
    //if the file is loaded, the drawing of the waveform happens
    if (fileLoaded == true)
    {
        audioThumb.drawChannel(g,
            getLocalBounds(),
            0,
            audioThumb.getTotalLength(),
            0,
            1.0f);
        
        g.setColour(juce::Colours::darkorange);
        g.drawRect(position * getWidth(), 0, 5, getHeight(), 2);
    }
    //else if the file is not yet loaded,
    //the below default text is displayed
    else
    {        
        g.setFont(20.0f);
        g.drawText("File not loaded", getLocalBounds(),
            juce::Justification::centred, true);   // draw some placeholder text
    }

}

void WaveformDisplay::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

void WaveformDisplay::loadURL(juce::URL audioURL)
{
    //Clears whatever has been loaded before 
    audioThumb.clear();

    //loading the source into the audioThumb object and returns a bool,
    // indicating success or failure, stored in the fileLoaded variable
    fileLoaded = audioThumb.setSource(new juce::URLInputSource(audioURL));
}

//Whenever there is a change the waveform is repainted and updated
void WaveformDisplay::changeListenerCallback(juce::ChangeBroadcaster* source)
{    
    repaint();
}

//Updating the position of the playhead 
void WaveformDisplay::setPositionRelative(double pos)
{
    if (pos != position)
    {
        position = pos;
        repaint();
    }
}