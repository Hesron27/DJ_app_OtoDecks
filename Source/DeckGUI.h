/*
  ==============================================================================

    DeckGUI.h
    Created: 1 Mar 2021 4:27:46pm
    Author:  Hesron

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "AudioPlayer.h"
#include "PlaylistComponent.h"
#include "TrackTitle.h"#
#include "WaveformDisplay.h"

//==============================================================================
/*A class that inherits from Component, Button Listener, Slider Listener and timer classes
*/
class DeckGUI  : public juce::Component,
    public juce::Button::Listener,
    public juce::Slider::Listener,
    public juce::Timer
{
public:
    //Constructor for a Deck GUI that takes a player, a playlist component, 
    //a track title and a waveform display as parameters
    DeckGUI(AudioPlayer* player,         
        PlaylistComponent* _trackList,
        TrackTitle* _title,
        WaveformDisplay* _display);

    ~DeckGUI() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    //Needs to be implemented since we inherit from Button::Listener class
    void buttonClicked(juce::Button* button) override;

    //Implement a slider listener
    void sliderValueChanged(juce::Slider* slider) override;  

    //Implementing this function since we inherit from the timer class
    void timerCallback() override;

    //A function that takes care of the painting and graphical representation of the buttons
    void buttonsRepainting();

    //A function that takes care of the painting and graphical representation of the sliders
    void slidersRepainting();

private:
    
    //A function that perform operations on the buttons in the timercallback function
    void buttonTimerCallback();

    //Set of buttons 
    juce::TextButton playButton{" Play / Pause "};     
    juce::TextButton stopButton{ " Stop " };
    juce::TextButton loadFile{ " LOAD " };
    juce::TextButton SaveToPlaylist{ " Save to Playlist " };
    juce::TextButton rewind{ "<<" };
    juce::TextButton fforward{ ">>" };
    juce::ToggleButton loop{ "Loop" };
    juce::TextButton cue_save{ "CUE Save" };
    juce::TextButton cue_play{ "CUE Play" };

    //3 sliders for the gain, speed and position
    juce::Slider gain;
    juce::Slider speed;
    juce::Slider position;

    //A pointer to a tracklist - playlist component
    PlaylistComponent* trackList;
    
    //A pointer to a player that goes with the deck 
    AudioPlayer* player;

    //A pointer to ta track title component
    TrackTitle* title;      

    //A file variable to store the loaded file
    juce::File fileLoaded;

    //A pointer to a waveform display
    WaveformDisplay* w_display;   

    //bool variables to indicate if a track has started or not,
    //used by the play / pause button
    bool started{ false };

    //A bool variable to indicate if the cue is being used or not
    bool cue{ false };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DeckGUI)        
};
