#include "RNBO_Utils.h"
#include "RNBO.h"
#include "RNBO_TimeConverter.h"
#include "RNBO_BinaryData.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

class CustomAudioProcessor :public juce::AudioProcessor ,public juce::AudioProcessorValueTreeState::Listener{
public:
    //static CustomAudioProcessor* CreateDefault();
    CustomAudioProcessor();
    ~CustomAudioProcessor() override = default;
    // 必須の純粋仮想関数
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
   
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void parameterChanged(const juce::String& parameterID, float newValue) override;
    
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
  
    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;
    
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    RNBO::CoreObject& getRnboObject() { return rnboObject; }
private:
    juce::AudioProcessorValueTreeState parameters;  
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CustomAudioProcessor)

    std::unordered_map<juce::String, RNBO::ParameterIndex> apvtsParamIdToRnboParamIndex;

    RNBO::CoreObject rnboObject;
};

