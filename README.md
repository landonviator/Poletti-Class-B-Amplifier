# Poletti Class-B Amplifier
!["Waveform"](https://github.com/landonviator/Poletti-Class-B-Amplifier/blob/main/assets/waveform.png)

## ⭐ Star me on GitHub!
[![Share](https://img.shields.io/badge/share-0A66C2?logo=linkedin&logoColor=white)](https://www.linkedin.com/in/landon-viator-736bb719a/)
[![Share](https://img.shields.io/badge/share-0088CC?logo=discord&logoColor=white)](https://discord.gg/Rv5MEWgwwQ)
[![Share](https://img.shields.io/badge/share-0088CC?logo=patreon&logoColor=white)](https://www.patreon.com/ViatorDSP)

## Table of Contents
- [About](#-about)
- [Technical Details](#technical-details)
- [How to Use](#how-to-use)
- [Feedback and Contributions](#-feedback-and-contributions)
- [License](#-license)
- [Contacts](#%EF%B8%8F-contacts)

## 🚀 About
This project is a custom DSP JUCE module that implements the Poletti Class-B Amplifier, as described 
by Will Pirkle in Chapter 19 of his Tube Amp Modeling Addendum. This module provides an authentic 
emulation of the vintage amplifier design, offering rich harmonic characteristics and dynamic response.

Key Features
- Accurate Emulation: Faithfully reproduces the nonlinear behavior of the Poletti Class-B amplifier, 
capturing its distinct tonal qualities.

- Versatile Applications: Suitable for a wide range of audio processing tasks, from music production to 
sound design.

- Customizable Parameters: Allows users to adjust various parameters to tailor the sound to their 
specific needs, including drive, mix, and output level.

## 🖥️ Technical Details
Will Pirkle illustrates the signal flow of the Poletti amplifier with the following diagram:
```mermaid
graph TD;
    input((input)) --> pos[Positive Asymmetrical Waveshaper]
    input --> neg[Negative Asymmetrical Waveshaper]
    
    pos --> hpf1[HPF]
    neg --> hpf2[HPF]
    
    hpf1 --> sym1[Symmetrical Waveshaper]
    hpf2 --> sym2[Symmetrical Waveshaper]
    
    sym1 --> sum1((Σ))
    sym2 --> sum1((Σ))
    
    sum1 --> output((output))
```
The waveshaper is described by the following piece-wise function:
```cpp
if (xn <= 0.0)
  yn = numerator / (1.0f - (numerator / ln));
else
  yn = numerator / (1.0f + (numerator / lp));
  ```
Here, `lp` and `ln` are the positive and negative limit values specified in Pirkle's paper. I've 
experimented with these parameters to closely replicate the waveform produced by Pirkle's original 
implementation.

## 🔨 How to Use
This repo is a Juce module, so if you're using the Projucer, you can include this repo in the `Modules` 
tab in Projucer. Go to Modules -> click the "plus" to add -> select "Add a module from a specified folder" 
-> then navigate to path\to\repo\Poletti-Class-B-Amplifier.

As an example, here is the path on my computer:
```/Volumes/Land00m/GitHub/Poletti-Class-B-Amplifier/Poletti-Class-B-Amplifier```

Once that's included, create an instance in `PluginProcessor.h`:
```cpp
PolettiClassBAmp polettiModule;
```

Call the `prepare()` method in `PluginProcessor.cpp`, passing in a `juce::dsp::ProcessSpec`:
```cpp
void PolettiAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    
    polettiModule.prepare(spec);
}
```

Call the processBuffer() method in `PluginProcessor.cpp`, passing in the buffer:
```cpp
void PolettiAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    auto drive = JUCE_LIVE_CONSTANT(0.0f);
    auto mix = JUCE_LIVE_CONSTANT(100);
    auto output = JUCE_LIVE_CONSTANT(0.0f);
    polettiModule.setMix(mix);
    polettiModule.setDrive(drive);
    polettiModule.setOutput(output);

    polettiModule.processBuffer(buffer);
}
```

You should also update the parameters in the `processBlock()` before calling `processBuffer()`. You can
see here I don't have any automatable parameters, I just used the `JUCE_LIVE_CONSTANT` macro to quickly
test parameter updates.
