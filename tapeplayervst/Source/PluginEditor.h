#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

class TapeplayervstAudioProcessorEditor  : public AudioProcessorEditor,
										   private Slider::Listener {
public:
    TapeplayervstAudioProcessorEditor (TapeplayervstAudioProcessor&);
    ~TapeplayervstAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    TapeplayervstAudioProcessor& processor;
	TextButton recButton;
	TextButton playButton;
	TextButton stopButton;
	void sliderValueChanged(Slider* slider) override; // [3]
	Slider speedSlider;
	void startRecording();
	void startPlayingRecording();
	void stopRecordingAndPlaying();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TapeplayervstAudioProcessorEditor)
};
