#pragma once

#include "ProcessorBase.h"

class AddProcessor : public ProcessorBase
{
public:
    AddProcessor(std::string newUniqueName, std::vector<float> gainLevels) : ProcessorBase(newUniqueName), myGainLevels{ gainLevels } {
    }

    void prepareToPlay(double sampleRate, int samplesPerBlock) {

    }

    void processBlock(juce::AudioSampleBuffer& buffer, juce::MidiBuffer&) {
        // Buffer will be an even number of channels, assuming we're doing stereo audio.
        // Speaking in terms of zero index channels, we need to read from channels 2, 4, 6... etc
        // and add them to write channel 0.
        // Similarly we need to read from channels 3, 5, 7.. etc and add them to
        // write channel 1.

        const int numInputs = buffer.getNumChannels() / 2;

        if (numInputs != myGainLevels.size() && myGainLevels.size()!=0) {
            // The size of myGainLevels doesn't match the number of inputs,
            // and the size of myGainLevels isn't zero. If it were zero,
            // getSafeGainLevel(index) will know to use 1.0f for every gain.

            // todo: throw graceful error
            return;
        }

        const int numTimesToSum = numInputs - 1;

        const auto readptrs = buffer.getArrayOfReadPointers();
        const auto writeptrs = buffer.getArrayOfWritePointers();

        //std::cout << "numTimesToSum: " << numTimesToSum << std::endl;

        buffer.applyGain(0, 0, buffer.getNumSamples(), getSafeGainLevel(0));
        buffer.applyGain(1, 0, buffer.getNumSamples(), getSafeGainLevel(0));

        for (int sumIndex = 0; sumIndex < numTimesToSum; sumIndex++)
        {
            buffer.addFrom(0, 0, readptrs[2 + sumIndex * 2], buffer.getNumSamples(), getSafeGainLevel(sumIndex + 1));
            buffer.addFrom(1, 0, readptrs[3 + sumIndex * 2], buffer.getNumSamples(), getSafeGainLevel(sumIndex + 1));
        }
    }

    void reset() {
    };

    const juce::String getName() { return "AddProcessor"; };

private:

    std::vector<float> myGainLevels;

    float getSafeGainLevel(int index) {
        if (myGainLevels.size() == 0) {
            return 1.f;
        }
        return myGainLevels.at(index);
    }

};