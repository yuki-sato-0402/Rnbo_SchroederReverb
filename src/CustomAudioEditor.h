#include "JuceHeader.h"
#include "RNBO.h"
#include "RNBO_JuceAudioProcessor.h"

class CustomAudioEditor : public AudioProcessorEditor, private AudioProcessorListener
{
public:
    CustomAudioEditor(RNBO::JuceAudioProcessor* const p, RNBO::CoreObject& rnboObject,juce::AudioProcessorValueTreeState& vts);
    ~CustomAudioEditor() override;
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

    //JUCEの AudioProcessorListener インターフェースのメソッドをオーバーライドしたもので、
    //オーディオプロセッサーに関する変更を通知するために使用されるもの
    void audioProcessorChanged (AudioProcessor*, const ChangeDetails&) override { }
    void audioProcessorParameterChanged(AudioProcessor*, int parameterIndex, float) override;

protected:
    AudioProcessor                              *_audioProcessor;
    RNBO::CoreObject&                           _rnboObject;  
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CustomAudioEditor)
};
