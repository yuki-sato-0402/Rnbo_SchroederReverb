/*
  ==============================================================================

    RootComponent.h
    Created: 24 Jan 2025 5:10:08pm
    Author:  Yuki Sato

  ==============================================================================
*/

#pragma once
#include "RNBO.h"
#include "RNBO_JuceAudioProcessor.h"
#include <JuceHeader.h>

class RootComponent : public juce::Component
{
public:
    RootComponent();
    ~RootComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    void setAudioProcessor(RNBO::JuceAudioProcessor *p);
    void updateSliderForParam(unsigned long index, double value);

private:
    std::unique_ptr<juce::Slider> juce__slider; // スライダーの宣言
    std::unique_ptr<juce::Slider> juce__slider2;
    std::unique_ptr<juce::Slider> juce__slider3;
    RNBO::JuceAudioProcessor *processor = nullptr;
    HashMap<int, Slider *> slidersByParameterIndex; // used to map parameter index to slider w
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RootComponent)
};
