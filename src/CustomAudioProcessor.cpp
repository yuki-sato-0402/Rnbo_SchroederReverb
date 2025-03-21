#include "CustomAudioProcessor.h"
#include "CustomAudioEditor.h"

CustomAudioProcessor::CustomAudioProcessor() 
: AudioProcessor (BusesProperties()
                  #if ! JucePlugin_IsMidiEffect
                  #if ! JucePlugin_IsSynth
                  .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                  #endif
                  .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                  #endif
                  ),
//コンストラクタの イニシャライザリスト で初期化
parameters(*this, nullptr, juce::Identifier("PARAMETERS"),
    juce::AudioProcessorValueTreeState::ParameterLayout {
      std::make_unique<juce::AudioParameterFloat>(ParameterID { "mix",  1}, "mix",
      juce::NormalisableRange<float>(0.f, 100.f, 0.01f),0.f),
      std::make_unique<juce::AudioParameterFloat>(ParameterID { "delayAll",  1}, "delayAll",
      juce::NormalisableRange<float>(0.f, 1000.f, 0.01f),5.f),
      std::make_unique<juce::AudioParameterFloat>(ParameterID { "delayCom",  1}, "delayCom",
      juce::NormalisableRange<float>(0.f, 1000.f, 0.01f),39.85f)
    }
  )
{
 
 
  for (RNBO::ParameterIndex i = 0; i < rnboObject.getNumParameters(); ++i){
    RNBO::ParameterInfo info;
    rnboObject.getParameterInfo (i, &info);

    if (info.visible){
      auto paramID = juce::String (rnboObject.getParameterId (i));
      std::cout << "Parameter Index: " << i << std::endl;
      std::cout << "Min Value: " << info.min << std::endl;
      std::cout << "Max Value: " << info.max << std::endl;

      // Each apvts parameter id and range must be the same as the rnbo param object's.
      // If you hit this assertion then you need to fix the incorrect id in ParamIDs.h.
      jassert (parameters.getParameter (paramID) != nullptr);

      // If you hit these assertions then you need to fix the incorrect apvts
      // parameter range in createParameterLayout().
      jassert (info.min == parameters.getParameterRange (paramID).start);
      jassert (info.max == parameters.getParameterRange (paramID).end);

      apvtsParamIdToRnboParamIndex[paramID] = i;
    

    parameters.addParameterListener(paramID, this);
    rnboObject.setParameterValue(i, parameters.getRawParameterValue(paramID)->load());  // RNBO に適用
      
    } 
  }

}

void CustomAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    rnboObject.prepareToProcess (sampleRate, static_cast<size_t> (samplesPerBlock));
}
 
void CustomAudioProcessor::releaseResources()
{
}
 


void CustomAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{ 
  juce::ignoreUnused (midiMessages);
  rnboObject.process(
                      buffer.getArrayOfWritePointers(), static_cast<RNBO::Index>(buffer.getNumChannels()),
                      buffer.getArrayOfWritePointers(), static_cast<RNBO::Index>(buffer.getNumChannels()),
                      static_cast<RNBO::Index> (buffer.getNumSamples())
     );     
}

////このコールバック メソッドは、パラメータが変更されたときに AudioProcessorValueTreeStateによって呼び出されます。
void CustomAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
 rnboObject.setParameterValue (apvtsParamIdToRnboParamIndex[parameterID], newValue);
}

juce::AudioProcessorEditor* CustomAudioProcessor::createEditor()
{
    //AudioProcessorEditor側でAudioProcessorValueTreeStateにアクセスするための方法が必要。
   return new CustomAudioEditor (*this,  parameters);
    //RNBOのデフォルトエディター, 標準的なパラメータ表示, 追加のカスタマイズが限定的
  // return RNBO::JuceAudioProcessor::createEditor();
}

bool CustomAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}


const juce::String CustomAudioProcessor::getName() const{
    return "Rnbo_FourierSeriesExpansionSynthesizer";
}

bool CustomAudioProcessor::acceptsMidi() const
{
    return false;
}

bool CustomAudioProcessor::producesMidi() const 
{
    return false;
}   
 
bool CustomAudioProcessor::isMidiEffect() const
{
    return false;
}

double CustomAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}
 
int CustomAudioProcessor::getNumPrograms()
{
    return 1;   
}
 
int CustomAudioProcessor::getCurrentProgram()
{
    return 0;
}
 
void CustomAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}
 
const juce::String CustomAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}
 
void CustomAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}


void CustomAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void CustomAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}


