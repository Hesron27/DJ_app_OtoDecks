#pragma once

#include <JuceHeader.h>
#include "DeckGUI.h"
#include "AudioPlayer.h"
#include "PlaylistComponent.h"
#include "TrackTitle.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent : public juce::AudioAppComponent                      
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override; 

private:
    //==============================================================================
    // Your private member variables go here...

    //An audioFormatManager object that is able to read the formats of file
    juce::AudioFormatManager formatManager;  

    //An audioThumbnailCache object used by the waveform display
    juce::AudioThumbnailCache thumbCache{ 100 };

    //A temporary audio player. It is used mainly by the playlist component
    //  to be able to calculate the time of each track in the playlist
    AudioPlayer tmp_player{ formatManager };

    // Initialization of a player and the Deck that goes with it
    AudioPlayer player1{ formatManager };
    DeckGUI deck1{&player1, &playlist1, &title_d1, &waveformDisplay1};

    //Initialization of another player and deck that go together
    AudioPlayer player2{ formatManager };
    DeckGUI deck2{ &player2, &playlist1, &title_d2, &waveformDisplay2};

    //2 Track title objects, one for each deck
    TrackTitle title_d1;
    TrackTitle title_d2;    

    //2 WaveformDisplays that work together with the playlist component and deckGUI
    WaveformDisplay waveformDisplay1{formatManager, thumbCache};
    WaveformDisplay waveformDisplay2{ formatManager, thumbCache };

    //A playlist component initialized with both players, the temporary player, 2 waveform displays
    //  and 2 track titles as parameters
    PlaylistComponent playlist1{&player1, 
                                    &player2, 
                                    &tmp_player, 
                                    &waveformDisplay1, 
                                    &waveformDisplay2, 
                                    &title_d1, 
                                    &title_d2};

    //The mixerAudioSource object needed to be able to play 2 tracks at the same time
    juce::MixerAudioSource mixerSource;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
