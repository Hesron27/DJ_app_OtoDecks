/*
  ==============================================================================

    DeckGUI.cpp
    Created: 1 Mar 2021 4:27:47pm
    Author:  Hesron

  ==============================================================================
*/

#include <JuceHeader.h>
#include "DeckGUI.h"

//==============================================================================
//DeckGUI constructor initialized with all the variables
DeckGUI::DeckGUI(AudioPlayer* _player,                 
                 PlaylistComponent* _trackList,
                 TrackTitle* _title,
                 WaveformDisplay* _display)
    : player(_player),    
    trackList(_trackList),
    title(_title),
    w_display(_display)
{
    //Function to make the buttons/sliders visible in the constructor of the DeckGUI
    addAndMakeVisible(playButton);    
    addAndMakeVisible(stopButton);
    addAndMakeVisible(loadFile);
    addAndMakeVisible(SaveToPlaylist);
    addAndMakeVisible(loop);
    addAndMakeVisible(rewind);
    addAndMakeVisible(fforward);
    addAndMakeVisible(cue_save);
    addAndMakeVisible(cue_play);

    //Setting the below buttons to be triggered when they are pressed down
    //as the default setting is that they are triggered when the mouse button is released
    stopButton.setTriggeredOnMouseDown(true);
    playButton.setTriggeredOnMouseDown(true);
    rewind.setTriggeredOnMouseDown(true);
    fforward.setTriggeredOnMouseDown(true);
    cue_save.setTriggeredOnMouseDown(true);
    cue_play.setTriggeredOnMouseDown(true);

    //Making visible the 3 sliders
    addAndMakeVisible(gain);
    addAndMakeVisible(speed);
    addAndMakeVisible(position);  
          
    //Register the buttons/slider to the event listener
    playButton.addListener(this);    
    stopButton.addListener(this);
    loadFile.addListener(this);
    SaveToPlaylist.addListener(this);
    rewind.addListener(this);
    fforward.addListener(this);
    cue_save.addListener(this);
    cue_play.addListener(this);

    gain.addListener(this);
    speed.addListener(this);
    position.addListener(this);
    loop.addListener(this);

    //setting the ranges for the sliders
    gain.setRange(0.0, 1.0);
    speed.setRange(0.0, 10);
    position.setRange(0.0, 1.0);   
    
    //The gain and speed sliders are changed to a rotary style slider and set the textbox below the slider
    gain.setSliderStyle(juce::Slider::Rotary);
    gain.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow,false,100,20);
    
    speed.setSliderStyle(juce::Slider::Rotary);
    speed.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 100, 20);
      
}

DeckGUI::~DeckGUI()
{
    stopTimer();
}

void DeckGUI::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));   // clear the background
        
    g.setColour(juce::Colours::darkorange);
    g.drawRect(getLocalBounds(), 1);   // draw an outline around the component
    
    //A call to the buttons and sliders painting functions
    buttonsRepainting();
    slidersRepainting();    
    
}    

void DeckGUI::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

    double rowH = 50;
    double button_width = getWidth() / 5;

    //All the elements that make up a DeckGUI component are positioned 
    rewind.setBounds(0, 0, button_width, rowH);
    playButton.setBounds(button_width, 0, button_width, rowH);
    stopButton.setBounds(button_width * 2, 0, button_width, rowH);
    fforward.setBounds(button_width * 3, 0, button_width, rowH);
    loop.setBounds(button_width * 4, 0, button_width, rowH);
    
    cue_save.setBounds(0, rowH, getWidth() / 3, rowH / 2);
    cue_play.setBounds(0, rowH+rowH/2, getWidth() / 3, rowH / 2);
    loadFile.setBounds(getWidth()/3, rowH, getWidth() / 3, rowH);
    SaveToPlaylist.setBounds(getWidth() / 3*2, rowH, getWidth() / 3, rowH);

    gain.setBounds(0, rowH*2, getWidth() / 2, rowH*2);
    speed.setBounds(getWidth() / 2, rowH*2, getWidth() / 2, rowH*2);
    position.setBounds(5, rowH*4, getWidth()-5, rowH);
}

