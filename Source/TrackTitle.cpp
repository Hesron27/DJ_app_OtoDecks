/*
  ==============================================================================

    TrackTitle.cpp
    Created: 6 Mar 2021 3:28:07pm
    Author:  Hesron

  ==============================================================================
*/

#include <JuceHeader.h>
#include "TrackTitle.h"

//==============================================================================
TrackTitle::TrackTitle()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    
    //Adding and making visible the title
    addAndMakeVisible(title);

    //Settings for the title and a default text set to be "TRACK TITLE"
    title.setJustification(juce::Justification::centred);
    title.setFontHeight(20.0f);
    title.setText("TRACK TITLE");
    title.setColour(juce::Colours::darkorange);
     
}

TrackTitle::~TrackTitle()
{
}

void TrackTitle::paint (juce::Graphics& g)
{   
    //setting the titlebox to be the displayTitle rectangle
    titleBox = displayTitle;
    title.setBoundingBox(titleBox);

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    //Filling the background with a beige colour and giving a lightseagreen outline
    g.fillAll(juce::Colours::beige);
    g.setColour(juce::Colours::lightseagreen);
    g.drawRect (getLocalBounds(), 5);   // draw an outline around the component   
   
}

void TrackTitle::resized()
{
    //Setting the bounds of the displayTitle rectangle to be the bounds of the component
    //These values are taken from the getLocalBounds() function. 
    //These values are set in the setBounds() function in the MainComponent resize() function
    displayTitle.setWidth(getLocalBounds().toFloat().getWidth());
    displayTitle.setHeight(getLocalBounds().toFloat().getHeight());
}

//A function to set the text to be displayed as a title of the Deck 
//As a default, it is "Track Title" and when a track is loaded from file or from the playlist,
//  it is updated with the track name and length of the song in minutes and seconds
void TrackTitle::setTitle(juce::String _title, juce::String _length)
{    
    title.setText(_title + "\n" + _length);
}
