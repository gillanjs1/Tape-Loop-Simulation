#pragma once

#include "PluginProcessor.h"

class TapeTest : public UnitTest
{
public:
	TapeTest() : UnitTest("Tape testing") {}

	// Returns pointer to a fake audio buffer, with consecutive values in it.
	float* getFakeAudioBuffer(int length);

	// Returns different chunks of audio buffer, mimicking the real one a bit more
	float* getRealisticFakeAudioBuffer(int bufferLength);

	void runTest() override;

private:

	std::vector<float> fakeAudioBuffer;
	bool fakeRealisticAudioBufferSetup{false};
	int realisticTimesCalled{ 0 };
	int realisticCounter{ 0 };
	bool test_recordToTape();
	bool test_getFromTape();
	bool test_getFromTape_fast();
	bool wrapped{true};
	int bufferLength;
	TapeLoop testTape;
};

static TapeTest test;