#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TapeplayervstAudioProcessor::TapeplayervstAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
	runTests = false;
	#ifdef JUCE_DEBUG
		// default to running tests in debug builds
		runTests = true;
	#else
		// in release mode, default to not running tests.
		runTests = false;
	#endif

	if (runTests)
	{
		UnitTestRunner testRunner;
		testRunner.runAllTests();
	}
}

TapeplayervstAudioProcessor::~TapeplayervstAudioProcessor() {
}

//==============================================================================
const String TapeplayervstAudioProcessor::getName() const {
    return JucePlugin_Name;
}

bool TapeplayervstAudioProcessor::acceptsMidi() const {
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool TapeplayervstAudioProcessor::producesMidi() const {
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool TapeplayervstAudioProcessor::isMidiEffect() const {
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double TapeplayervstAudioProcessor::getTailLengthSeconds() const {
    return 0.0;
}

int TapeplayervstAudioProcessor::getNumPrograms() {
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int TapeplayervstAudioProcessor::getCurrentProgram() {
    return 0;
}

void TapeplayervstAudioProcessor::setCurrentProgram (int index) {
}

const String TapeplayervstAudioProcessor::getProgramName (int index) {
    return {};
}

void TapeplayervstAudioProcessor::changeProgramName (int index, const String& newName) {
}

//==============================================================================
void TapeplayervstAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock) {
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
	tape.setTapeLength(1, sampleRate, samplesPerBlock);
	tape.setUpFilter(sampleRate);
}

void TapeplayervstAudioProcessor::releaseResources() {
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool TapeplayervstAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const {
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void TapeplayervstAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages) {
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
	const int bufferLength = buffer.getNumSamples();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
	for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
        buffer.clear (i, 0, buffer.getNumSamples());
	}

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel) {
        auto* channelData = buffer.getWritePointer (channel);

		// Read pointers for buffer data - allows us to read from buffers
		const float* bufferData = buffer.getReadPointer(channel);

		// Just recording left channel for now
		if (recordToTape && channel == 0) {
			tape.recordToTape(bufferData, bufferLength);
		}
		buffer.clear(channel, 0, bufferLength);

		if (playRecording && channel == 0) {
			tape.getFromTape(channelData, bufferLength, playbackSpeed);
		}

		// Just copy to both channels for playback
		else if (playRecording && channel == 1) {
			const float* otherChannelData = buffer.getReadPointer(0);
			std::copy(otherChannelData, otherChannelData +bufferLength, channelData);
		}
    }
}

void TapeplayervstAudioProcessor::startRecording() {
	playRecording = false;
	recordToTape = true;
}

void TapeplayervstAudioProcessor::stopRecordingAndPlaying() {
	recordToTape = false;
	playRecording = false;
}

void TapeplayervstAudioProcessor::startPlayingRecording() {
	recordToTape = false;
	playRecording = true;
}

void TapeplayervstAudioProcessor::toggleFilter() {
	tape.toggleFilter();
}


//==============================================================================
bool TapeplayervstAudioProcessor::hasEditor() const {
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* TapeplayervstAudioProcessor::createEditor() {
    return new TapeplayervstAudioProcessorEditor (*this);
}

//==============================================================================
void TapeplayervstAudioProcessor::getStateInformation (MemoryBlock& destData) {
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void TapeplayervstAudioProcessor::setStateInformation (const void* data, int sizeInBytes) {
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new TapeplayervstAudioProcessor();
}
