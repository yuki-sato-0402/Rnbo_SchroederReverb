#include "CustomAudioProcessor.h"
#include "CustomAudioEditor.h"

CustomAudioEditor::CustomAudioEditor (CustomAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), valueTreeState(vts)// 参照メンバーを初期化（必須）
{
    //Mix
    addAndMakeVisible(dial1Slider);
    // デバッグ: vts の state を確認
    DBG("vts.state: " + valueTreeState.state.toXmlString());

    std::cout << "mix1" << std::endl;
    //スライダーひAPVTSのパラメータを紐づけます。
    dial1Attachment.reset (new SliderAttachment (valueTreeState, "mix", dial1Slider));
    dial1Slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    dial1Slider.setTextValueSuffix (" %");     
    dial1Slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, dial1Slider.getTextBoxWidth(), dial1Slider.getTextBoxHeight());
    dial1Slider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::white);
    dial1Slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::deepskyblue.withAlpha(0.75f));
    dial1Slider.setColour(juce::Slider::thumbColourId , juce::Colours::deepskyblue.brighter(1.5));
    dial1Slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    dial1Slider.setColour(juce::Slider::textBoxOutlineColourId , juce::Colours::white);

    addAndMakeVisible(label1);
    label1.setText ("Mix", juce::dontSendNotification);
    label1.setJustificationType(juce::Justification::centred);
    
    //All-pass Filter
    addAndMakeVisible(dial2Slider);
    std::cout << "delayAll" << std::endl;
    dial2Attachment.reset (new SliderAttachment (valueTreeState, "delayAll", dial2Slider));
    dial2Slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    dial2Slider.setTextValueSuffix (" ms");     
    dial2Slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, dial2Slider.getTextBoxWidth(), dial2Slider.getTextBoxHeight());
    dial2Slider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::white);
    dial2Slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::deepskyblue.withAlpha(0.75f));
    dial2Slider.setColour(juce::Slider::thumbColourId , juce::Colours::deepskyblue.brighter(1.5));
    dial2Slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    dial2Slider.setColour(juce::Slider::textBoxOutlineColourId , juce::Colours::white);

    addAndMakeVisible(label2);
    label2.setText ("Del All-Pass Filter ", juce::dontSendNotification);
    label2.setJustificationType(juce::Justification::centred);
    
    //Comb Filter
    addAndMakeVisible(dial3Slider);
    std::cout << "delayCom" << std::endl;
    dial3Attachment.reset (new SliderAttachment (valueTreeState, "delayCom", dial3Slider));
    dial3Slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    dial3Slider.setTextValueSuffix (" ms");     
    dial3Slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, dial3Slider.getTextBoxWidth(), dial3Slider.getTextBoxHeight());
    dial3Slider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::white);
    dial3Slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::deepskyblue.withAlpha(0.75f));
    dial3Slider.setColour(juce::Slider::thumbColourId , juce::Colours::deepskyblue.brighter(1.5));
    dial3Slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    dial3Slider.setColour(juce::Slider::textBoxOutlineColourId , juce::Colours::white);

    addAndMakeVisible(label3);
    label3.setText ("Del Comb Filter", juce::dontSendNotification);
    label3.setJustificationType(juce::Justification::centred);
    
    setSize(400, 180);
}

void CustomAudioEditor::paint (Graphics& g)
{
    g.fillAll(juce::Colours::deepskyblue); // 背景色を設定
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
