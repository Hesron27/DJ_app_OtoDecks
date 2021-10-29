/*
  ==============================================================================

    PlaylistComponent.cpp
    Created: 3 Mar 2021 1:34:56pm
    Author:  Hesron

  ==============================================================================
*/

#include <JuceHeader.h>
#include "PlaylistComponent.h"
#include <iostream>

//==============================================================================
//Playlist Component constructor with all the variables initialized
// -- Audio players that are used by the playlist component to choose where to load the track
//         when the user choose to load a track from the playlist
// -- Tmp audio player is used to calculate the track length of each track while loading it from file
// -- Waveform displays are used to choose where to display the title
// -- TrackTitles objects are used to choose which display is used when a track is loaded
PlaylistComponent::PlaylistComponent(AudioPlayer* _player1, 
                                     AudioPlayer* _player2,
                                     AudioPlayer* _tmp,
                                     WaveformDisplay* _display1,
                                     WaveformDisplay* _display2,
                                     TrackTitle* _title_d1,
                                     TrackTitle* _title_d2)
    :   player1(_player1),
        player2(_player2),
        tmp_player(_tmp), 
        w_display1(_display1),
        w_display2(_display2),
        titled1(_title_d1),
        titled2(_title_d2)
{    
    //Setting up the Headers and column titles of the playlist TableListBox
    playlist.getHeader().addColumn("Track Title", 1, 200);
    playlist.getHeader().addColumn("Location", 2, 200);
    playlist.getHeader().addColumn("Length", 3, 200);
    playlist.getHeader().addColumn("", 4, 200);
    //The playlist is enabled to accept multiple selection of rows instead of 1 at a time
    playlist.setMultipleSelectionEnabled(true);

    //Making visible the playlist TableListBox, SearchField and inputText Labels
    addAndMakeVisible(playlist);    
    addAndMakeVisible(searchField);
    addAndMakeVisible(inputText); 

    //Setting this playlist component as a model to the TableListBox playlist since
    //  the playlist component is a TableListBoxModel
    playlist.setModel(this);

    //The directories of each track are read from file to be displayed by the playlist
    loadDirectories();
    //As a debug message the playlist file directory is written
    DBG("Playlist directory is " << playlistFile.getFullPathName());
}

PlaylistComponent::~PlaylistComponent()
{
    //When this component is destructed when closing the app
    //  the directories of all the tracks that are found in the playlist get written to file 
    //  using the savePlaylistToFile function
    savePlaylistToFile();
}

void PlaylistComponent::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    //Setting the inputText label to be editable so the user can enter text to search music
    inputText.setEditable(true);
    //Setting the colour of the text
    inputText.setColour(juce::Label::textColourId, juce::Colours::darkorange);
    //When user presses the return button the searchTrack function is called passing the text entered by the user
    inputText.onTextChange = [this] {searchTrack(inputText.getText()); };   

    //Below are the settings for the searchField label
    searchField.setText("Search for Music: ", juce::NotificationType::dontSendNotification);
    searchField.setFont(juce::Font(16.0f, juce::Font::bold));
    //Attaching the searchField label to go with the inputText
    searchField.attachToComponent(&inputText, true);
    searchField.setColour(juce::Label::textColourId, juce::Colours::darkorange);
    searchField.setJustificationType(juce::Justification::right);   
}

void PlaylistComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    inputText.setBounds(125, 0, getWidth() - 125, 25);
    playlist.setBounds(0, 25, getWidth(), getHeight()-50);    
}

//Implemented because it is a pure virtual function of the TableListBoxModel abstract class
int PlaylistComponent::getNumRows()
{
    return tracks.size();   
}

void PlaylistComponent::paintRowBackground(juce::Graphics& g,
    int rowNumber,
    int width,
    int height,
    bool rowIsSelected)
{
    //When a row is selected it changes colour to orange, when it is not, its colour is lightseagreen
    if (rowIsSelected == true)
    {
        g.fillAll(juce::Colours::darkorange);
    }
    else
    {
        g.fillAll(juce::Colours::lightseagreen);
    }    
}

void PlaylistComponent::paintCell(juce::Graphics& g,
    int rowNumber,
    int columnId,
    int width,
    int height,
    bool rowIsSelected)
{
    //The next file in the tracks vector is loaded into the tmp player so that the songlength
    // is extracted
    tmp_player->loadURL(juce::URL(tracks[rowNumber]));
    
    //At column 1 the file name / track title is displayed
    if (columnId == 1)
    {
        g.drawText(tracks[rowNumber].getFileNameWithoutExtension(),
            2,
            0,
            width - 4,
            height,
            juce::Justification::horizontallyCentred,
            true);
    }

    //At column 2 the location of the file is displayed
    if (columnId == 2)
    {
        g.drawText(tracks[rowNumber].getFullPathName(),
            2,
            0,
            width - 4,
            height,
            juce::Justification::horizontallyCentred,
            true);
    }

    //At column 3 the song length in minutes and seconds is displayed
    if (columnId == 3)
    {            
        g.drawText(tmp_player->getSongLength(),
            2,
            0,
            width - 4,
            height,
            juce::Justification::horizontallyCentred,
            true);
    }    
}

