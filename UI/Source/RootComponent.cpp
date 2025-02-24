/*
  ==============================================================================

    RootComponent.cpp
    Created: 24 Jan 2025 5:10:08pm
    Author:  Yuki Sato

  ==============================================================================
*/

#include "RootComponent.h"



RootComponent::RootComponent()
{
    // コンストラクタで初期化
    //MyCustomComponentクラスのインスタンスが生成された際に呼び出される。
    //背景色の設定。子コンポーネントの追加 (addAndMakeVisibleを使う)。
}
RootComponent::~RootComponent()
{
  //MyCustomComponentクラスのインスタンスが破棄される際に呼び出される。
  //動的メモリを解放 (delete など)。
}

void RootComponent::paint(juce::Graphics& g)
{
  //コンポーネントの描画内容を定義します。
    g.fillAll(juce::Colours::lightgrey); // 背景色を設定
    g.setColour(juce::Colours::black);
    g.drawText("My Custom Component", getLocalBounds(), juce::Justification::centred, true);
}

void RootComponent::setAudioProcessor(RNBO::JuceAudioProcessor *p)
{
    processor = p;

    RNBO::ParameterInfo parameterInfo;
    RNBO::CoreObject& coreObject = processor->getRnboObject();

    for (unsigned long i = 0; i < coreObject.getNumParameters(); i++) {
      //パラメータの一意なID（名前）を取得。
        auto parameterName = coreObject.getParameterId(i);
        //パラメータの現在の値を取得。
        RNBO::ParameterValue value = coreObject.getParameterValue(i);
        Slider *slider = nullptr;
        //RNBOのパラメータID（parameterName）とスライダーの名前（juce__slider.get()->getName()）を比較。
        if (juce::String(parameterName) == juce__slider.get()->getName()) {
            slider = juce__slider.get();
        } else if (juce::String(parameterName) == juce__slider2.get()->getName()) {
            slider = juce__slider2.get();
        } else if (juce::String(parameterName) == juce__slider3.get()->getName()) {
            slider = juce__slider3.get();
        }

        if (slider) {
            slidersByParameterIndex.set(i, slider);
            coreObject.getParameterInfo(i, &parameterInfo);
            slider->setRange(parameterInfo.min, parameterInfo.max);
            slider->setValue(value);
        }
    }
}

void RootComponent::sliderValueChanged (juce::Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    if (processor == nullptr) return;
    //processorからRNBOオブジェクト（CoreObject）とパラメータリストを取得。
    //RNBO::CoreObjectはRnbo内のオーディオ処理の中心的なクラス。パラメータやそのインデックスを管理。
    RNBO::CoreObject& coreObject = processor->getRnboObject();
   //JUCEのパラメータラッパーを取得し、RNBOパラメータとの操作を簡単にする。
    auto parameters = processor->getParameters();
    //[/UsersliderValueChanged_Pre]
    //スライダーごとに異なるコードを記述できる（例: 特定のパラメータの処理）。
    if (sliderThatWasMoved == juce__slider.get())
    {
        //[UserSliderCode_juce__slider] -- add your slider handling code here..
        //[/UserSliderCode_juce__slider]
    }
    else if (sliderThatWasMoved == juce__slider2.get())
    {
        //[UserSliderCode_juce__slider2] -- add your slider handling code here..
        //[/UserSliderCode_juce__slider2]
    }
    else if (sliderThatWasMoved == juce__slider3.get())
    {
        //[UserSliderCode_juce__slider3] -- add your slider handling code here..
        //[/UserSliderCode_juce__slider3]
    }

    //[UsersliderValueChanged_Post]
    RNBO::ParameterIndex index = coreObject.getParameterIndexForID(sliderThatWasMoved->getName().toRawUTF8());
    if (index != -1) {
      //動かされたスライダーの名前（getName）に基づいて、RNBOパラメータのインデックスを取得。
        const auto param = processor->getParameters()[index];
        auto newVal = sliderThatWasMoved->getValue();

        if (param && param->getValue() != newVal)
        {
            auto normalizedValue = coreObject.convertToNormalizedParameterValue(index, newVal);
            param->beginChangeGesture();
            param->setValueNotifyingHost(normalizedValue);
            param->endChangeGesture();
        }
    }
    //[/UsersliderValueChanged_Post]
}


void RootComponent::updateSliderForParam(unsigned long index, double value)
{
    if (processor == nullptr) return;
    RNBO::CoreObject& coreObject = processor->getRnboObject();
    auto denormalizedValue = coreObject.convertFromNormalizedParameterValue(index, value);
    auto slider = slidersByParameterIndex.getReference((int) index);
    if (slider && (slider->getThumbBeingDragged() == -1)) {
        slider->setValue(denormalizedValue, NotificationType::dontSendNotification);
    }
}

void RootComponent::resized()
{
    // 子コンポーネントのサイズ設定を行う
}
