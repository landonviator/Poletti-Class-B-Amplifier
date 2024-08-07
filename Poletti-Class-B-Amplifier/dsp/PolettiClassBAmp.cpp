/*
  ==============================================================================

    PolettiClassBAmp.cpp
    Created: 6 Aug 2024 9:05:00pm
    Author:  Landon Viator

  ==============================================================================
*/

#include "PolettiClassBAmp.h"

void PolettiClassBAmp::prepare (const juce::dsp::ProcessSpec& spec)
{
    jassert (spec.sampleRate > 0);
    jassert (spec.numChannels > 0);
    
    // Initialize value smoothers
    driveParam.reset(spec.sampleRate, 0.01);
    mixParam.reset(spec.sampleRate, 0.01);
    outputParam.reset(spec.sampleRate, 0.01);

    // Initialize DC filters
    negativeDCFilter.prepare(spec);
    negativeDCFilter.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
    negativeDCFilter.setCutoffFrequency(20.0);
    positiveDCFilter.prepare(spec);
    positiveDCFilter.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
    positiveDCFilter.setCutoffFrequency(20.0);
}

void PolettiClassBAmp::reset()
{
    negativeDCFilter.reset();
    positiveDCFilter.reset();
}

// =====================================================================
// Parameter Updates
// =====================================================================
void PolettiClassBAmp::setDrive(const float newDrive)
{
    // The drive works optimally between 0 and 30. Adjust this range if needed.
    auto drive = juce::jlimit(0.0f, 30.0f, newDrive);
    driveParam.setTargetValue(juce::Decibels::decibelsToGain(drive));
}

void PolettiClassBAmp::setMix (const float newMix)
{
    // Mix should always be between 0 and 1.
    auto mix = juce::jlimit(0.0f, 100.0f, newMix * 0.01f);
    mixParam.setTargetValue(mix);
}

void PolettiClassBAmp::setOutput (const float newOutput)
{
    // The output works optimally between -60 and 60. Adjust this range if needed.
    auto volume = juce::jlimit(-60.0f, 60.0f, newOutput);
    outputParam.setTargetValue(juce::Decibels::decibelsToGain(volume));
}
