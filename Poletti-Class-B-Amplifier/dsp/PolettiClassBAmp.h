/*
  ==============================================================================

    PolettiClassBAmp.h
    Created: 6 Aug 2024 9:05:00pm
    Author:  Landon Viator
 
    Implemented from Will Pirkle's "Tube Amp Modeling Addendum: Chapter 19"
    see: https://willpirkle.com/special/Addendum_A19_Pirkle_v1.0.pdf

  ==============================================================================
*/

#ifndef PolettiClassBAmp_h
#define PolettiClassBAmp_h
#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

class PolettiClassBAmp
{
public:
    
    PolettiClassBAmp() = default;
    
    void prepare (const juce::dsp::ProcessSpec& spec);
    void reset();
    
    void processBuffer (juce::AudioBuffer<float>& buffer)
    {
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            auto* data = buffer.getWritePointer(channel);

            for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            {
                data[sample] = processSample(data[sample], channel);
            }
        }
    }
    
    inline float processSample(float inputSample, int channel)
    {
        // xn from the equations
        const auto input = inputSample * 0.01f * driveParam.getNextValue();

        // asymmetrical waveshaper
        auto outputPositive = processWaveshaper(input, saturation, negativeLimit, positiveLimit);
        auto outputNegative = processWaveshaper(input, saturation, positiveLimit, negativeLimit);

        // highpass for dc offset
        outputPositive = positiveDCFilter.processSample(channel, outputPositive);
        outputNegative = negativeDCFilter.processSample(channel, outputNegative);

        // symmetrical waveshaper
        outputPositive = processWaveshaper(outputPositive, saturation * 2.0f, negativePositiveLimit, negativePositiveLimit);
        outputNegative = processWaveshaper(outputNegative, saturation * 2.0f, negativePositiveLimit, negativePositiveLimit);

        // recombine
        const auto signal = (outputPositive + outputNegative) * 0.51f;
        
        // mix
        const auto blend = (1.0f - mixParam.getNextValue()) * inputSample + signal *
            outputParam.getNextValue() * mixParam.getNextValue();
        
        return blend;
    }
    
    void setDrive(float newDrive);
    void setMix (float newMix);
    void setOutput (float newOutput);
    
private:
    float negativeLimit = 0.5f;
    float positiveLimit = 23.0f;
    float negativePositiveLimit = 1.01f;
    float saturation = 10.0f;
    
    juce::SmoothedValue<float> driveParam;
    juce::SmoothedValue<float> mixParam;
    juce::SmoothedValue<float> outputParam;
    
    juce::dsp::LinkwitzRileyFilter<float> negativeDCFilter;
    juce::dsp::LinkwitzRileyFilter<float> positiveDCFilter;
    
    inline float processWaveshaper (const float xn, const float k, const float ln, const float lp)
    {
        float yn = 0.0f;
        const float numerator = k * xn;

        if (xn <= 0.0)
          yn = numerator / (1.0f - (numerator / ln));
        else
          yn = numerator / (1.0f + (numerator / lp));

        return yn;
    }
};

#endif
