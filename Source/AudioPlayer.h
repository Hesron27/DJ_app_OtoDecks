/*
  ==============================================================================

    AudioPlayer.h
    Created: 1 Mar 2021 4:17:53pm
    Author:  Hesron

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class AudioPlayer : public juce::AudioSource,
                    public juce::PositionableAudioSource 
{
public:

    AudioPlayer(juce::AudioFormatManager& _formatManager);
    ~AudioPlayer();

    //==============================================================================
    //The 3 functions that need to be implemented since we inherit from AudioSource
    //As they are pure virtual functions
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================

    //All the public functions needed to operate on the AudioTransportSource

    //Loading the file into the transportSource object 
    void loadURL(juce::URL audioURL);
    //Setting the gain 
    void setGain(double gain);
    //Setting the speed at which the track is played
    void setSpeed(double ratio);
    //Setting the position on the transportSource in seconds
    void setPosition(double posInSecs);
    //Setting the position in seconds relative to between 0 and 1
    void setPositionRelative(double pos);
    
    //Setting the cue position
    void setCuePosition(double cue_pos);
    //Getting the cue position back
    double getCuePosition();

    //Recording the current position of the transportSource
    void setCurrentPosition(double current_pos);
    //Getting the recorded current position
    double getCurrentPosition();
    
    //A function that return the total time of the track in minutes and seconds
    juce::String getSongLength();

    //Functions to be able to start, stop, move backwards, and move forwards
    //on the transportSource
    void start();
    void stop();
    void rewind();
    void forward();

    //Implementing the below 4 function since we inherit from PositionableAudioSource class to implement the looping function
    void setNextReadPosition(juce::int64 newPosition) override;
    juce::int64 getNextReadPosition() const override;
    juce::int64 getTotalLength() const override;
    bool isLooping() const override;

    //Functions to set the playback to loop or not to loop
    void setLoop();
    void unsetLoop();
    
    //get the relative position of the playhead
    double getPositionRelative();

    //The AudioTransportSource object used to play a file
    juce::AudioTransportSource transportSource;   

private:

    //Variable to record the cue position when the user press the cue_save button
    double cue_position;
    //Variable to record the current position of the transportSource when
    //the user presses the cue_play button so we will be able to go back to it
    //When the button is released.
    double current_position;

    //To be able to read audio from file
    juce::AudioFormatManager& formatManager;    

    //smart pointer    
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;    

    juce::ResamplingAudioSource resampleSource{ &transportSource, false, 2 };
};