//A function that is implemented since we inherit from Button Listener class
//  and the below function is a pure virtual function
void DeckGUI::buttonClicked(juce::Button* button)
{
    //If the play / pause button is clicked the following happens
    if (button == &playButton)
    {        
        //A check is made to see whether the player is already playing a track
        //if so, the player just stops playing
        if (player->transportSource.isPlaying())
        {
            DBG("Track paused");
            player->stop();          
        }
        else
        {
            //if the player is not playing
            DBG("Track started/resumed");
            //This is a check to see whether the track is being played 
            //for the first time or not.
            //If yes, the following settings are done to the player and sliders
            if (started == false)
            {
                gain.setValue(0.25, juce::dontSendNotification);
                player->setGain(gain.getValue());
                speed.setValue(1.00, juce::dontSendNotification);
                player->setSpeed(speed.getValue());
            }
            //player starts playing
            player->start(); 
            //the started bool variable is set to true
            started = true;
            //If the player is playing the timer is started
            if (player->transportSource.isPlaying())
            {
                startTimer(200);
            }
        }         
    }

    //If the button clicked is the stop button
    //the player is stopped and the position is set back to the beginning of the track
    //also, the started bool variable is set to false, so that when it's played again,
    //the gain and speed are set back to the defaults
    if (button == &stopButton)
    {       
        DBG("Track stopped");
        player->stop(); 
        player->setPosition(0);
        started = false;
    }

    //If the cue_play button is pressed the following gets executed
    if (button == &cue_play)
    {
        //the cue bool variable is set to true
        //the current position of the player is recorded and saved for later
        //the position of the player is set to the cue position and the player starts playing      
        cue = true;
        player->setCurrentPosition(player->transportSource.getCurrentPosition());
        player->setPosition(player->getCuePosition());
        player->start();
        //if the player is already playing the button state is set to on
        //this is used by the paint function to change its colour
        //the timer is started as well
        if (player->transportSource.isPlaying())
        {
            button->setToggleState(true, false);
            startTimer(200);
        }
    }

    //When the cue_save button is pressed the position of the player at that instance is saved
    if (button == &cue_save)
    {
        player->setCuePosition(player->transportSource.getCurrentPosition());
    }

    //if the button clicked is the Load button, a file chooser gets created and the chosen file
    //gets loaded into the player, and to the waveform display object
    //Also, the track title and song length are sent to the title object
    if (button == &loadFile)
    {
        juce::FileChooser chooser{ "Please select a file..." };
        if (chooser.browseForFileToOpen())
        {
            fileLoaded = chooser.getResult();
            //The URL of the file is passed to the loadURL function of the player and is loaded
            //  into the transportSource object of the player
            player->loadURL(juce::URL{ fileLoaded });
            //The URL of the file is passed to the loadURL function of the waveformDisplay object and is loaded
            //  into the audioThumb object 
            w_display->loadURL(juce::URL{ fileLoaded });
            //The filename and the song length of the chosen file is passed to the setTitle function found in the
            //TrackTitle class
            title->setTitle(fileLoaded.getFileNameWithoutExtension().toUpperCase(),player->getSongLength());
            
            //if the the loop toggle button is on, the player is set to loop the track
            if (loop.getToggleState() == true)
            {
                player->setLoop();
            }
        }
    }

    //Whenever the SaveToPlaylist button is pressed, the loaded track gets added to the playlist
    //  by the addAudioFile function passing the file
    if (button == &SaveToPlaylist)
    {        
        trackList->addAudioFile(&fileLoaded);
    }

    //If the toggle button loop is set to on, the player is set to loop
    //and vice versa if the toggle state is false, the player is set to not loop
    if (button == &loop)
    {
        if (loop.getToggleState() == true)
        {           
            player->setLoop();
        }
        else
        {
            player->unsetLoop();
        }
    }
}

//Implement a slider listener
void DeckGUI::sliderValueChanged(juce::Slider* slider)
{
    //Wheneve the gain slider value changes, the player's gain is changed according to the
    //slider's value
    if (slider == &gain)
    {
        player->setGain(slider->getValue());
    }

    //Wheneve the speed slider value changes, the player's speed is changed according to the
    //slider's value
    if (slider == &speed)
    {
        player->setSpeed(slider->getValue());
    }

    //Wheneve the position slider value changes the player's position along the track is changed according to the
    //slider's value. Also, the position of the playhead in the waveform display changes according to the slider's value
    if (slider == &position)
    {
        player->setPositionRelative(slider->getValue());
        w_display->setPositionRelative(player->getPositionRelative());
    }
}