juce::Component* PlaylistComponent::refreshComponentForCell(int rowNumber,
    int columnId,
    bool isRowSelected,
    juce::Component* existingComponentToUpdate)
{
    //We re attaching a text button to eachcell in column 4 that enables 
    //the user to load a track into a player 
    if (columnId == 4)
    {
        if (existingComponentToUpdate == nullptr)
        {
            juce::TextButton* btn = new juce::TextButton{ "Load Track" };
            juce::String id{ std::to_string(rowNumber) };
            btn->setComponentID(id);
            btn->addListener(this);
            existingComponentToUpdate = btn;           
        }
    }
    
    return existingComponentToUpdate;
}

//This function is called whenever a button is clicked in the playlist component
//A button is passed as a parameter, whenever a button is clicked inside this component
void PlaylistComponent::buttonClicked(juce::Button* button)
{
    //The component id set in the refreshComponentForCell function is extracted from each button
    int id = std::stoi(button->getComponentID().toStdString());

    //If player 1 is playing and player 2 is not, the track is loaded into player 2
    //Also, the title is sent to the title2 pointer and same is done to the w_display2 pointer
    if (player1->transportSource.isPlaying() == true && player2->transportSource.isPlaying()==false)
    {
        player2->loadURL(juce::URL{ tracks[id] });
        titled2->setTitle(tracks[id].getFileNameWithoutExtension().toUpperCase(), player2->getSongLength());
        w_display2->loadURL(juce::URL{ tracks[id] });
    }

    //Vice versa, if player 1 is not playing and player 2 is playing, the track is loaded into player 1
    //As above, the track title and waveform display are sent to the according pointers
    if (player1->transportSource.isPlaying() == false && player2->transportSource.isPlaying() == true)
    {
        player1->loadURL(juce::URL{ tracks[id] });
        titled1->setTitle(tracks[id].getFileNameWithoutExtension().toUpperCase(), player1->getSongLength());
        w_display1->loadURL(juce::URL{ tracks[id] });
    }

    //If both players are playing, track is loaded into player 1
    if (player1->transportSource.isPlaying() == true && player2->transportSource.isPlaying() == true)
    {
        player1->loadURL(juce::URL{ tracks[id] });
        titled1->setTitle(tracks[id].getFileNameWithoutExtension().toUpperCase(), player1->getSongLength());
        w_display1->loadURL(juce::URL{ tracks[id] });
    }

    //If player 1 is not playing, track is loaded into player 1
    if (player1->transportSource.isPlaying() == false)
    {
        player1->loadURL(juce::URL{ tracks[id] });
        titled1->setTitle(tracks[id].getFileNameWithoutExtension().toUpperCase(), player1->getSongLength());
        w_display1->loadURL(juce::URL{ tracks[id] });
    }   
}

//A function that enables adding of file to the tracklist
void PlaylistComponent::addAudioFile(juce::File* audioFile)
{
    //bool variable used as a signal whether the file already exists or not
    bool alreadyExists{ false };

    for (juce::File& file : tracks)
    {
        //if the filename of the file in the tracks vector is equal to the filename of the audiofile passed
        //  the bool variable is set to true and loop exits
        if (file.getFileName().equalsIgnoreCase(audioFile->getFileName()))
        {
            alreadyExists = true;
            break;
        }
    }  

    //If alreadyExists is still false and the file's passed directory string is empty
    //the file is pushed back the vector
    //The playlist's content is updated and the paint function is recalled
    if (alreadyExists == false && audioFile->getFullPathName() != "")
    {
        tracks.push_back(*audioFile);        
        playlist.updateContent();
        playlist.repaint();
    }    
}

//A function that reads the playlist and saves the URLs to file
void PlaylistComponent::savePlaylistToFile()
{
    //A file output stream used to write to file
    juce::FileOutputStream stream{ playlistFile };

    //If the file output stream opened ok, the stream position is set at the beginning of the file
    //For each file in tracks vector the directory string is written to file added a carriage return 
    //for ease of reading when the file is read back
    if (stream.openedOk() == true)
    {     
        stream.setPosition(0);
        
        for (auto &file : tracks)
        {
            juce::String directory{ file.getFullPathName()+"\n"};
            DBG(directory);            
            stream.writeText(directory, false, false, nullptr);           
        }
        //stream is flushed to make sure that everything in the stream is written to file
        stream.flush();
    }
}

//A function to load the URLs of the files found in the playlist textfiles
void PlaylistComponent::loadDirectories()
{
    //An input stream is opened indicating the file that is used to read from
    juce::FileInputStream stream{ playlistFile };    

    //if stream opened ok, a string is created for each line and a new file
    //is created from the line read and added to the tracks vector
    if (stream.openedOk() == true)
    {      
        while (!stream.isExhausted())
        {           
            juce::String str =  stream.readNextLine();                  
            tracks.push_back( juce::File{ str });             
        }
    }
    else
    {
        DBG("INPUT STREAM failed to open");
    }
}

//A function that lets the user search for a track
void PlaylistComponent::searchTrack(juce::String text)
{
    //At the beginning of each call of this function all the rows in the playlist are deselected
    playlist.deselectAllRows();

    //Looping through each file in the tracks vector, 
    //the file name is compared with the text passed as an exact match or 
    //if it contains the text as a substring
    for (int i = 0; i < tracks.size(); ++i)
    {
        if (tracks[i].getFileNameWithoutExtension().equalsIgnoreCase(text) 
            || tracks[i].getFileNameWithoutExtension().containsIgnoreCase(text))
        {
            //if one of the above conditions return true, the respective row is set as selected
            //content is updated and repaint function is called
            playlist.selectRow(i, false, false);
            playlist.updateContent();
            playlist.repaint();
        }        
    }
}