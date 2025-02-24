#include "CustomAudioProcessor.h"
#include "CustomAudioEditor.h"
#include <json/json.hpp>

#ifdef RNBO_INCLUDE_DESCRIPTION_FILE
#include <rnbo_description.h>
#endif

//create an instance of our custom plugin, optionally set description, presets and binary data (datarefs)
CustomAudioProcessor* CustomAudioProcessor::CreateDefault() {
	nlohmann::json patcher_desc, presets;

#ifdef RNBO_BINARY_DATA_STORAGE_NAME
	extern RNBO::BinaryDataImpl::Storage RNBO_BINARY_DATA_STORAGE_NAME;
	RNBO::BinaryDataImpl::Storage dataStorage = RNBO_BINARY_DATA_STORAGE_NAME;
#else
	RNBO::BinaryDataImpl::Storage dataStorage;
#endif
	RNBO::BinaryDataImpl data(dataStorage);

#ifdef RNBO_INCLUDE_DESCRIPTION_FILE
	patcher_desc = RNBO::patcher_description;
	presets = RNBO::patcher_presets;
#endif
  return new CustomAudioProcessor(patcher_desc, presets, data);
}

CustomAudioProcessor::CustomAudioProcessor(
    const nlohmann::json& patcher_desc,
    const nlohmann::json& presets,
    const RNBO::BinaryData& data
    ) 
  : RNBO::JuceAudioProcessor(patcher_desc, presets, data) 
{
  
  parameters = std::make_unique<juce::AudioProcessorValueTreeState>(
    *this, nullptr, juce::Identifier("APVTSTutorial"),
    juce::AudioProcessorValueTreeState::ParameterLayout {
        std::make_unique<juce::AudioParameterFloat>(ParameterID { "mix",  1}, "mix",
        juce::NormalisableRange<float>(0.f, 100.f, 0.01f),0.f),
        std::make_unique<juce::AudioParameterFloat>(ParameterID { "delayAll",  1}, "delayAll",
        juce::NormalisableRange<float>(0.f, 1000.f, 0.01f),5.f),
        std::make_unique<juce::AudioParameterFloat>(ParameterID { "delayCom",  1}, "delayCom",
        juce::NormalisableRange<float>(0.f, 1000.f, 0.01f),39.85f)
    }
  );
  if (!parameters){
        DBG("ERROR: parameters is nullptr!");
        jassertfalse;
  }else{
        DBG("parameters successfully created!");
        DBG("parameters state: " + parameters->state.toXmlString()); // 追加
  }
 
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
      jassert (parameters->getParameter (paramID) != nullptr);

      // If you hit these assertions then you need to fix the incorrect apvts
      // parameter range in createParameterLayout().
      jassert (info.min == parameters->getParameterRange (paramID).start);
      jassert (info.max == parameters->getParameterRange (paramID).end);
    
      if (paramID == "mix") {
          // パラメータのポインタを取得
        MixParameter = parameters->getRawParameterValue("mix");
        parameters->addParameterListener("mix", this);
        rnboObject.setParameterValue(i, *MixParameter);  // RNBO に適用
      }else if(paramID == "delayAll") {
        // パラメータのポインタを取得
        delayAllParameter = parameters->getRawParameterValue("delayAll");
        parameters->addParameterListener("delayAll", this);
        rnboObject.setParameterValue(i, *delayAllParameter);  // RNBO に適用
      }else{
        delayComParameter = parameters->getRawParameterValue("delayCom");
        parameters->addParameterListener("delayCom", this);
        rnboObject.setParameterValue(i, *delayComParameter);  // RNBO に適用
      }


    } 
  }
  
}

//このコールバック メソッドは、パラメータが変更されたときに AudioProcessorValueTreeStateによって呼び出されます。
void CustomAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
    if (parameterID == "mix"){
      RNBO::ParameterIndex index = rnboObject.getParameterIndexForID("mix");
      rnboObject.setParameterValue(index, newValue);
    }else if(parameterID == "delayAll"){
      RNBO::ParameterIndex index = rnboObject.getParameterIndexForID("delayAll");
      rnboObject.setParameterValue(index, newValue);
    }else{
      RNBO::ParameterIndex index = rnboObject.getParameterIndexForID("delayCom");
      rnboObject.setParameterValue(index, newValue);
    }
}



CustomAudioProcessor::~CustomAudioProcessor()
{
    // リスナーを必ず削除
    if (parameters != nullptr)
    {
        parameters->removeParameterListener("mix", this);
        parameters->removeParameterListener("delayAll", this);
        parameters->removeParameterListener("delayCom", this);
    }
}

void CustomAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters->copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void CustomAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters->state.getType()))
            parameters->replaceState(juce::ValueTree::fromXml(*xmlState));
}


AudioProcessorEditor* CustomAudioProcessor::createEditor()
{

    //AudioProcessorEditor側でAudioProcessorValueTreeStateにアクセスするための方法が必要です。
    //一般的なアプローチは、AudioProcessorからAudioProcessorValueTreeStateへの参照またはポインタを取得できるようにすること
    return new CustomAudioEditor (this, this->_rnboObject, *parameters);
    //RNBOのデフォルトエディター, 標準的なパラメータ表示, 追加のカスタマイズが限定的
    //return RNBO::JuceAudioProcessor::createEditor();
}



