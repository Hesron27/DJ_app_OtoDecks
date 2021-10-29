/*
  ==============================================================================

    AudioPlayer.cpp
    Created: 1 Mar 2021 4:17:53pm
    Author:  Hesron

  ==============================================================================
*/

#include "AudioPlayer.h"

//Constructor for the AudioPlayer and the initialization list
AudioPlayer::AudioPlayer(juce::AudioFormatManager& _formatManager) 
    : formatManager(_formatManager),
      cue_position(0),
      current_position(0)
{

}

AudioPlayer::~AudioPlayer()
{

}

//==============================================================================
void AudioPlayer::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    //formatManager.registerBasicFormats();
    /*for (int i = 0; i < formatManager.getNumKnownFormats(); i++) {
        std::string s = formatManager.getKnownFormat(i)->getFormatName().toStdString
        ();
        std::cout << i << " " << s << std::endl;
        DBG(i);
        DBG(s);
    }*/

    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    resampleSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}
   
//Getting the next audio block to play from the buffer
void AudioPlayer::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{    
    resampleSource.getNextAudioBlock(bufferToFill);     
}

//Releasing resources
void AudioPlayer::releaseResources()
{
    transportSource.releaseResources();
    resampleSource.releaseResources();    
}

//==============================================================================

//Loading the file into the transportSource
void AudioPlayer::loadURL(juce::URL audioURL)
{
    juce::AudioFormatReader* reader = formatManager.createReaderFor(audioURL.createInputStream(false));

    if (reader != nullptr)
    {
        std::unique_ptr<juce::AudioFormatReaderSource> newSource
        (new juce::AudioFormatReaderSource(reader, true));

        transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
        readerSource.reset(newSource.release());
    }
    else
    {
        DBG("Something went wrong in the file");
    }
}

//Setting the gain of the transportSource using the function from the AudioTransportSource class
void AudioPlayer::setGain(double gain)
{
    if (gain < 0 || gain > 1.0)
    {
        DBG("DJAudioPlayer::setGain gain should be between 0 and 1");
    }
    else
    {
        transportSource.setGain(gain);
    }
}

//Setting the speed using the function from the ResamplingAudioSource class
void AudioPlayer::setSpeed(double ratio)
{
    if (ratio < 0 || ratio > 100.0)
    {
        DBG("DJAudioPlayer::setSpeed ratio should be between 0 and 100");
    }
    else
    {
        resampleSource.setResamplingRatio(ratio);
    }
}

//Setting the position of the transportSource using the function from the AudioTransportSource class
void AudioPlayer::setPosition(double posInSecs)
{
    transportSource.setPosition(posInSecs);
}

//Setting the position of the transportSource relative to range of the slider being 0 and 1
void AudioPlayer::setPositionRelative(double pos)
{
    if (pos < 0 || pos > 1.0)
    {
        DBG("DJAudioPlayer::setPositionRelative pos should be between 0 and 1");
    }
    else
    {
        double posInSecs = transportSource.getLengthInSeconds() * pos;
        setPosition(posInSecs);
    }
}

//Starting of playback
void AudioPlayer::start()
{
    transportSource.start();
}

//Stopping of the playback 
void AudioPlayer::stop()
{
    transportSource.stop();    
}

//Moving the position backwards of the transportSource
void AudioPlayer::rewind()
{
    //First a check is made to see if after moving back,
    //  the new position value is less than 0 or not 
    if (transportSource.getCurrentPosition() - 1 > 0)
    {
        transportSource.setPosition(transportSource.getCurrentPosition() - 1.5);
    }
}

//Moving the playback forward
void AudioPlayer::forward()
{
    //Calculating the last position on the transportSource - end of the track
    double last_pos{ transportSource.getLengthInSeconds() };

    //If the new calculated value is equal or greater to the last position 
    //It will not move and it will not update the position
    //It will update the position only if the new position is in range
    if (transportSource.getCurrentPosition() +0.5 != last_pos || transportSource.getCurrentPosition() +0.5 > last_pos)
    {
        transportSource.setPosition(transportSource.getCurrentPosition() + 1.5);
    }
}

//The below 4 functions are pure virtual functions that need to be implemented
//  since we inherit from the PositionableAudioSource class
void AudioPlayer::setNextReadPosition(juce::int64 newPosition)
{
    
}

juce::int64 AudioPlayer::getNextReadPosition() const
{
    return readerSource->getNextReadPosition();
}

juce::int64 AudioPlayer::getTotalLength() const
{    
    return readerSource->getTotalLength();
}

//Returning true if it is looping, or false if it is not
bool AudioPlayer::isLooping() const
{
    return false;
}

//Setting the playback to loop
void AudioPlayer::setLoop()
{
    //if the readerSource is not a null pointer, the readerSource is set to loop
    //using the function from the PositionableAudioSource class
    if (readerSource != nullptr)
    {
        readerSource->setLooping(true);
    }  
}

//Setting the playback not to loop
void AudioPlayer::unsetLoop()
{
    //if the readerSource is not a null pointer, the readerSource is set to not loop
    //using the function from the PositionableAudioSource class
    if (readerSource != nullptr)
    {
        readerSource->setLooping(false);
    }
}

//get the relative position of the playhead returned as a double
double AudioPlayer::getPositionRelative()
{
    return transportSource.getCurrentPosition() / transportSource.getLengthInSeconds();
}

//A function that returns the total time of the track in minutes and seconds as a String
juce::String AudioPlayer::getSongLength()
{
    double length_minutes{ transportSource.getLengthInSeconds() / 60 };
   
    //Variables needed to seperate the integer and fractional value of the calculated minutes
    //from the return value in seconds of the function getLengthInSeconds
    double fractional_part, integer_part;
    fractional_part = modf(length_minutes, &integer_part);

    //two variables that hold the calculated minutes and seconds
    std::string minutes{ std::to_string((int)integer_part) };
    std::string seconds{ std::to_string((int)round(fractional_part * 60)) };
     
    juce::String final_string{ minutes + " min : " + seconds + " sec" };   

    return final_string;
}

//Recording the cue position saved by the user.
//  Passed as an argument and stored in the class variable
void AudioPlayer::setCuePosition(double cue_pos)
{
    cue_position = cue_pos;
}

//Returning back the recorded cue position
double AudioPlayer::getCuePosition()
{
    return cue_position;
}

//Storing the current position of the playback
void AudioPlayer::setCurrentPosition(double current_pos)
{
    current_position = current_pos;
}

//Returning the value stored in the current position variable
double AudioPlayer::getCurrentPosition()
{
    return current_position;
}
