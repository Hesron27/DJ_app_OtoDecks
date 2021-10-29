#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    // Make sure you set the size of the component after
    // you add any child components.
    setSize (800, 700);

    DBG("Height of title is: " << getHeight() / 5);

    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (2, 2);
    }

    //Adding and making visible all the objects that make up the application
    addAndMakeVisible(deck1);
    addAndMakeVisible(deck2);
    addAndMakeVisible(playlist1);
    addAndMakeVisible(title_d1);
    addAndMakeVisible(title_d2);
    addAndMakeVisible(waveformDisplay1);
    addAndMakeVisible(waveformDisplay2);

    //This lets the format manager register and learn about the formats eg. mp3 wav etc
    //without it, it will not load any file
    formatManager.registerBasicFormats();
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
//Preparing the players and the mixersource to play 
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.
    
    player1.prepareToPlay(samplesPerBlockExpected, sampleRate);
    player2.prepareToPlay(samplesPerBlockExpected, sampleRate);

    mixerSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    mixerSource.addInputSource(&player1, false);
    mixerSource.addInputSource(&player2, false);    
}

//Getting the next audio block from the buffer to play
void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    mixerSource.getNextAudioBlock(bufferToFill);    
}

//Releasing any resource used by player1, player 2 and MixerSource
void MainComponent::releaseResources()
{
    player1.releaseResources();
    player2.releaseResources();
    mixerSource.releaseResources();
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
        
}

//Setting the bounds of every object and put everything in position 
void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.

    title_d1.setBounds(0, 0, getWidth() / 2, 100);
    title_d2.setBounds(getWidth() / 2,0, getWidth() / 2, 100);
    deck1.setBounds(0, getHeight()/14+50, getWidth()/2, 250);
    deck2.setBounds(getWidth()/2, getHeight() / 14+50, getWidth()/2, 250);
    waveformDisplay1.setBounds(0, 350, getWidth() / 2, 100);
    waveformDisplay2.setBounds(getWidth()/2, 350, getWidth() / 2, 100);
    playlist1.setBounds(0, 450, getWidth(), 275);   
}


