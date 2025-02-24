#include "CustomAudioEditor.h"

CustomAudioEditor::CustomAudioEditor (RNBO::JuceAudioProcessor* const p, RNBO::CoreObject& rnboObject, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (p)
    , _audioProcessor(p)
    , _rnboObject(rnboObject)      // 参照メンバーを初期化（必須）
    , valueTreeState(vts)
{

    // ダイアルの設定
    //auto paramID1 = juce::String (rnboObject.getParameterId (0));
    //auto paramID2 = juce::String (rnboObject.getParameterId (1));
    //auto paramID3 = juce::String (rnboObject.getParameterId (2));
    //auto paramName1 = juce::String(paramID1);
    //auto paramName2 = juce::String(paramID2);
    //auto paramName3 = juce::String(paramID3);

    //std::unique_ptr<juce::AudioParameterFloat> parameter1;
    //parameter1 = std::make_unique<juce::AudioParameterFloat>(paramID1, paramName1, 0.0f, 100.0f, 0.0f);

    //_audioProcessor->addParameter(parameter1);

    //parameter2 = new juce::AudioParameterFloat(paramID2, paramName2, 0.0f, 1000.0f, 5.0f);
    //_audioProcessor->addParameter(parameter2);

    //parameter3 = new juce::AudioParameterFloat(paramID3, paramName3, 0.0f, 1000.0f, 39.85f);
    //_audioProcessor->addParameter(parameter3);
     // スライダーをパラメータに接続

   
    addAndMakeVisible(dial1Slider);
    // デバッグ: vts が有効かチェック
    // デバッグ: vts の state を確認
    DBG("vts.state: " + valueTreeState.state.toXmlString());

    std::cout << "mix1" << std::endl;
    //スライダーひAPVTSのパラメータを紐づけます。
    dial1Attachment.reset (new SliderAttachment (valueTreeState, "mix", dial1Slider));
    dial1Slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    dial1Slider.setTextValueSuffix (" %");     
    dial1Slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, dial1Slider.getTextBoxWidth(), dial1Slider.getTextBoxHeight());

    //rnboObject.setParameterValue (i, apvts.getRawParameterValue (dial1));

    addAndMakeVisible(label1);
    label1.setText ("Mix", juce::dontSendNotification);
    label1.setJustificationType(juce::Justification::centred);
    
    addAndMakeVisible(dial2Slider);
    std::cout << "delayAll" << std::endl;
    dial2Attachment.reset (new SliderAttachment (valueTreeState, "delayAll", dial2Slider));
    dial2Slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    dial2Slider.setTextValueSuffix (" ms");     
    dial2Slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, dial2Slider.getTextBoxWidth(), dial2Slider.getTextBoxHeight());

  
    addAndMakeVisible(label2);
    ////All-pass Filter
    label2.setText ("Del All-Pass Filter ", juce::dontSendNotification);
    label2.setJustificationType(juce::Justification::centred);
    
    addAndMakeVisible(dial3Slider);
    std::cout << "delayCom" << std::endl;
    dial3Attachment.reset (new SliderAttachment (valueTreeState, "delayCom", dial3Slider));
    dial3Slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    dial3Slider.setTextValueSuffix (" ms");     
    dial3Slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, dial3Slider.getTextBoxWidth(), dial3Slider.getTextBoxHeight());


    addAndMakeVisible(label3);
    label3.setText ("Del Comb Filter", juce::dontSendNotification);
    label3.setJustificationType(juce::Justification::centred);
    
    _audioProcessor->addListener(this);
    setSize(400, 180);
}

CustomAudioEditor::~CustomAudioEditor()
{
    _audioProcessor->removeListener(this);
    
}

void CustomAudioEditor::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void CustomAudioEditor::resized()
{
    //四隅からマージンを設けてサイズを縮小するために使用する値
    auto border = 6;

//エディター全体の領域を取得
    auto area = getLocalBounds();

    dial1Slider.setBounds(area.removeFromLeft(area.getWidth() / 3).reduced(border));
    dial2Slider.setBounds(area.removeFromLeft(area.getWidth() / 2).reduced(border));
    dial3Slider.setBounds(area.reduced(border));

    label1.setBounds(dial1Slider.getX(), dial1Slider.getY(), dial1Slider.getWidth(),dial1Slider.getTextBoxHeight() );
    label2.setBounds(dial2Slider.getX(), dial2Slider.getY(), dial2Slider.getWidth(),dial2Slider.getTextBoxHeight() );
    label3.setBounds(dial3Slider.getX(), dial3Slider.getY(), dial3Slider.getWidth(),dial3Slider.getTextBoxHeight() );

}

//GUI側でのパラメータ変更の検知とGUIの更新
void CustomAudioEditor::audioProcessorParameterChanged (AudioProcessor*, int parameterIndex, float value)
{
    //std::cout << "ParameterIndex: " << parameterIndex << std::endl;

    // switch (parameterIndex)
    //{
    //   case 3:
    //    {

    //        dial1Slider.setValue( value * 100.0f , juce::dontSendNotification);
    //        std::cout << "Setting parameter 1 normalized value: " << value * 100.0f<< std::endl;
    //        break; // 各 case の終わりに break を追加
    //    }
    //    case 4: 
    //    {   
    //        dial2.setValue(value  * 1000.0f, juce::dontSendNotification);
    //        std::cout << "Setting parameter 2 normalized value: " << value * 1000.0f<< std::endl;
    //        break; // 各 case の終わりに break を追加
    //    }
    //    case 5:
    //    {
    //        dial3.setValue(value * 1000.0f , juce::dontSendNotification);
    //        std::cout << "Setting parameter 3 normalized value: " << value * 1000.0f<< std::endl;
    //        break; // 各 case の終わりに break を追加
    //    }
    //    default:
    //        break;
    //}
}
