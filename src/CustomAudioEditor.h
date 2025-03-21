#include "JuceHeader.h"
#include "RNBO.h"

class CustomAudioEditor : public juce::AudioProcessorEditor
{
public:
    CustomAudioEditor(CustomAudioProcessor& p, juce::AudioProcessorValueTreeState& vts);
    ~CustomAudioEditor() override = default;
    void paint (Graphics& g) override;
    void resized() override; 
    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;

private:
///コンストラクタでプロセッサ側から受け取るAPVTSの参照を格納するメンバを定義,パラメータとUIを紐づけるため。
    juce::AudioProcessorValueTreeState& valueTreeState; // ✅ 参照で保持
    juce::Slider dial1Slider;
    juce::Slider dial2Slider;
    juce::Slider dial3Slider;

    juce::Label  label1;
    juce::Label  label2;
    juce::Label  label3;

    //AudioProcessorValueTreeState::SliderAttachmentのスマートポインタ
    std::unique_ptr<SliderAttachment> dial1Attachment;
    std::unique_ptr<SliderAttachment> dial2Attachment;
    std::unique_ptr<SliderAttachment> dial3Attachment;
   
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CustomAudioEditor)
};