//A function that gets called instead the timerCallBack function
void DeckGUI::buttonTimerCallback()
{
    //If the rewind button is down and cue is not being used,
    //Also, if the player is playing, the state of the rewind button is set to on
    //and the player rewind function gets called
    if (rewind.isDown() && cue == false)
    {   
        if (player->transportSource.isPlaying())
        {
            rewind.setToggleState(true, false);
            player->rewind();            
        }
    }
    //else the toggle state is set to false
    else
    {
        rewind.setToggleState(false, false);
    }

    //As above the same checks are done,
    //if the conditions are true, the fforward toggle state is set to true and 
    //the player forward function gets called
    if (fforward.isDown() && cue == false)
    {
        if (player->transportSource.isPlaying())
        {
            fforward.setToggleState(true, false);
            player->forward();              
        }
    }
    //else, the toggle state is set to false
    else
    {
        fforward.setToggleState(false, false);
    }
    
    //if the cue_play button is not down or released
    //cue is set to false, the player's position is set to the position 
    //that was saved earlier when the cue_play button was first pressed
    //the cue_play button toggle state is set to false
    if (!cue_play.isDown() && cue == true)
    {
        cue = false;        
        player->setPosition(player->getCurrentPosition());        
        player->stop();
        cue_play.setToggleState(false, false);        
    }    
}

//Implementing this function since we inherit from the timer class
void DeckGUI::timerCallback()
{   
    //The function gets called
    buttonTimerCallback();
    
    //Moving the position slider according to the new position of the transportSource 
    //normalized between 0 and 1
    position.setValue(player->getPositionRelative(),juce::NotificationType::dontSendNotification);
    //The playhead position is updated according to the player's relative position
    w_display->setPositionRelative(player->getPositionRelative());    
}

//Function takes care of the painting of the buttons
void DeckGUI::buttonsRepainting()
{
    //The below if conditions checks whether the mouse is over the respective button or not
    //If the mouse is over the button, it gets painted a darkcyan colour,
    //  if not, it is painted a darkslategrey colour
    if (playButton.isOver())
    {
        playButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkcyan);
    }
    else
    {
        playButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkslategrey);
    }

    if (stopButton.isOver())
    {
        stopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkcyan);
    }
    else
    {
        stopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkslategrey);
    }

    if (loadFile.isOver())
    {
        loadFile.setColour(juce::TextButton::buttonColourId, juce::Colours::darkcyan);
    }
    else
    {
        loadFile.setColour(juce::TextButton::buttonColourId, juce::Colours::darkslategrey);
    }

    if (SaveToPlaylist.isOver())
    {
        SaveToPlaylist.setColour(juce::TextButton::buttonColourId, juce::Colours::darkcyan);
    }
    else
    {
        SaveToPlaylist.setColour(juce::TextButton::buttonColourId, juce::Colours::darkslategrey);
    }

    if (rewind.isOver())
    {
        rewind.setColour(juce::TextButton::buttonColourId, juce::Colours::darkcyan);
    }
    else
    {
        rewind.setColour(juce::TextButton::buttonColourId, juce::Colours::darkslategrey);
    }

    if (fforward.isOver())
    {
        fforward.setColour(juce::TextButton::buttonColourId, juce::Colours::darkcyan);
    }
    else
    {
        fforward.setColour(juce::TextButton::buttonColourId, juce::Colours::darkslategrey);
    }

    if (cue_play.isOver())
    {
        cue_play.setColour(juce::TextButton::buttonColourId, juce::Colours::darkcyan);
    }
    else
    {
        cue_play.setColour(juce::TextButton::buttonColourId, juce::Colours::darkslategrey);
    }

    if (cue_save.isOver())
    {
        cue_save.setColour(juce::TextButton::buttonColourId, juce::Colours::darkcyan);
    }
    else
    {
        cue_save.setColour(juce::TextButton::buttonColourId, juce::Colours::darkslategrey);
    }

    //The below 3 statements sets the colour of the buttons darkorange whenever the buttons togglestates are on
    cue_play.setColour(juce::TextButton::buttonOnColourId, juce::Colours::darkorange);
    rewind.setColour(juce::TextButton::buttonOnColourId, juce::Colours::darkorange);
    fforward.setColour(juce::TextButton::buttonOnColourId, juce::Colours::darkorange);

    //This sets the colour of the tick of the loop togglebutton
    loop.getLookAndFeel().setColour(juce::ToggleButton::tickColourId, juce::Colours::orange);
}

//The function that takes care of the colours to be used by the sliders
void DeckGUI::slidersRepainting()
{    
    gain.setColour(juce::Slider::thumbColourId, juce::Colours::darkorange);
    speed.setColour(juce::Slider::thumbColourId, juce::Colours::darkorange);
    position.setColour(juce::Slider::thumbColourId, juce::Colours::darkorange);

    gain.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::lightseagreen);
    speed.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::lightseagreen);
    position.setColour(juce::Slider::trackColourId, juce::Colours::lightseagreen);
}