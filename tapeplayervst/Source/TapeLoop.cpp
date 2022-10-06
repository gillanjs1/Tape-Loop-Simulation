#include "TapeLoop.h"

// Constructor
TapeLoop::TapeLoop() {
}

void TapeLoop::setTapeLength(int loopLengthInSeconds, double sampleRate, int samplesPerBlock) {
	tapeLoopData.resize(loopLengthInSeconds * sampleRate);

	//Set a write pointer to the first element
	tapeLoopDataWritePointer = &tapeLoopData[0];
	tapeLoopDataStartPointer = &tapeLoopData[0];
	tapeLoopDataReadPointer = &tapeLoopData[0];

	tapeLoopDataEndPointer = &tapeLoopData[tapeLoopData.size() - 1];
}

void TapeLoop::setUpFilter(double sampleRate) {
	// Sample rate * 0.5 is the highest you can go
	filter.setCoefficients(IIRCoefficients::makeLowPass(sampleRate, (sampleRate * 0.5) - 1300, 0.1));
}

void TapeLoop::toggleFilter() {
	currentlyFiltering = !currentlyFiltering;
}


void TapeLoop::recordToTape(const float* inputPointer, int numOfSamplesToRecord) {
	if (tapeLoopDataWritePointer + numOfSamplesToRecord <= tapeLoopDataEndPointer) {

		// If theres enough space on our tape, whack it in.
		std::copy(inputPointer, inputPointer + numOfSamplesToRecord, tapeLoopDataWritePointer);

		// Move the write pointer so that we know where to start
		tapeLoopDataWritePointer = tapeLoopDataWritePointer + numOfSamplesToRecord;
	}
	// This triggers only when it gets right to the end apparently, and has to fill it right up
	else if (tapeLoopDataWritePointer + (numOfSamplesToRecord - 1) <= tapeLoopDataEndPointer) {
		// If theres enough space on our tape, whack it in.
		std::copy(inputPointer, inputPointer + numOfSamplesToRecord, tapeLoopDataWritePointer);

		// Move the write pointer so that we know where to start
		tapeLoopDataWritePointer = &tapeLoopData[0];
	}
	else {
		// Fill it up until the end since tapeLoopDataEndPointer points to the last element in the array, we have to add 1 to it to find the actual stop point
		const float *partialInputReadPointer = inputPointer + ((tapeLoopDataEndPointer + 1) - tapeLoopDataWritePointer);
		std::copy(inputPointer, partialInputReadPointer, tapeLoopDataWritePointer);

		// Wrap
		tapeLoopDataWritePointer = &tapeLoopData[0];
		std::copy(partialInputReadPointer, inputPointer + numOfSamplesToRecord, tapeLoopDataWritePointer);
		int numOfWrappedSamples = (inputPointer + numOfSamplesToRecord) - partialInputReadPointer;
		tapeLoopDataWritePointer = tapeLoopDataWritePointer + numOfWrappedSamples;

		// Find where we're currently at in array & wrap the pointer
		// I called it an imaginary index because it's too far: it's left the vector due to the wrapping required
		int currentImaginaryIndex = tapeLoopDataWritePointer - tapeLoopDataStartPointer;
		tapeLoopDataWritePointer = &tapeLoopData[currentImaginaryIndex % tapeLoopData.size()];
	}
}

void TapeLoop::getFromTape(float* outputPointer, int numOfSamplesToGet) {
	getFromTape(outputPointer, numOfSamplesToGet, 1.0);
}

void TapeLoop::getFromTape(float* outputPointer, int numOfSamplesToGet, double speed) {
	int actualNumberOfSamplesRequiredFromTape = (int)std::floor(numOfSamplesToGet*speed);
	resultsVector.resize(actualNumberOfSamplesRequiredFromTape);
	float *results = &resultsVector[0];

	readDirectlyFromTape(results, actualNumberOfSamplesRequiredFromTape);

	lagrangeResults.resize(numOfSamplesToGet);
	float *lagrangeResultsPointer = &lagrangeResults[0];
	lagrange.process(speed, results, lagrangeResultsPointer, numOfSamplesToGet);

	if (currentlyFiltering) {
		filter.processSamples(lagrangeResultsPointer, numOfSamplesToGet);
	}

	// Copy to output using your results - it always goes to floor when calculating buffer length
	std::copy(lagrangeResultsPointer, lagrangeResultsPointer + numOfSamplesToGet, outputPointer);
}

// Returns tapeLoopData by val. Shouldn't use other than for testing/validation purposes
std::vector<float> TapeLoop::getTapeLoopData() {
	return tapeLoopData;
}

// Returns tapeLoopDataReadPointer by val. Shouldn't use other than for testing/validation purposes
void TapeLoop::resetTapeLoopDataReadPointer() {
	tapeLoopDataReadPointer = &tapeLoopData[0];
}

// Private method for TapeLoop to use when requiring actual raw data from tape (doesn't pass through lagrange)
void TapeLoop::readDirectlyFromTape(float* outputPointer, int numOfSamplesToGet) {

	// get some stuff from tapeLoopData
	if (tapeLoopDataReadPointer + numOfSamplesToGet <= tapeLoopDataEndPointer) {

		// If theres enough space on our tape, whack it in.
		std::copy(tapeLoopDataReadPointer, tapeLoopDataReadPointer + numOfSamplesToGet, outputPointer);

		// Move the write pointer so that we know where to start
		tapeLoopDataReadPointer = tapeLoopDataReadPointer + numOfSamplesToGet;
	}
	// This triggers only when it gets right to the end, and has to fill it right up
	else if (tapeLoopDataReadPointer + (numOfSamplesToGet - 1) <= tapeLoopDataEndPointer) {

		// If theres enough space on our tape, whack it in.
		std::copy(tapeLoopDataReadPointer, tapeLoopDataReadPointer + numOfSamplesToGet, outputPointer);

		// Move the write pointer so that we know where to start
		tapeLoopDataReadPointer = &tapeLoopData[0];
	}
	else {
		// Fill it up until the end since tapeLoopDataEndPointer points to the last element in the array, we have to add 1 to it to find the actual stop point
		std::copy(tapeLoopDataReadPointer, &tapeLoopData[tapeLoopData.size() - 1], outputPointer);
		int numberOfValuesCopiedBeforeWrap = tapeLoopDataEndPointer - tapeLoopDataReadPointer;
		outputPointer = outputPointer + numberOfValuesCopiedBeforeWrap;

		// Wrap
		tapeLoopDataReadPointer = &tapeLoopData[0];
		std::copy(tapeLoopDataReadPointer, tapeLoopDataReadPointer + (numOfSamplesToGet - numberOfValuesCopiedBeforeWrap), outputPointer);
		tapeLoopDataReadPointer = tapeLoopDataReadPointer + (numOfSamplesToGet - numberOfValuesCopiedBeforeWrap);

		// Find where we're currently at in array & wrap the pointer
		// I called it an imaginary index because it's too far: it's left the vector due to the wrapping required
		int currentImaginaryIndex = tapeLoopDataReadPointer - tapeLoopDataStartPointer;
		tapeLoopDataReadPointer = &tapeLoopData[currentImaginaryIndex % tapeLoopData.size()];
	}
}