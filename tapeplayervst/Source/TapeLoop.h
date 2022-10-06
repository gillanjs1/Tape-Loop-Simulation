#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class TapeLoop
{
public:
	TapeLoop();
	void setTapeLength(int loopLengthInSeconds, double sampleRate, int samplesPerBlock);
	void setUpFilter(double sampleRate);
	void toggleFilter();
	void recordToTape(const float * inputPointer, int numOfSamplesToRecord);
	void getFromTape(float * outputPointer, int numOfSamplesToGet);
	void getFromTape(float * outputPointer, int numOfSamplesToGet, double speed);
	std::vector<float> getTapeLoopData();
	void resetTapeLoopDataReadPointer();

private:

	// This is a vector containing the delay buffer data
	std::vector<float> tapeLoopData;
	float *tapeLoopDataWritePointer;
	float *tapeLoopDataReadPointer;
	const float *tapeLoopDataStartPointer;
	const float *tapeLoopDataEndPointer;
	LagrangeInterpolator lagrange;
	std::vector<float> lagrangeResults;
	std::vector<float> resultsVector;
	IIRFilter filter;

	// Disabling filter by default
	bool currentlyFiltering{ false };
	void readDirectlyFromTape(float * outputPointer, int numOfSamplesToGet);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TapeLoop)
};
