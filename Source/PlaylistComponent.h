/*
  ==============================================================================

    PlaylistComponent.h
    Created: 3 Mar 2021 1:34:56pm
    Author:  Hesron

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <vector>
#include <string>
#include "AudioPlayer.h"
#include "TrackTitle.h"
#include "WaveformDisplay.h"

//==============================================================================

//The playlistComponent that inherits from Component, TableListBoxModel and Button Listener classes
class PlaylistComponent : public juce::Component,
    public juce::TableListBoxModel,
    public juce::Button::Listener   
{
public:
    //Constructor for PlaylistComponent with relevant arguments
    PlaylistComponent(AudioPlayer* _player1, 
                        AudioPlayer* _player2, 
                        AudioPlayer* _tmp,
                        WaveformDisplay* _display1,
                        WaveformDisplay* _display2,
                        TrackTitle* _title_d1, 
                        TrackTitle* _title_d2);

    //Destructor
    ~PlaylistComponent() override;

    //2 functions needed as this is a component class
    void paint (juce::Graphics&) override;
    void resized() override;

    //we implement the below 3 functions since we inherit from TableListBoxModel as they are pure virtual functions
    int getNumRows() override;
    void paintRowBackground(juce::Graphics& g, 
        int rowNumber, 
        int width, 
        int height, 
        bool rowIsSelected) override;
    void paintCell(juce::Graphics& g,
        int rowNumber,
        int columnId,
        int width,
        int height,
        bool rowIsSelected) override;

    //Function used since we attach another component to a cell in the playlist TableListBox
    juce::Component* refreshComponentForCell(int rowNumber, 
        int columnId, 
        bool isRowSelected, 
        juce::Component* existingComponentToUpdate) override;

    //Needs to be implemented since we inherit from Button::Listener class
    void buttonClicked(juce::Button* button) override;

    //A function that enables adding of file to the tracklist
    void addAudioFile(juce::File* audioFile); 

    //A function that reads the playlist and saves the directories to file
    void savePlaylistToFile();

    //A function to load the directories of the files found in the playlist textfiles
    void loadDirectories();

    //A function that lets the user search for a track
    void searchTrack(juce::String text);   

private:
    //A vector of type File is used to store the tracks added to the playlist
    std::vector<juce::File> tracks;
    //TableListBox object used to display a table used for the playlist
    juce::TableListBox playlist;

    //3 pointers of type Audioplayers
    AudioPlayer* player1;
    AudioPlayer* player2;
    AudioPlayer* tmp_player;
    
    //2 pointers of type TrackTitle
    TrackTitle* titled1;
    TrackTitle* titled2; 

    //2 pointers of type WaveformDisplay
    WaveformDisplay* w_display1;
    WaveformDisplay* w_display2;

    //2 Label objects that are used to let the user search for music
    juce::Label searchField;
    juce::Label inputText;
    
    //The text file that is used to store the directories of the tracks in the playlist
    //It is passed the default directory used by the user to store application data added with the name of the file
    juce::File playlistFile{ juce::File::getSpecialLocation(
        juce::File::SpecialLocationType::userApplicationDataDirectory).getFullPathName() + "\\playlist.txt" };
        
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlaylistComponent)
};
