#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TapeplayervstAudioProcessorEditor::TapeplayervstAudioProcessorEditor (TapeplayervstAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p) {
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
	setSize(300, 200);
	recButton.setButtonText("Rec");
	addAndMakeVisible(&recButton);
	recButton.onClick = [this] { startRecording(); };
	recButton.setColour(TextButton::buttonColourId, Colours::red);
	playButton.setButtonText("Play");
	addAndMakeVisible(&playButton);
	playButton.onClick = [this] { startPlayingRecording(); };
	playButton.setColour(TextButton::buttonColourId, Colours::green);
	playButton.setEnabled(false);
	stopButton.setButtonText("Stop");
	addAndMakeVisible(&stopButton);
	stopButton.onClick = [this] { stopRecordingAndPlaying(); };
	stopButton.setColour(TextButton::buttonColourId, Colours::black);
	stopButton.setEnabled(false);

	// these define the parameters of our slider object
	speedSlider.setSliderStyle(Slider::LinearHorizontal);
	speedSlider.setRange(0.2, 2.0, 0.025);
	speedSlider.setTextBoxStyle(Slider::NoTextBox, false, 90, 0);
	speedSlider.setPopupDisplayEnabled(true, false, this);
	speedSlider.setTextValueSuffix(" Speed");
	speedSlider.setValue(1.0);

	// this function adds the slider to the editor
	addAndMakeVisible(&speedSlider);

	speedSlider.addListener(this);
}

void TapeplayervstAudioProcessorEditor::startRecording() {
	playButton.setEnabled(true);
	recButton.setEnabled(false);
	stopButton.setEnabled(true);
	processor.startRecording();
}

void TapeplayervstAudioProcessorEditor::startPlayingRecording() {
	playButton.setEnabled(false);
	recButton.setEnabled(true);
	stopButton.setEnabled(true);
	processor.startPlayingRecording();
}

void TapeplayervstAudioProcessorEditor::stopRecordingAndPlaying() {
	playButton.setEnabled(true);
	recButton.setEnabled(true);
	stopButton.setEnabled(false);
	processor.stopRecordingAndPlaying();
}

void TapeplayervstAudioProcessorEditor::sliderValueChanged(Slider* slider) {
	processor.playbackSpeed = slider->getValue();
}

TapeplayervstAudioProcessorEditor::~TapeplayervstAudioProcessorEditor() {
}

//==============================================================================
void TapeplayervstAudioProcessorEditor::paint (Graphics& g) {
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
}

void TapeplayervstAudioProcessorEditor::resized() {
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
	recButton.setBounds(10, 10, getWidth() - 20, 20);
	playButton.setBounds(10, 40, getWidth() - 20, 20);
	stopButton.setBounds(10, 70, getWidth() - 20, 20);
	speedSlider.setBounds(10, 110, getWidth() - 20, 20);
}
