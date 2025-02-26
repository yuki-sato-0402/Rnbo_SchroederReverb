/*******************************************************************************************************************
Copyright (c) 2023 Cycling '74

The code that Max generates automatically and that end users are capable of
exporting and using, and any associated documentation files (the “Software”)
is a work of authorship for which Cycling '74 is the author and owner for
copyright purposes.

This Software is dual-licensed either under the terms of the Cycling '74
License for Max-Generated Code for Export, or alternatively under the terms
of the General Public License (GPL) Version 3. You may use the Software
according to either of these licenses as it is most appropriate for your
project on a case-by-case basis (proprietary or not).

A) Cycling '74 License for Max-Generated Code for Export

A license is hereby granted, free of charge, to any person obtaining a copy
of the Software (“Licensee”) to use, copy, modify, merge, publish, and
distribute copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions:

The Software is licensed to Licensee for all uses that do not include the sale,
sublicensing, or commercial distribution of software that incorporates this
source code. This means that the Licensee is free to use this software for
educational, research, and prototyping purposes, to create musical or other
creative works with software that incorporates this source code, or any other
use that does not constitute selling software that makes use of this source
code. Commercial distribution also includes the packaging of free software with
other paid software, hardware, or software-provided commercial services.

For entities with UNDER $200k in annual revenue or funding, a license is hereby
granted, free of charge, for the sale, sublicensing, or commercial distribution
of software that incorporates this source code, for as long as the entity's
annual revenue remains below $200k annual revenue or funding.

For entities with OVER $200k in annual revenue or funding interested in the
sale, sublicensing, or commercial distribution of software that incorporates
this source code, please send inquiries to licensing@cycling74.com.

The above copyright notice and this license shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Please see
https://support.cycling74.com/hc/en-us/articles/10730637742483-RNBO-Export-Licensing-FAQ
for additional information

B) General Public License Version 3 (GPLv3)
Details of the GPLv3 license can be found at: https://www.gnu.org/licenses/gpl-3.0.html
*******************************************************************************************************************/

#include "RNBO_Common.h"
#include "RNBO_AudioSignal.h"

namespace RNBO {


#define trunc(x) ((Int)(x))

#if defined(__GNUC__) || defined(__clang__)
    #define RNBO_RESTRICT __restrict__
#elif defined(_MSC_VER)
    #define RNBO_RESTRICT __restrict
#endif

#define FIXEDSIZEARRAYINIT(...) { }

class rnbomatic : public PatcherInterfaceImpl {
public:

rnbomatic()
{
}

~rnbomatic()
{
}

rnbomatic* getTopLevelPatcher() {
    return this;
}

void cancelClockEvents()
{
}

template <typename T> void listquicksort(T& arr, T& sortindices, Int l, Int h, bool ascending) {
    if (l < h) {
        Int p = (Int)(this->listpartition(arr, sortindices, l, h, ascending));
        this->listquicksort(arr, sortindices, l, p - 1, ascending);
        this->listquicksort(arr, sortindices, p + 1, h, ascending);
    }
}

template <typename T> Int listpartition(T& arr, T& sortindices, Int l, Int h, bool ascending) {
    number x = arr[(Index)h];
    Int i = (Int)(l - 1);

    for (Int j = (Int)(l); j <= h - 1; j++) {
        bool asc = (bool)((bool)(ascending) && arr[(Index)j] <= x);
        bool desc = (bool)((bool)(!(bool)(ascending)) && arr[(Index)j] >= x);

        if ((bool)(asc) || (bool)(desc)) {
            i++;
            this->listswapelements(arr, i, j);
            this->listswapelements(sortindices, i, j);
        }
    }

    i++;
    this->listswapelements(arr, i, h);
    this->listswapelements(sortindices, i, h);
    return i;
}

template <typename T> void listswapelements(T& arr, Int a, Int b) {
    auto tmp = arr[(Index)a];
    arr[(Index)a] = arr[(Index)b];
    arr[(Index)b] = tmp;
}

number maximum(number x, number y) {
    return (x < y ? y : x);
}

number samplerate() {
    return this->sr;
}

inline number linearinterp(number frac, number x, number y) {
    return x + (y - x) * frac;
}

inline number cubicinterp(number a, number w, number x, number y, number z) {
    number a2 = a * a;
    number f0 = z - y - w + x;
    number f1 = w - x - f0;
    number f2 = y - w;
    number f3 = x;
    return f0 * a * a2 + f1 * a2 + f2 * a + f3;
}

inline number splineinterp(number a, number w, number x, number y, number z) {
    number a2 = a * a;
    number f0 = -0.5 * w + 1.5 * x - 1.5 * y + 0.5 * z;
    number f1 = w - 2.5 * x + 2 * y - 0.5 * z;
    number f2 = -0.5 * w + 0.5 * y;
    return f0 * a * a2 + f1 * a2 + f2 * a + x;
}

inline number cosT8(number r) {
    number t84 = 56.0;
    number t83 = 1680.0;
    number t82 = 20160.0;
    number t81 = 2.4801587302e-05;
    number t73 = 42.0;
    number t72 = 840.0;
    number t71 = 1.9841269841e-04;

    if (r < 0.785398163397448309615660845819875721 && r > -0.785398163397448309615660845819875721) {
        number rr = r * r;
        return 1.0 - rr * t81 * (t82 - rr * (t83 - rr * (t84 - rr)));
    } else if (r > 0.0) {
        r -= 1.57079632679489661923132169163975144;
        number rr = r * r;
        return -r * (1.0 - t71 * rr * (t72 - rr * (t73 - rr)));
    } else {
        r += 1.57079632679489661923132169163975144;
        number rr = r * r;
        return r * (1.0 - t71 * rr * (t72 - rr * (t73 - rr)));
    }
}

inline number cosineinterp(number frac, number x, number y) {
    number a2 = (1.0 - this->cosT8(frac * 3.14159265358979323846)) / (number)2.0;
    return x * (1.0 - a2) + y * a2;
}

number mstosamps(MillisecondTime ms) {
    return ms * this->sr * 0.001;
}

Index vectorsize() {
    return this->vs;
}

inline number safediv(number num, number denom) {
    return (denom == 0.0 ? 0.0 : num / denom);
}

number safepow(number base, number exponent) {
    return fixnan(rnbo_pow(base, exponent));
}

number scale(
    number x,
    number lowin,
    number hiin,
    number lowout,
    number highout,
    number pow
) {
    auto inscale = this->safediv(1., hiin - lowin);
    number outdiff = highout - lowout;
    number value = (x - lowin) * inscale;

    if (pow != 1) {
        if (value > 0)
            value = this->safepow(value, pow);
        else
            value = -this->safepow(-value, pow);
    }

    value = value * outdiff + lowout;
    return value;
}

MillisecondTime currenttime() {
    return this->_currentTime;
}

number tempo() {
    return this->getTopLevelPatcher()->globaltransport_getTempo(this->currenttime());
}

number mstobeats(number ms) {
    return ms * this->tempo() * 0.008 / (number)480;
}

MillisecondTime sampstoms(number samps) {
    return samps * 1000 / this->sr;
}

Index getNumMidiInputPorts() const {
    return 0;
}

void processMidiEvent(MillisecondTime , int , ConstByteArray , Index ) {}

Index getNumMidiOutputPorts() const {
    return 0;
}

void process(
    const SampleValue * const* inputs,
    Index numInputs,
    SampleValue * const* outputs,
    Index numOutputs,
    Index n
) {
    this->vs = n;
    this->updateTime(this->getEngine()->getCurrentTime());
    SampleValue * out1 = (numOutputs >= 1 && outputs[0] ? outputs[0] : this->dummyBuffer);
    SampleValue * out2 = (numOutputs >= 2 && outputs[1] ? outputs[1] : this->dummyBuffer);
    const SampleValue * in1 = (numInputs >= 1 && inputs[0] ? inputs[0] : this->zeroBuffer);
    const SampleValue * in2 = (numInputs >= 2 && inputs[1] ? inputs[1] : this->zeroBuffer);

    this->gen_01_perform(
        in1,
        in2,
        this->gen_01_delayAll,
        this->gen_01_delayCom,
        this->gen_01_mix,
        out1,
        out2,
        n
    );

    this->scopetilde_01_perform(out1, this->zeroBuffer, n);
    this->stackprotect_perform(n);
    this->globaltransport_advance();
    this->audioProcessSampleCount += this->vs;
}

void prepareToProcess(number sampleRate, Index maxBlockSize, bool force) {
    if (this->maxvs < maxBlockSize || !this->didAllocateSignals) {
        this->globaltransport_tempo = resizeSignal(this->globaltransport_tempo, this->maxvs, maxBlockSize);
        this->globaltransport_state = resizeSignal(this->globaltransport_state, this->maxvs, maxBlockSize);
        this->zeroBuffer = resizeSignal(this->zeroBuffer, this->maxvs, maxBlockSize);
        this->dummyBuffer = resizeSignal(this->dummyBuffer, this->maxvs, maxBlockSize);
        this->didAllocateSignals = true;
    }

    const bool sampleRateChanged = sampleRate != this->sr;
    const bool maxvsChanged = maxBlockSize != this->maxvs;
    const bool forceDSPSetup = sampleRateChanged || maxvsChanged || force;

    if (sampleRateChanged || maxvsChanged) {
        this->vs = maxBlockSize;
        this->maxvs = maxBlockSize;
        this->sr = sampleRate;
        this->invsr = 1 / sampleRate;
    }

    this->gen_01_dspsetup(forceDSPSetup);
    this->scopetilde_01_dspsetup(forceDSPSetup);
    this->globaltransport_dspsetup(forceDSPSetup);

    if (sampleRateChanged)
        this->onSampleRateChanged(sampleRate);
}

void setProbingTarget(MessageTag id) {
    switch (id) {
    default:
        {
        this->setProbingIndex(-1);
        break;
        }
    }
}

void setProbingIndex(ProbingIndex ) {}

Index getProbingChannels(MessageTag outletId) const {
    RNBO_UNUSED(outletId);
    return 0;
}

DataRef* getDataRef(DataRefIndex index)  {
    switch (index) {
    case 0:
        {
        return addressOf(this->gen_01_del_9_bufferobj);
        break;
        }
    case 1:
        {
        return addressOf(this->gen_01_del_8_bufferobj);
        break;
        }
    case 2:
        {
        return addressOf(this->gen_01_del_7_bufferobj);
        break;
        }
    case 3:
        {
        return addressOf(this->gen_01_del1_bufferobj);
        break;
        }
    case 4:
        {
        return addressOf(this->gen_01_del_6_bufferobj);
        break;
        }
    case 5:
        {
        return addressOf(this->gen_01_del_5_bufferobj);
        break;
        }
    case 6:
        {
        return addressOf(this->gen_01_del_4_bufferobj);
        break;
        }
    case 7:
        {
        return addressOf(this->gen_01_del_3_bufferobj);
        break;
        }
    case 8:
        {
        return addressOf(this->gen_01_del_2_bufferobj);
        break;
        }
    case 9:
        {
        return addressOf(this->gen_01_del_1_bufferobj);
        break;
        }
    case 10:
        {
        return addressOf(this->gen_01_del3_bufferobj);
        break;
        }
    case 11:
        {
        return addressOf(this->gen_01_del2_bufferobj);
        break;
        }
    default:
        {
        return nullptr;
        }
    }
}

DataRefIndex getNumDataRefs() const {
    return 12;
}

void fillDataRef(DataRefIndex , DataRef& ) {}

void zeroDataRef(DataRef& ref) {
    ref->setZero();
}

void processDataViewUpdate(DataRefIndex index, MillisecondTime time) {
    this->updateTime(time);

    if (index == 0) {
        this->gen_01_del_9_buffer = new Float64Buffer(this->gen_01_del_9_bufferobj);
    }

    if (index == 1) {
        this->gen_01_del_8_buffer = new Float64Buffer(this->gen_01_del_8_bufferobj);
    }

    if (index == 2) {
        this->gen_01_del_7_buffer = new Float64Buffer(this->gen_01_del_7_bufferobj);
    }

    if (index == 3) {
        this->gen_01_del1_buffer = new Float64Buffer(this->gen_01_del1_bufferobj);
    }

    if (index == 4) {
        this->gen_01_del_6_buffer = new Float64Buffer(this->gen_01_del_6_bufferobj);
    }

    if (index == 5) {
        this->gen_01_del_5_buffer = new Float64Buffer(this->gen_01_del_5_bufferobj);
    }

    if (index == 6) {
        this->gen_01_del_4_buffer = new Float64Buffer(this->gen_01_del_4_bufferobj);
    }

    if (index == 7) {
        this->gen_01_del_3_buffer = new Float64Buffer(this->gen_01_del_3_bufferobj);
    }

    if (index == 8) {
        this->gen_01_del_2_buffer = new Float64Buffer(this->gen_01_del_2_bufferobj);
    }

    if (index == 9) {
        this->gen_01_del_1_buffer = new Float64Buffer(this->gen_01_del_1_bufferobj);
    }

    if (index == 10) {
        this->gen_01_del3_buffer = new Float64Buffer(this->gen_01_del3_bufferobj);
    }

    if (index == 11) {
        this->gen_01_del2_buffer = new Float64Buffer(this->gen_01_del2_bufferobj);
    }
}

void initialize() {
    this->gen_01_del_9_bufferobj = initDataRef("gen_01_del_9_bufferobj", true, nullptr, "buffer~");
    this->gen_01_del_8_bufferobj = initDataRef("gen_01_del_8_bufferobj", true, nullptr, "buffer~");
    this->gen_01_del_7_bufferobj = initDataRef("gen_01_del_7_bufferobj", true, nullptr, "buffer~");
    this->gen_01_del1_bufferobj = initDataRef("gen_01_del1_bufferobj", true, nullptr, "buffer~");
    this->gen_01_del_6_bufferobj = initDataRef("gen_01_del_6_bufferobj", true, nullptr, "buffer~");
    this->gen_01_del_5_bufferobj = initDataRef("gen_01_del_5_bufferobj", true, nullptr, "buffer~");
    this->gen_01_del_4_bufferobj = initDataRef("gen_01_del_4_bufferobj", true, nullptr, "buffer~");
    this->gen_01_del_3_bufferobj = initDataRef("gen_01_del_3_bufferobj", true, nullptr, "buffer~");
    this->gen_01_del_2_bufferobj = initDataRef("gen_01_del_2_bufferobj", true, nullptr, "buffer~");
    this->gen_01_del_1_bufferobj = initDataRef("gen_01_del_1_bufferobj", true, nullptr, "buffer~");
    this->gen_01_del3_bufferobj = initDataRef("gen_01_del3_bufferobj", true, nullptr, "buffer~");
    this->gen_01_del2_bufferobj = initDataRef("gen_01_del2_bufferobj", true, nullptr, "buffer~");
    this->assign_defaults();
    this->setState();
    this->gen_01_del_9_bufferobj->setIndex(0);
    this->gen_01_del_9_buffer = new Float64Buffer(this->gen_01_del_9_bufferobj);
    this->gen_01_del_8_bufferobj->setIndex(1);
    this->gen_01_del_8_buffer = new Float64Buffer(this->gen_01_del_8_bufferobj);
    this->gen_01_del_7_bufferobj->setIndex(2);
    this->gen_01_del_7_buffer = new Float64Buffer(this->gen_01_del_7_bufferobj);
    this->gen_01_del1_bufferobj->setIndex(3);
    this->gen_01_del1_buffer = new Float64Buffer(this->gen_01_del1_bufferobj);
    this->gen_01_del_6_bufferobj->setIndex(4);
    this->gen_01_del_6_buffer = new Float64Buffer(this->gen_01_del_6_bufferobj);
    this->gen_01_del_5_bufferobj->setIndex(5);
    this->gen_01_del_5_buffer = new Float64Buffer(this->gen_01_del_5_bufferobj);
    this->gen_01_del_4_bufferobj->setIndex(6);
    this->gen_01_del_4_buffer = new Float64Buffer(this->gen_01_del_4_bufferobj);
    this->gen_01_del_3_bufferobj->setIndex(7);
    this->gen_01_del_3_buffer = new Float64Buffer(this->gen_01_del_3_bufferobj);
    this->gen_01_del_2_bufferobj->setIndex(8);
    this->gen_01_del_2_buffer = new Float64Buffer(this->gen_01_del_2_bufferobj);
    this->gen_01_del_1_bufferobj->setIndex(9);
    this->gen_01_del_1_buffer = new Float64Buffer(this->gen_01_del_1_bufferobj);
    this->gen_01_del3_bufferobj->setIndex(10);
    this->gen_01_del3_buffer = new Float64Buffer(this->gen_01_del3_bufferobj);
    this->gen_01_del2_bufferobj->setIndex(11);
    this->gen_01_del2_buffer = new Float64Buffer(this->gen_01_del2_bufferobj);
    this->initializeObjects();
    this->allocateDataRefs();
    this->startup();
}

Index getIsMuted()  {
    return this->isMuted;
}

void setIsMuted(Index v)  {
    this->isMuted = v;
}

Index getPatcherSerial() const {
    return 0;
}

void getState(PatcherStateInterface& ) {}

void setState() {}

void getPreset(PatcherStateInterface& preset) {
    preset["__presetid"] = "rnbo";
    this->param_01_getPresetValue(getSubState(preset, "mix"));
    this->param_02_getPresetValue(getSubState(preset, "delayAll"));
    this->param_03_getPresetValue(getSubState(preset, "delayCom"));
}

void setPreset(MillisecondTime time, PatcherStateInterface& preset) {
    this->updateTime(time);
    this->param_01_setPresetValue(getSubState(preset, "mix"));
    this->param_02_setPresetValue(getSubState(preset, "delayAll"));
    this->param_03_setPresetValue(getSubState(preset, "delayCom"));
}

void processTempoEvent(MillisecondTime time, Tempo tempo) {
    this->updateTime(time);

    if (this->globaltransport_setTempo(this->_currentTime, tempo, false))
        {}
}

void processTransportEvent(MillisecondTime time, TransportState state) {
    this->updateTime(time);

    if (this->globaltransport_setState(this->_currentTime, state, false))
        {}
}

void processBeatTimeEvent(MillisecondTime time, BeatTime beattime) {
    this->updateTime(time);

    if (this->globaltransport_setBeatTime(this->_currentTime, beattime, false))
        {}
}

void onSampleRateChanged(double ) {}

void processTimeSignatureEvent(MillisecondTime time, int numerator, int denominator) {
    this->updateTime(time);

    if (this->globaltransport_setTimeSignature(this->_currentTime, numerator, denominator, false))
        {}
}

void setParameterValue(ParameterIndex index, ParameterValue v, MillisecondTime time) {
    this->updateTime(time);

    switch (index) {
    case 0:
        {
        this->param_01_value_set(v);
        break;
        }
    case 1:
        {
        this->param_02_value_set(v);
        break;
        }
    case 2:
        {
        this->param_03_value_set(v);
        break;
        }
    }
}

void processParameterEvent(ParameterIndex index, ParameterValue value, MillisecondTime time) {
    this->setParameterValue(index, value, time);
}

void processParameterBangEvent(ParameterIndex index, MillisecondTime time) {
    this->setParameterValue(index, this->getParameterValue(index), time);
}

void processNormalizedParameterEvent(ParameterIndex index, ParameterValue value, MillisecondTime time) {
    this->setParameterValueNormalized(index, value, time);
}

ParameterValue getParameterValue(ParameterIndex index)  {
    switch (index) {
    case 0:
        {
        return this->param_01_value;
        }
    case 1:
        {
        return this->param_02_value;
        }
    case 2:
        {
        return this->param_03_value;
        }
    default:
        {
        return 0;
        }
    }
}

ParameterIndex getNumSignalInParameters() const {
    return 0;
}

ParameterIndex getNumSignalOutParameters() const {
    return 0;
}

ParameterIndex getNumParameters() const {
    return 3;
}

ConstCharPointer getParameterName(ParameterIndex index) const {
    switch (index) {
    case 0:
        {
        return "mix";
        }
    case 1:
        {
        return "delayAll";
        }
    case 2:
        {
        return "delayCom";
        }
    default:
        {
        return "bogus";
        }
    }
}

ConstCharPointer getParameterId(ParameterIndex index) const {
    switch (index) {
    case 0:
        {
        return "mix";
        }
    case 1:
        {
        return "delayAll";
        }
    case 2:
        {
        return "delayCom";
        }
    default:
        {
        return "bogus";
        }
    }
}

void getParameterInfo(ParameterIndex index, ParameterInfo * info) const {
    {
        switch (index) {
        case 0:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 0;
            info->min = 0;
            info->max = 100;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 1:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 5;
            info->min = 0;
            info->max = 1000;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 2:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 39.85;
            info->min = 0;
            info->max = 1000;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        }
    }
}

void sendParameter(ParameterIndex index, bool ignoreValue) {
    this->getEngine()->notifyParameterValueChanged(index, (ignoreValue ? 0 : this->getParameterValue(index)), ignoreValue);
}

ParameterValue applyStepsToNormalizedParameterValue(ParameterValue normalizedValue, int steps) const {
    if (steps == 1) {
        if (normalizedValue > 0) {
            normalizedValue = 1.;
        }
    } else {
        ParameterValue oneStep = (number)1. / (steps - 1);
        ParameterValue numberOfSteps = rnbo_fround(normalizedValue / oneStep * 1 / (number)1) * (number)1;
        normalizedValue = numberOfSteps * oneStep;
    }

    return normalizedValue;
}

ParameterValue convertToNormalizedParameterValue(ParameterIndex index, ParameterValue value) const {
    switch (index) {
    case 0:
        {
        {
            value = (value < 0 ? 0 : (value > 100 ? 100 : value));
            ParameterValue normalizedValue = (value - 0) / (100 - 0);
            return normalizedValue;
        }
        }
    case 1:
    case 2:
        {
        {
            value = (value < 0 ? 0 : (value > 1000 ? 1000 : value));
            ParameterValue normalizedValue = (value - 0) / (1000 - 0);
            return normalizedValue;
        }
        }
    default:
        {
        return value;
        }
    }
}

ParameterValue convertFromNormalizedParameterValue(ParameterIndex index, ParameterValue value) const {
    value = (value < 0 ? 0 : (value > 1 ? 1 : value));

    switch (index) {
    case 0:
        {
        {
            value = (value < 0 ? 0 : (value > 1 ? 1 : value));

            {
                return 0 + value * (100 - 0);
            }
        }
        }
    case 1:
    case 2:
        {
        {
            value = (value < 0 ? 0 : (value > 1 ? 1 : value));

            {
                return 0 + value * (1000 - 0);
            }
        }
        }
    default:
        {
        return value;
        }
    }
}

ParameterValue constrainParameterValue(ParameterIndex index, ParameterValue value) const {
    switch (index) {
    case 0:
        {
        return this->param_01_value_constrain(value);
        }
    case 1:
        {
        return this->param_02_value_constrain(value);
        }
    case 2:
        {
        return this->param_03_value_constrain(value);
        }
    default:
        {
        return value;
        }
    }
}

void scheduleParamInit(ParameterIndex index, Index order) {
    this->paramInitIndices->push(index);
    this->paramInitOrder->push(order);
}

void processParamInitEvents() {
    this->listquicksort(
        this->paramInitOrder,
        this->paramInitIndices,
        0,
        (int)(this->paramInitOrder->length - 1),
        true
    );

    for (Index i = 0; i < this->paramInitOrder->length; i++) {
        this->getEngine()->scheduleParameterBang(this->paramInitIndices[i], 0);
    }
}

void processClockEvent(MillisecondTime , ClockId , bool , ParameterValue ) {}

void processOutletAtCurrentTime(EngineLink* , OutletIndex , ParameterValue ) {}

void processOutletEvent(
    EngineLink* sender,
    OutletIndex index,
    ParameterValue value,
    MillisecondTime time
) {
    this->updateTime(time);
    this->processOutletAtCurrentTime(sender, index, value);
}

void processNumMessage(MessageTag , MessageTag , MillisecondTime , number ) {}

void processListMessage(MessageTag , MessageTag , MillisecondTime , const list& ) {}

void processBangMessage(MessageTag , MessageTag , MillisecondTime ) {}

MessageTagInfo resolveTag(MessageTag tag) const {
    switch (tag) {
    case TAG("setup"):
        {
        return "setup";
        }
    case TAG("scope~_obj-11"):
        {
        return "scope~_obj-11";
        }
    case TAG("monitor"):
        {
        return "monitor";
        }
    }

    return "";
}

MessageIndex getNumMessages() const {
    return 0;
}

const MessageInfo& getMessageInfo(MessageIndex index) const {
    switch (index) {

    }

    return NullMessageInfo;
}

protected:

void param_01_value_set(number v) {
    v = this->param_01_value_constrain(v);
    this->param_01_value = v;
    this->sendParameter(0, false);

    if (this->param_01_value != this->param_01_lastValue) {
        this->getEngine()->presetTouched();
        this->param_01_lastValue = this->param_01_value;
    }

    this->gen_01_mix_set(v);
}

void param_02_value_set(number v) {
    v = this->param_02_value_constrain(v);
    this->param_02_value = v;
    this->sendParameter(1, false);

    if (this->param_02_value != this->param_02_lastValue) {
        this->getEngine()->presetTouched();
        this->param_02_lastValue = this->param_02_value;
    }

    this->gen_01_delayAll_set(v);
}

void param_03_value_set(number v) {
    v = this->param_03_value_constrain(v);
    this->param_03_value = v;
    this->sendParameter(2, false);

    if (this->param_03_value != this->param_03_lastValue) {
        this->getEngine()->presetTouched();
        this->param_03_lastValue = this->param_03_value;
    }

    this->gen_01_delayCom_set(v);
}

number msToSamps(MillisecondTime ms, number sampleRate) {
    return ms * sampleRate * 0.001;
}

MillisecondTime sampsToMs(SampleIndex samps) {
    return samps * (this->invsr * 1000);
}

Index getMaxBlockSize() const {
    return this->maxvs;
}

number getSampleRate() const {
    return this->sr;
}

bool hasFixedVectorSize() const {
    return false;
}

Index getNumInputChannels() const {
    return 2;
}

Index getNumOutputChannels() const {
    return 2;
}

void allocateDataRefs() {
    this->gen_01_del_9_buffer = this->gen_01_del_9_buffer->allocateIfNeeded();

    if (this->gen_01_del_9_bufferobj->hasRequestedSize()) {
        if (this->gen_01_del_9_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_01_del_9_bufferobj);

        this->getEngine()->sendDataRefUpdated(0);
    }

    this->gen_01_del_8_buffer = this->gen_01_del_8_buffer->allocateIfNeeded();

    if (this->gen_01_del_8_bufferobj->hasRequestedSize()) {
        if (this->gen_01_del_8_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_01_del_8_bufferobj);

        this->getEngine()->sendDataRefUpdated(1);
    }

    this->gen_01_del_7_buffer = this->gen_01_del_7_buffer->allocateIfNeeded();

    if (this->gen_01_del_7_bufferobj->hasRequestedSize()) {
        if (this->gen_01_del_7_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_01_del_7_bufferobj);

        this->getEngine()->sendDataRefUpdated(2);
    }

    this->gen_01_del1_buffer = this->gen_01_del1_buffer->allocateIfNeeded();

    if (this->gen_01_del1_bufferobj->hasRequestedSize()) {
        if (this->gen_01_del1_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_01_del1_bufferobj);

        this->getEngine()->sendDataRefUpdated(3);
    }

    this->gen_01_del_6_buffer = this->gen_01_del_6_buffer->allocateIfNeeded();

    if (this->gen_01_del_6_bufferobj->hasRequestedSize()) {
        if (this->gen_01_del_6_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_01_del_6_bufferobj);

        this->getEngine()->sendDataRefUpdated(4);
    }

    this->gen_01_del_5_buffer = this->gen_01_del_5_buffer->allocateIfNeeded();

    if (this->gen_01_del_5_bufferobj->hasRequestedSize()) {
        if (this->gen_01_del_5_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_01_del_5_bufferobj);

        this->getEngine()->sendDataRefUpdated(5);
    }

    this->gen_01_del_4_buffer = this->gen_01_del_4_buffer->allocateIfNeeded();

    if (this->gen_01_del_4_bufferobj->hasRequestedSize()) {
        if (this->gen_01_del_4_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_01_del_4_bufferobj);

        this->getEngine()->sendDataRefUpdated(6);
    }

    this->gen_01_del_3_buffer = this->gen_01_del_3_buffer->allocateIfNeeded();

    if (this->gen_01_del_3_bufferobj->hasRequestedSize()) {
        if (this->gen_01_del_3_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_01_del_3_bufferobj);

        this->getEngine()->sendDataRefUpdated(7);
    }

    this->gen_01_del_2_buffer = this->gen_01_del_2_buffer->allocateIfNeeded();

    if (this->gen_01_del_2_bufferobj->hasRequestedSize()) {
        if (this->gen_01_del_2_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_01_del_2_bufferobj);

        this->getEngine()->sendDataRefUpdated(8);
    }

    this->gen_01_del_1_buffer = this->gen_01_del_1_buffer->allocateIfNeeded();

    if (this->gen_01_del_1_bufferobj->hasRequestedSize()) {
        if (this->gen_01_del_1_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_01_del_1_bufferobj);

        this->getEngine()->sendDataRefUpdated(9);
    }

    this->gen_01_del3_buffer = this->gen_01_del3_buffer->allocateIfNeeded();

    if (this->gen_01_del3_bufferobj->hasRequestedSize()) {
        if (this->gen_01_del3_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_01_del3_bufferobj);

        this->getEngine()->sendDataRefUpdated(10);
    }

    this->gen_01_del2_buffer = this->gen_01_del2_buffer->allocateIfNeeded();

    if (this->gen_01_del2_bufferobj->hasRequestedSize()) {
        if (this->gen_01_del2_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_01_del2_bufferobj);

        this->getEngine()->sendDataRefUpdated(11);
    }
}

void initializeObjects() {
    this->gen_01_del_9_init();
    this->gen_01_del_8_init();
    this->gen_01_del_7_init();
    this->gen_01_del1_init();
    this->gen_01_del_6_init();
    this->gen_01_del_5_init();
    this->gen_01_del_4_init();
    this->gen_01_del_3_init();
    this->gen_01_del_2_init();
    this->gen_01_del_1_init();
    this->gen_01_del3_init();
    this->gen_01_del2_init();
}

void sendOutlet(OutletIndex index, ParameterValue value) {
    this->getEngine()->sendOutlet(this, index, value);
}

void startup() {
    this->updateTime(this->getEngine()->getCurrentTime());

    {
        this->scheduleParamInit(0, 0);
    }

    {
        this->scheduleParamInit(1, 0);
    }

    {
        this->scheduleParamInit(2, 0);
    }

    this->processParamInitEvents();
}

static number param_01_value_constrain(number v) {
    v = (v > 100 ? 100 : (v < 0 ? 0 : v));
    return v;
}

void gen_01_mix_set(number v) {
    this->gen_01_mix = v;
}

static number param_02_value_constrain(number v) {
    v = (v > 1000 ? 1000 : (v < 0 ? 0 : v));
    return v;
}

void gen_01_delayAll_set(number v) {
    this->gen_01_delayAll = v;
}

static number param_03_value_constrain(number v) {
    v = (v > 1000 ? 1000 : (v < 0 ? 0 : v));
    return v;
}

void gen_01_delayCom_set(number v) {
    this->gen_01_delayCom = v;
}

void gen_01_perform(
    const Sample * in1,
    const Sample * in2,
    number delayAll,
    number delayCom,
    number mix,
    SampleValue * out1,
    SampleValue * out2,
    Index n
) {
    number div_10_0 = mix / (number)100;
    auto scale_11_1 = this->scale(mix, 0, 100, 1, 2.25, 0.5);
    Index i;

    for (i = 0; i < n; i++) {
        number div_12_2 = (scale_11_1 == 0. ? 0. : in1[(Index)i] / scale_11_1);
        number mul_13_3 = div_12_2 * 0.9;
        number div_14_4 = (scale_11_1 == 0. ? 0. : in2[(Index)i] / scale_11_1);
        number mul_15_5 = div_14_4 * 0.9;
        number mul1_6 = this->gen_01_del2_read(this->mstosamps(delayCom) / (number)1.321, 0) * 0.9;
        number mul2_7 = this->gen_01_del3_read(this->mstosamps(delayCom) / (number)1.321, 0) * 0.9;
        number add2_8 = mul_13_3 + mul1_6;
        number add3_9 = mul_15_5 + mul2_7;
        number expr_16_10 = (add2_8 > 1 ? 1 : (add2_8 < -1 ? -1 : add2_8));
        number expr_17_11 = (add3_9 > 1 ? 1 : (add3_9 < -1 ? -1 : add3_9));
        this->gen_01_del2_write(add2_8 + mul_13_3);
        this->gen_01_del3_write(add3_9 + mul_15_5);
        number mul_18_12 = this->gen_01_del_1_read(this->mstosamps(delayCom) / (number)1.103, 0) * 0.88;
        number mul_19_13 = this->gen_01_del_2_read(this->mstosamps(delayCom) / (number)1.103, 0) * 0.88;
        number add_20_14 = mul_13_3 + mul_18_12;
        number add_21_15 = mul_15_5 + mul_19_13;
        number expr_22_16 = (add_20_14 > 1 ? 1 : (add_20_14 < -1 ? -1 : add_20_14));
        number expr_23_17 = (add_21_15 > 1 ? 1 : (add_21_15 < -1 ? -1 : add_21_15));
        this->gen_01_del_1_write(add_20_14 + mul_13_3);
        this->gen_01_del_2_write(add_21_15 + mul_15_5);
        number mul_24_18 = this->gen_01_del_3_read(this->mstosamps(delayCom) / (number)1.197, 0) * 0.89;
        number mul_25_19 = this->gen_01_del_4_read(this->mstosamps(delayCom) / (number)1.197, 0) * 0.89;
        number add_26_20 = mul_13_3 + mul_24_18;
        number add_27_21 = mul_15_5 + mul_25_19;
        number expr_28_22 = (add_26_20 > 1 ? 1 : (add_26_20 < -1 ? -1 : add_26_20));
        number expr_29_23 = (add_27_21 > 1 ? 1 : (add_27_21 < -1 ? -1 : add_27_21));
        this->gen_01_del_3_write(add_26_20 + mul_13_3);
        this->gen_01_del_4_write(add_27_21 + mul_15_5);
        number mul_30_24 = this->gen_01_del_5_read(this->mstosamps(delayCom), 0) * 0.87;
        number mul_31_25 = this->gen_01_del_6_read(this->mstosamps(delayCom), 0) * 0.87;
        number add_32_26 = mul_13_3 + mul_30_24;
        number add_33_27 = mul_15_5 + mul_31_25;
        number expr_34_28 = (add_32_26 > 1 ? 1 : (add_32_26 < -1 ? -1 : add_32_26));
        number expr_35_29 = (add_33_27 > 1 ? 1 : (add_33_27 < -1 ? -1 : add_33_27));
        this->gen_01_del_5_write(add_32_26 + mul_13_3);
        this->gen_01_del_6_write(add_33_27 + mul_15_5);
        number add_36_30 = expr_34_28 + expr_28_22 + (expr_22_16 + expr_16_10);
        number add_37_31 = expr_35_29 + expr_23_17 + expr_29_23 + expr_17_11 + 0;
        number tap1_32 = this->gen_01_del_7_read(this->mstosamps(delayAll), 0);
        number tap2_33 = this->gen_01_del_7_read(this->mstosamps(delayAll), 0);
        number add1_34 = add_36_30 + tap1_32 * 0.7 * -1;
        number add_38_35 = add_37_31 + tap2_33 * 0.7 * -1;
        number expr_39_36 = (add1_34 * 0.7 + tap1_32 > 1 ? 1 : (add1_34 * 0.7 + tap1_32 < -1 ? -1 : add1_34 * 0.7 + tap1_32));
        number expr_40_37 = (add_38_35 * 0.7 + tap1_32 > 1 ? 1 : (add_38_35 * 0.7 + tap1_32 < -1 ? -1 : add_38_35 * 0.7 + tap1_32));
        this->gen_01_del1_write(add1_34);
        this->gen_01_del_7_write(add_38_35);
        number tap_41_38 = this->gen_01_del_8_read(this->mstosamps(delayAll) / (number)2.941, 0);
        number tap_42_39 = this->gen_01_del_9_read(this->mstosamps(delayAll) / (number)2.941, 0);
        number add_43_40 = expr_39_36 + tap_41_38 * 0.7 * -1;
        number add_44_41 = expr_40_37 + tap_42_39 * 0.7 * -1;
        number expr_45_42 = (add_43_40 * 0.7 + tap_41_38 > 1 ? 1 : (add_43_40 * 0.7 + tap_41_38 < -1 ? -1 : add_43_40 * 0.7 + tap_41_38));
        number expr_46_43 = (add_44_41 * 0.7 + tap_41_38 > 1 ? 1 : (add_44_41 * 0.7 + tap_41_38 < -1 ? -1 : add_44_41 * 0.7 + tap_41_38));
        this->gen_01_del_8_write(add_43_40);
        this->gen_01_del_9_write(add_44_41);
        number add_47_44 = expr_45_42 + mul_13_3;
        number mix_48_45 = in1[(Index)i] + div_10_0 * (add_47_44 - in1[(Index)i]);
        out1[(Index)i] = mix_48_45;
        number add_49_46 = expr_46_43 + mul_15_5;
        number mix_50_47 = in2[(Index)i] + div_10_0 * (add_49_46 - in2[(Index)i]);
        out2[(Index)i] = mix_50_47;
        this->gen_01_del_9_step();
        this->gen_01_del_8_step();
        this->gen_01_del_7_step();
        this->gen_01_del1_step();
        this->gen_01_del_6_step();
        this->gen_01_del_5_step();
        this->gen_01_del_4_step();
        this->gen_01_del_3_step();
        this->gen_01_del_2_step();
        this->gen_01_del_1_step();
        this->gen_01_del3_step();
        this->gen_01_del2_step();
    }
}

void scopetilde_01_perform(const SampleValue * x, const SampleValue * y, Index n) {
    auto __scopetilde_01_ysign = this->scopetilde_01_ysign;
    auto __scopetilde_01_ymonitorvalue = this->scopetilde_01_ymonitorvalue;
    auto __scopetilde_01_xsign = this->scopetilde_01_xsign;
    auto __scopetilde_01_xmonitorvalue = this->scopetilde_01_xmonitorvalue;
    auto __scopetilde_01_mode = this->scopetilde_01_mode;

    for (Index i = 0; i < n; i++) {
        number xval = x[(Index)i];
        number yval = y[(Index)i];

        if (__scopetilde_01_mode == 1) {
            number xabsval = rnbo_abs(xval);

            if (xabsval > __scopetilde_01_xmonitorvalue) {
                __scopetilde_01_xmonitorvalue = xabsval;
                __scopetilde_01_xsign = (xval < 0 ? -1 : 1);
            }

            number yabsval = rnbo_abs(yval);

            if (yabsval > __scopetilde_01_ymonitorvalue) {
                __scopetilde_01_ymonitorvalue = yabsval;
                __scopetilde_01_ysign = (yval < 0 ? -1 : 1);
            }
        } else {
            __scopetilde_01_xmonitorvalue = xval;
            __scopetilde_01_xsign = 1;
            __scopetilde_01_ymonitorvalue = yval;
            __scopetilde_01_ysign = 1;
        }

        this->scopetilde_01_effectiveCount--;

        if (this->scopetilde_01_effectiveCount <= 0) {
            this->scopetilde_01_updateEffectiveCount();
            this->scopetilde_01_monitorbuffer->push(__scopetilde_01_xmonitorvalue * __scopetilde_01_xsign);

            if (__scopetilde_01_mode == 1)
                __scopetilde_01_xmonitorvalue = 0;

            if (this->scopetilde_01_monitorbuffer->length >= 128 * (1 + 0)) {
                this->getEngine()->sendListMessage(
                    TAG("monitor"),
                    TAG("scope~_obj-11"),
                    this->scopetilde_01_monitorbuffer,
                    this->_currentTime
                );;

                this->scopetilde_01_monitorbuffer->length = 0;
            }
        }
    }

    this->scopetilde_01_xmonitorvalue = __scopetilde_01_xmonitorvalue;
    this->scopetilde_01_xsign = __scopetilde_01_xsign;
    this->scopetilde_01_ymonitorvalue = __scopetilde_01_ymonitorvalue;
    this->scopetilde_01_ysign = __scopetilde_01_ysign;
}

void stackprotect_perform(Index n) {
    RNBO_UNUSED(n);
    auto __stackprotect_count = this->stackprotect_count;
    __stackprotect_count = 0;
    this->stackprotect_count = __stackprotect_count;
}

void scopetilde_01_updateEffectiveCount() {
    number effectiveCount = 256 * 1 + 256 * 0;
    this->scopetilde_01_effectiveCount = this->maximum(effectiveCount, 256);
}

void scopetilde_01_dspsetup(bool force) {
    if ((bool)(this->scopetilde_01_setupDone) && (bool)(!(bool)(force)))
        return;

    {
        this->scopetilde_01_mode = 1;
    }

    this->getEngine()->sendListMessage(
        TAG("setup"),
        TAG("scope~_obj-11"),
        {1, 1, this->samplerate(), 0, 1, 0, 0, 128, this->scopetilde_01_mode},
        this->_currentTime
    );;

    this->scopetilde_01_updateEffectiveCount();
    this->scopetilde_01_setupDone = true;
}

void gen_01_del_9_step() {
    this->gen_01_del_9_reader++;

    if (this->gen_01_del_9_reader >= (int)(this->gen_01_del_9_buffer->getSize()))
        this->gen_01_del_9_reader = 0;
}

number gen_01_del_9_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_01_del_9_buffer->getSize()) + this->gen_01_del_9_reader - ((size > this->gen_01_del_9__maxdelay ? this->gen_01_del_9__maxdelay : (size < (this->gen_01_del_9_reader != this->gen_01_del_9_writer) ? this->gen_01_del_9_reader != this->gen_01_del_9_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_01_del_9_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del_9_wrap))
        ), this->gen_01_del_9_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_01_del_9_wrap))
        ));
    }

    number r = (int)(this->gen_01_del_9_buffer->getSize()) + this->gen_01_del_9_reader - ((size > this->gen_01_del_9__maxdelay ? this->gen_01_del_9__maxdelay : (size < (this->gen_01_del_9_reader != this->gen_01_del_9_writer) ? this->gen_01_del_9_reader != this->gen_01_del_9_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_01_del_9_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del_9_wrap))
    );
}

void gen_01_del_9_write(number v) {
    this->gen_01_del_9_writer = this->gen_01_del_9_reader;
    this->gen_01_del_9_buffer[(Index)this->gen_01_del_9_writer] = v;
}

number gen_01_del_9_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_01_del_9__maxdelay : size);
    number val = this->gen_01_del_9_read(effectiveSize, 0);
    this->gen_01_del_9_write(v);
    this->gen_01_del_9_step();
    return val;
}

array<Index, 2> gen_01_del_9_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_01_del_9_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_01_del_9_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_01_del_9_init() {
    auto result = this->gen_01_del_9_calcSizeInSamples();
    this->gen_01_del_9__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_01_del_9_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_01_del_9_wrap = requestedSizeInSamples - 1;
}

void gen_01_del_9_clear() {
    this->gen_01_del_9_buffer->setZero();
}

void gen_01_del_9_reset() {
    auto result = this->gen_01_del_9_calcSizeInSamples();
    this->gen_01_del_9__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_01_del_9_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_01_del_9_buffer);
    this->gen_01_del_9_wrap = this->gen_01_del_9_buffer->getSize() - 1;
    this->gen_01_del_9_clear();

    if (this->gen_01_del_9_reader >= this->gen_01_del_9__maxdelay || this->gen_01_del_9_writer >= this->gen_01_del_9__maxdelay) {
        this->gen_01_del_9_reader = 0;
        this->gen_01_del_9_writer = 0;
    }
}

void gen_01_del_9_dspsetup() {
    this->gen_01_del_9_reset();
}

number gen_01_del_9_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return this->samplerate();
}

number gen_01_del_9_size() {
    return this->gen_01_del_9__maxdelay;
}

void gen_01_del_8_step() {
    this->gen_01_del_8_reader++;

    if (this->gen_01_del_8_reader >= (int)(this->gen_01_del_8_buffer->getSize()))
        this->gen_01_del_8_reader = 0;
}

number gen_01_del_8_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_01_del_8_buffer->getSize()) + this->gen_01_del_8_reader - ((size > this->gen_01_del_8__maxdelay ? this->gen_01_del_8__maxdelay : (size < (this->gen_01_del_8_reader != this->gen_01_del_8_writer) ? this->gen_01_del_8_reader != this->gen_01_del_8_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_01_del_8_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del_8_wrap))
        ), this->gen_01_del_8_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_01_del_8_wrap))
        ));
    }

    number r = (int)(this->gen_01_del_8_buffer->getSize()) + this->gen_01_del_8_reader - ((size > this->gen_01_del_8__maxdelay ? this->gen_01_del_8__maxdelay : (size < (this->gen_01_del_8_reader != this->gen_01_del_8_writer) ? this->gen_01_del_8_reader != this->gen_01_del_8_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_01_del_8_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del_8_wrap))
    );
}

void gen_01_del_8_write(number v) {
    this->gen_01_del_8_writer = this->gen_01_del_8_reader;
    this->gen_01_del_8_buffer[(Index)this->gen_01_del_8_writer] = v;
}

number gen_01_del_8_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_01_del_8__maxdelay : size);
    number val = this->gen_01_del_8_read(effectiveSize, 0);
    this->gen_01_del_8_write(v);
    this->gen_01_del_8_step();
    return val;
}

array<Index, 2> gen_01_del_8_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_01_del_8_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_01_del_8_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_01_del_8_init() {
    auto result = this->gen_01_del_8_calcSizeInSamples();
    this->gen_01_del_8__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_01_del_8_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_01_del_8_wrap = requestedSizeInSamples - 1;
}

void gen_01_del_8_clear() {
    this->gen_01_del_8_buffer->setZero();
}

void gen_01_del_8_reset() {
    auto result = this->gen_01_del_8_calcSizeInSamples();
    this->gen_01_del_8__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_01_del_8_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_01_del_8_buffer);
    this->gen_01_del_8_wrap = this->gen_01_del_8_buffer->getSize() - 1;
    this->gen_01_del_8_clear();

    if (this->gen_01_del_8_reader >= this->gen_01_del_8__maxdelay || this->gen_01_del_8_writer >= this->gen_01_del_8__maxdelay) {
        this->gen_01_del_8_reader = 0;
        this->gen_01_del_8_writer = 0;
    }
}

void gen_01_del_8_dspsetup() {
    this->gen_01_del_8_reset();
}

number gen_01_del_8_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return this->samplerate();
}

number gen_01_del_8_size() {
    return this->gen_01_del_8__maxdelay;
}

void gen_01_del_7_step() {
    this->gen_01_del_7_reader++;

    if (this->gen_01_del_7_reader >= (int)(this->gen_01_del_7_buffer->getSize()))
        this->gen_01_del_7_reader = 0;
}

number gen_01_del_7_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_01_del_7_buffer->getSize()) + this->gen_01_del_7_reader - ((size > this->gen_01_del_7__maxdelay ? this->gen_01_del_7__maxdelay : (size < (this->gen_01_del_7_reader != this->gen_01_del_7_writer) ? this->gen_01_del_7_reader != this->gen_01_del_7_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_01_del_7_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del_7_wrap))
        ), this->gen_01_del_7_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_01_del_7_wrap))
        ));
    }

    number r = (int)(this->gen_01_del_7_buffer->getSize()) + this->gen_01_del_7_reader - ((size > this->gen_01_del_7__maxdelay ? this->gen_01_del_7__maxdelay : (size < (this->gen_01_del_7_reader != this->gen_01_del_7_writer) ? this->gen_01_del_7_reader != this->gen_01_del_7_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_01_del_7_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del_7_wrap))
    );
}

void gen_01_del_7_write(number v) {
    this->gen_01_del_7_writer = this->gen_01_del_7_reader;
    this->gen_01_del_7_buffer[(Index)this->gen_01_del_7_writer] = v;
}

number gen_01_del_7_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_01_del_7__maxdelay : size);
    number val = this->gen_01_del_7_read(effectiveSize, 0);
    this->gen_01_del_7_write(v);
    this->gen_01_del_7_step();
    return val;
}

array<Index, 2> gen_01_del_7_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_01_del_7_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_01_del_7_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_01_del_7_init() {
    auto result = this->gen_01_del_7_calcSizeInSamples();
    this->gen_01_del_7__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_01_del_7_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_01_del_7_wrap = requestedSizeInSamples - 1;
}

void gen_01_del_7_clear() {
    this->gen_01_del_7_buffer->setZero();
}

void gen_01_del_7_reset() {
    auto result = this->gen_01_del_7_calcSizeInSamples();
    this->gen_01_del_7__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_01_del_7_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_01_del_7_buffer);
    this->gen_01_del_7_wrap = this->gen_01_del_7_buffer->getSize() - 1;
    this->gen_01_del_7_clear();

    if (this->gen_01_del_7_reader >= this->gen_01_del_7__maxdelay || this->gen_01_del_7_writer >= this->gen_01_del_7__maxdelay) {
        this->gen_01_del_7_reader = 0;
        this->gen_01_del_7_writer = 0;
    }
}

void gen_01_del_7_dspsetup() {
    this->gen_01_del_7_reset();
}

number gen_01_del_7_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return this->samplerate();
}

number gen_01_del_7_size() {
    return this->gen_01_del_7__maxdelay;
}

void gen_01_del1_step() {
    this->gen_01_del1_reader++;

    if (this->gen_01_del1_reader >= (int)(this->gen_01_del1_buffer->getSize()))
        this->gen_01_del1_reader = 0;
}

number gen_01_del1_read(number size, Int interp) {
    if (interp == 0) {
        number r = (int)(this->gen_01_del1_buffer->getSize()) + this->gen_01_del1_reader - ((size > this->gen_01_del1__maxdelay ? this->gen_01_del1__maxdelay : (size < (this->gen_01_del1_reader != this->gen_01_del1_writer) ? this->gen_01_del1_reader != this->gen_01_del1_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_01_del1_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del1_wrap))
        ), this->gen_01_del1_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_01_del1_wrap))
        ));
    } else if (interp == 1) {
        number r = (int)(this->gen_01_del1_buffer->getSize()) + this->gen_01_del1_reader - ((size > this->gen_01_del1__maxdelay ? this->gen_01_del1__maxdelay : (size < (1 + this->gen_01_del1_reader != this->gen_01_del1_writer) ? 1 + this->gen_01_del1_reader != this->gen_01_del1_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        Index index2 = (Index)(index1 + 1);
        Index index3 = (Index)(index2 + 1);
        Index index4 = (Index)(index3 + 1);

        return this->cubicinterp(frac, this->gen_01_del1_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del1_wrap))
        ), this->gen_01_del1_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_01_del1_wrap))
        ), this->gen_01_del1_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index3 & (BinOpInt)this->gen_01_del1_wrap))
        ), this->gen_01_del1_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index4 & (BinOpInt)this->gen_01_del1_wrap))
        ));
    } else if (interp == 2) {
        number r = (int)(this->gen_01_del1_buffer->getSize()) + this->gen_01_del1_reader - ((size > this->gen_01_del1__maxdelay ? this->gen_01_del1__maxdelay : (size < (1 + this->gen_01_del1_reader != this->gen_01_del1_writer) ? 1 + this->gen_01_del1_reader != this->gen_01_del1_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        Index index2 = (Index)(index1 + 1);
        Index index3 = (Index)(index2 + 1);
        Index index4 = (Index)(index3 + 1);

        return this->splineinterp(frac, this->gen_01_del1_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del1_wrap))
        ), this->gen_01_del1_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_01_del1_wrap))
        ), this->gen_01_del1_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index3 & (BinOpInt)this->gen_01_del1_wrap))
        ), this->gen_01_del1_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index4 & (BinOpInt)this->gen_01_del1_wrap))
        ));
    } else if (interp == 3) {
        number r = (int)(this->gen_01_del1_buffer->getSize()) + this->gen_01_del1_reader - ((size > this->gen_01_del1__maxdelay ? this->gen_01_del1__maxdelay : (size < (this->gen_01_del1_reader != this->gen_01_del1_writer) ? this->gen_01_del1_reader != this->gen_01_del1_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        Index index2 = (Index)(index1 + 1);

        return this->cosineinterp(frac, this->gen_01_del1_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del1_wrap))
        ), this->gen_01_del1_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_01_del1_wrap))
        ));
    }

    number r = (int)(this->gen_01_del1_buffer->getSize()) + this->gen_01_del1_reader - ((size > this->gen_01_del1__maxdelay ? this->gen_01_del1__maxdelay : (size < (this->gen_01_del1_reader != this->gen_01_del1_writer) ? this->gen_01_del1_reader != this->gen_01_del1_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_01_del1_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del1_wrap))
    );
}

void gen_01_del1_write(number v) {
    this->gen_01_del1_writer = this->gen_01_del1_reader;
    this->gen_01_del1_buffer[(Index)this->gen_01_del1_writer] = v;
}

number gen_01_del1_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_01_del1__maxdelay : size);
    number val = this->gen_01_del1_read(effectiveSize, 0);
    this->gen_01_del1_write(v);
    this->gen_01_del1_step();
    return val;
}

array<Index, 2> gen_01_del1_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_01_del1_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_01_del1_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_01_del1_init() {
    auto result = this->gen_01_del1_calcSizeInSamples();
    this->gen_01_del1__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_01_del1_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_01_del1_wrap = requestedSizeInSamples - 1;
}

void gen_01_del1_clear() {
    this->gen_01_del1_buffer->setZero();
}

void gen_01_del1_reset() {
    auto result = this->gen_01_del1_calcSizeInSamples();
    this->gen_01_del1__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_01_del1_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_01_del1_buffer);
    this->gen_01_del1_wrap = this->gen_01_del1_buffer->getSize() - 1;
    this->gen_01_del1_clear();

    if (this->gen_01_del1_reader >= this->gen_01_del1__maxdelay || this->gen_01_del1_writer >= this->gen_01_del1__maxdelay) {
        this->gen_01_del1_reader = 0;
        this->gen_01_del1_writer = 0;
    }
}

void gen_01_del1_dspsetup() {
    this->gen_01_del1_reset();
}

number gen_01_del1_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return this->samplerate();
}

number gen_01_del1_size() {
    return this->gen_01_del1__maxdelay;
}

void gen_01_del_6_step() {
    this->gen_01_del_6_reader++;

    if (this->gen_01_del_6_reader >= (int)(this->gen_01_del_6_buffer->getSize()))
        this->gen_01_del_6_reader = 0;
}

number gen_01_del_6_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_01_del_6_buffer->getSize()) + this->gen_01_del_6_reader - ((size > this->gen_01_del_6__maxdelay ? this->gen_01_del_6__maxdelay : (size < (this->gen_01_del_6_reader != this->gen_01_del_6_writer) ? this->gen_01_del_6_reader != this->gen_01_del_6_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_01_del_6_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del_6_wrap))
        ), this->gen_01_del_6_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_01_del_6_wrap))
        ));
    }

    number r = (int)(this->gen_01_del_6_buffer->getSize()) + this->gen_01_del_6_reader - ((size > this->gen_01_del_6__maxdelay ? this->gen_01_del_6__maxdelay : (size < (this->gen_01_del_6_reader != this->gen_01_del_6_writer) ? this->gen_01_del_6_reader != this->gen_01_del_6_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_01_del_6_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del_6_wrap))
    );
}

void gen_01_del_6_write(number v) {
    this->gen_01_del_6_writer = this->gen_01_del_6_reader;
    this->gen_01_del_6_buffer[(Index)this->gen_01_del_6_writer] = v;
}

number gen_01_del_6_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_01_del_6__maxdelay : size);
    number val = this->gen_01_del_6_read(effectiveSize, 0);
    this->gen_01_del_6_write(v);
    this->gen_01_del_6_step();
    return val;
}

array<Index, 2> gen_01_del_6_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_01_del_6_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_01_del_6_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_01_del_6_init() {
    auto result = this->gen_01_del_6_calcSizeInSamples();
    this->gen_01_del_6__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_01_del_6_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_01_del_6_wrap = requestedSizeInSamples - 1;
}

void gen_01_del_6_clear() {
    this->gen_01_del_6_buffer->setZero();
}

void gen_01_del_6_reset() {
    auto result = this->gen_01_del_6_calcSizeInSamples();
    this->gen_01_del_6__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_01_del_6_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_01_del_6_buffer);
    this->gen_01_del_6_wrap = this->gen_01_del_6_buffer->getSize() - 1;
    this->gen_01_del_6_clear();

    if (this->gen_01_del_6_reader >= this->gen_01_del_6__maxdelay || this->gen_01_del_6_writer >= this->gen_01_del_6__maxdelay) {
        this->gen_01_del_6_reader = 0;
        this->gen_01_del_6_writer = 0;
    }
}

void gen_01_del_6_dspsetup() {
    this->gen_01_del_6_reset();
}

number gen_01_del_6_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return this->samplerate();
}

number gen_01_del_6_size() {
    return this->gen_01_del_6__maxdelay;
}

void gen_01_del_5_step() {
    this->gen_01_del_5_reader++;

    if (this->gen_01_del_5_reader >= (int)(this->gen_01_del_5_buffer->getSize()))
        this->gen_01_del_5_reader = 0;
}

number gen_01_del_5_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_01_del_5_buffer->getSize()) + this->gen_01_del_5_reader - ((size > this->gen_01_del_5__maxdelay ? this->gen_01_del_5__maxdelay : (size < (this->gen_01_del_5_reader != this->gen_01_del_5_writer) ? this->gen_01_del_5_reader != this->gen_01_del_5_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_01_del_5_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del_5_wrap))
        ), this->gen_01_del_5_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_01_del_5_wrap))
        ));
    }

    number r = (int)(this->gen_01_del_5_buffer->getSize()) + this->gen_01_del_5_reader - ((size > this->gen_01_del_5__maxdelay ? this->gen_01_del_5__maxdelay : (size < (this->gen_01_del_5_reader != this->gen_01_del_5_writer) ? this->gen_01_del_5_reader != this->gen_01_del_5_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_01_del_5_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del_5_wrap))
    );
}

void gen_01_del_5_write(number v) {
    this->gen_01_del_5_writer = this->gen_01_del_5_reader;
    this->gen_01_del_5_buffer[(Index)this->gen_01_del_5_writer] = v;
}

number gen_01_del_5_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_01_del_5__maxdelay : size);
    number val = this->gen_01_del_5_read(effectiveSize, 0);
    this->gen_01_del_5_write(v);
    this->gen_01_del_5_step();
    return val;
}

array<Index, 2> gen_01_del_5_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_01_del_5_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_01_del_5_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_01_del_5_init() {
    auto result = this->gen_01_del_5_calcSizeInSamples();
    this->gen_01_del_5__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_01_del_5_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_01_del_5_wrap = requestedSizeInSamples - 1;
}

void gen_01_del_5_clear() {
    this->gen_01_del_5_buffer->setZero();
}

void gen_01_del_5_reset() {
    auto result = this->gen_01_del_5_calcSizeInSamples();
    this->gen_01_del_5__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_01_del_5_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_01_del_5_buffer);
    this->gen_01_del_5_wrap = this->gen_01_del_5_buffer->getSize() - 1;
    this->gen_01_del_5_clear();

    if (this->gen_01_del_5_reader >= this->gen_01_del_5__maxdelay || this->gen_01_del_5_writer >= this->gen_01_del_5__maxdelay) {
        this->gen_01_del_5_reader = 0;
        this->gen_01_del_5_writer = 0;
    }
}

void gen_01_del_5_dspsetup() {
    this->gen_01_del_5_reset();
}

number gen_01_del_5_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return this->samplerate();
}

number gen_01_del_5_size() {
    return this->gen_01_del_5__maxdelay;
}

void gen_01_del_4_step() {
    this->gen_01_del_4_reader++;

    if (this->gen_01_del_4_reader >= (int)(this->gen_01_del_4_buffer->getSize()))
        this->gen_01_del_4_reader = 0;
}

number gen_01_del_4_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_01_del_4_buffer->getSize()) + this->gen_01_del_4_reader - ((size > this->gen_01_del_4__maxdelay ? this->gen_01_del_4__maxdelay : (size < (this->gen_01_del_4_reader != this->gen_01_del_4_writer) ? this->gen_01_del_4_reader != this->gen_01_del_4_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_01_del_4_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del_4_wrap))
        ), this->gen_01_del_4_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_01_del_4_wrap))
        ));
    }

    number r = (int)(this->gen_01_del_4_buffer->getSize()) + this->gen_01_del_4_reader - ((size > this->gen_01_del_4__maxdelay ? this->gen_01_del_4__maxdelay : (size < (this->gen_01_del_4_reader != this->gen_01_del_4_writer) ? this->gen_01_del_4_reader != this->gen_01_del_4_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_01_del_4_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del_4_wrap))
    );
}

void gen_01_del_4_write(number v) {
    this->gen_01_del_4_writer = this->gen_01_del_4_reader;
    this->gen_01_del_4_buffer[(Index)this->gen_01_del_4_writer] = v;
}

number gen_01_del_4_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_01_del_4__maxdelay : size);
    number val = this->gen_01_del_4_read(effectiveSize, 0);
    this->gen_01_del_4_write(v);
    this->gen_01_del_4_step();
    return val;
}

array<Index, 2> gen_01_del_4_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_01_del_4_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_01_del_4_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_01_del_4_init() {
    auto result = this->gen_01_del_4_calcSizeInSamples();
    this->gen_01_del_4__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_01_del_4_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_01_del_4_wrap = requestedSizeInSamples - 1;
}

void gen_01_del_4_clear() {
    this->gen_01_del_4_buffer->setZero();
}

void gen_01_del_4_reset() {
    auto result = this->gen_01_del_4_calcSizeInSamples();
    this->gen_01_del_4__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_01_del_4_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_01_del_4_buffer);
    this->gen_01_del_4_wrap = this->gen_01_del_4_buffer->getSize() - 1;
    this->gen_01_del_4_clear();

    if (this->gen_01_del_4_reader >= this->gen_01_del_4__maxdelay || this->gen_01_del_4_writer >= this->gen_01_del_4__maxdelay) {
        this->gen_01_del_4_reader = 0;
        this->gen_01_del_4_writer = 0;
    }
}

void gen_01_del_4_dspsetup() {
    this->gen_01_del_4_reset();
}

number gen_01_del_4_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return this->samplerate();
}

number gen_01_del_4_size() {
    return this->gen_01_del_4__maxdelay;
}

void gen_01_del_3_step() {
    this->gen_01_del_3_reader++;

    if (this->gen_01_del_3_reader >= (int)(this->gen_01_del_3_buffer->getSize()))
        this->gen_01_del_3_reader = 0;
}

number gen_01_del_3_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_01_del_3_buffer->getSize()) + this->gen_01_del_3_reader - ((size > this->gen_01_del_3__maxdelay ? this->gen_01_del_3__maxdelay : (size < (this->gen_01_del_3_reader != this->gen_01_del_3_writer) ? this->gen_01_del_3_reader != this->gen_01_del_3_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_01_del_3_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del_3_wrap))
        ), this->gen_01_del_3_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_01_del_3_wrap))
        ));
    }

    number r = (int)(this->gen_01_del_3_buffer->getSize()) + this->gen_01_del_3_reader - ((size > this->gen_01_del_3__maxdelay ? this->gen_01_del_3__maxdelay : (size < (this->gen_01_del_3_reader != this->gen_01_del_3_writer) ? this->gen_01_del_3_reader != this->gen_01_del_3_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_01_del_3_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del_3_wrap))
    );
}

void gen_01_del_3_write(number v) {
    this->gen_01_del_3_writer = this->gen_01_del_3_reader;
    this->gen_01_del_3_buffer[(Index)this->gen_01_del_3_writer] = v;
}

number gen_01_del_3_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_01_del_3__maxdelay : size);
    number val = this->gen_01_del_3_read(effectiveSize, 0);
    this->gen_01_del_3_write(v);
    this->gen_01_del_3_step();
    return val;
}

array<Index, 2> gen_01_del_3_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_01_del_3_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_01_del_3_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_01_del_3_init() {
    auto result = this->gen_01_del_3_calcSizeInSamples();
    this->gen_01_del_3__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_01_del_3_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_01_del_3_wrap = requestedSizeInSamples - 1;
}

void gen_01_del_3_clear() {
    this->gen_01_del_3_buffer->setZero();
}

void gen_01_del_3_reset() {
    auto result = this->gen_01_del_3_calcSizeInSamples();
    this->gen_01_del_3__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_01_del_3_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_01_del_3_buffer);
    this->gen_01_del_3_wrap = this->gen_01_del_3_buffer->getSize() - 1;
    this->gen_01_del_3_clear();

    if (this->gen_01_del_3_reader >= this->gen_01_del_3__maxdelay || this->gen_01_del_3_writer >= this->gen_01_del_3__maxdelay) {
        this->gen_01_del_3_reader = 0;
        this->gen_01_del_3_writer = 0;
    }
}

void gen_01_del_3_dspsetup() {
    this->gen_01_del_3_reset();
}

number gen_01_del_3_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return this->samplerate();
}

number gen_01_del_3_size() {
    return this->gen_01_del_3__maxdelay;
}

void gen_01_del_2_step() {
    this->gen_01_del_2_reader++;

    if (this->gen_01_del_2_reader >= (int)(this->gen_01_del_2_buffer->getSize()))
        this->gen_01_del_2_reader = 0;
}

number gen_01_del_2_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_01_del_2_buffer->getSize()) + this->gen_01_del_2_reader - ((size > this->gen_01_del_2__maxdelay ? this->gen_01_del_2__maxdelay : (size < (this->gen_01_del_2_reader != this->gen_01_del_2_writer) ? this->gen_01_del_2_reader != this->gen_01_del_2_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_01_del_2_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del_2_wrap))
        ), this->gen_01_del_2_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_01_del_2_wrap))
        ));
    }

    number r = (int)(this->gen_01_del_2_buffer->getSize()) + this->gen_01_del_2_reader - ((size > this->gen_01_del_2__maxdelay ? this->gen_01_del_2__maxdelay : (size < (this->gen_01_del_2_reader != this->gen_01_del_2_writer) ? this->gen_01_del_2_reader != this->gen_01_del_2_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_01_del_2_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del_2_wrap))
    );
}

void gen_01_del_2_write(number v) {
    this->gen_01_del_2_writer = this->gen_01_del_2_reader;
    this->gen_01_del_2_buffer[(Index)this->gen_01_del_2_writer] = v;
}

number gen_01_del_2_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_01_del_2__maxdelay : size);
    number val = this->gen_01_del_2_read(effectiveSize, 0);
    this->gen_01_del_2_write(v);
    this->gen_01_del_2_step();
    return val;
}

array<Index, 2> gen_01_del_2_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_01_del_2_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_01_del_2_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_01_del_2_init() {
    auto result = this->gen_01_del_2_calcSizeInSamples();
    this->gen_01_del_2__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_01_del_2_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_01_del_2_wrap = requestedSizeInSamples - 1;
}

void gen_01_del_2_clear() {
    this->gen_01_del_2_buffer->setZero();
}

void gen_01_del_2_reset() {
    auto result = this->gen_01_del_2_calcSizeInSamples();
    this->gen_01_del_2__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_01_del_2_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_01_del_2_buffer);
    this->gen_01_del_2_wrap = this->gen_01_del_2_buffer->getSize() - 1;
    this->gen_01_del_2_clear();

    if (this->gen_01_del_2_reader >= this->gen_01_del_2__maxdelay || this->gen_01_del_2_writer >= this->gen_01_del_2__maxdelay) {
        this->gen_01_del_2_reader = 0;
        this->gen_01_del_2_writer = 0;
    }
}

void gen_01_del_2_dspsetup() {
    this->gen_01_del_2_reset();
}

number gen_01_del_2_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return this->samplerate();
}

number gen_01_del_2_size() {
    return this->gen_01_del_2__maxdelay;
}

void gen_01_del_1_step() {
    this->gen_01_del_1_reader++;

    if (this->gen_01_del_1_reader >= (int)(this->gen_01_del_1_buffer->getSize()))
        this->gen_01_del_1_reader = 0;
}

number gen_01_del_1_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_01_del_1_buffer->getSize()) + this->gen_01_del_1_reader - ((size > this->gen_01_del_1__maxdelay ? this->gen_01_del_1__maxdelay : (size < (this->gen_01_del_1_reader != this->gen_01_del_1_writer) ? this->gen_01_del_1_reader != this->gen_01_del_1_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_01_del_1_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del_1_wrap))
        ), this->gen_01_del_1_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_01_del_1_wrap))
        ));
    }

    number r = (int)(this->gen_01_del_1_buffer->getSize()) + this->gen_01_del_1_reader - ((size > this->gen_01_del_1__maxdelay ? this->gen_01_del_1__maxdelay : (size < (this->gen_01_del_1_reader != this->gen_01_del_1_writer) ? this->gen_01_del_1_reader != this->gen_01_del_1_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_01_del_1_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del_1_wrap))
    );
}

void gen_01_del_1_write(number v) {
    this->gen_01_del_1_writer = this->gen_01_del_1_reader;
    this->gen_01_del_1_buffer[(Index)this->gen_01_del_1_writer] = v;
}

number gen_01_del_1_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_01_del_1__maxdelay : size);
    number val = this->gen_01_del_1_read(effectiveSize, 0);
    this->gen_01_del_1_write(v);
    this->gen_01_del_1_step();
    return val;
}

array<Index, 2> gen_01_del_1_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_01_del_1_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_01_del_1_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_01_del_1_init() {
    auto result = this->gen_01_del_1_calcSizeInSamples();
    this->gen_01_del_1__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_01_del_1_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_01_del_1_wrap = requestedSizeInSamples - 1;
}

void gen_01_del_1_clear() {
    this->gen_01_del_1_buffer->setZero();
}

void gen_01_del_1_reset() {
    auto result = this->gen_01_del_1_calcSizeInSamples();
    this->gen_01_del_1__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_01_del_1_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_01_del_1_buffer);
    this->gen_01_del_1_wrap = this->gen_01_del_1_buffer->getSize() - 1;
    this->gen_01_del_1_clear();

    if (this->gen_01_del_1_reader >= this->gen_01_del_1__maxdelay || this->gen_01_del_1_writer >= this->gen_01_del_1__maxdelay) {
        this->gen_01_del_1_reader = 0;
        this->gen_01_del_1_writer = 0;
    }
}

void gen_01_del_1_dspsetup() {
    this->gen_01_del_1_reset();
}

number gen_01_del_1_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return this->samplerate();
}

number gen_01_del_1_size() {
    return this->gen_01_del_1__maxdelay;
}

void gen_01_del3_step() {
    this->gen_01_del3_reader++;

    if (this->gen_01_del3_reader >= (int)(this->gen_01_del3_buffer->getSize()))
        this->gen_01_del3_reader = 0;
}

number gen_01_del3_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_01_del3_buffer->getSize()) + this->gen_01_del3_reader - ((size > this->gen_01_del3__maxdelay ? this->gen_01_del3__maxdelay : (size < (this->gen_01_del3_reader != this->gen_01_del3_writer) ? this->gen_01_del3_reader != this->gen_01_del3_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_01_del3_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del3_wrap))
        ), this->gen_01_del3_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_01_del3_wrap))
        ));
    }

    number r = (int)(this->gen_01_del3_buffer->getSize()) + this->gen_01_del3_reader - ((size > this->gen_01_del3__maxdelay ? this->gen_01_del3__maxdelay : (size < (this->gen_01_del3_reader != this->gen_01_del3_writer) ? this->gen_01_del3_reader != this->gen_01_del3_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_01_del3_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del3_wrap))
    );
}

void gen_01_del3_write(number v) {
    this->gen_01_del3_writer = this->gen_01_del3_reader;
    this->gen_01_del3_buffer[(Index)this->gen_01_del3_writer] = v;
}

number gen_01_del3_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_01_del3__maxdelay : size);
    number val = this->gen_01_del3_read(effectiveSize, 0);
    this->gen_01_del3_write(v);
    this->gen_01_del3_step();
    return val;
}

array<Index, 2> gen_01_del3_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_01_del3_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_01_del3_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_01_del3_init() {
    auto result = this->gen_01_del3_calcSizeInSamples();
    this->gen_01_del3__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_01_del3_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_01_del3_wrap = requestedSizeInSamples - 1;
}

void gen_01_del3_clear() {
    this->gen_01_del3_buffer->setZero();
}

void gen_01_del3_reset() {
    auto result = this->gen_01_del3_calcSizeInSamples();
    this->gen_01_del3__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_01_del3_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_01_del3_buffer);
    this->gen_01_del3_wrap = this->gen_01_del3_buffer->getSize() - 1;
    this->gen_01_del3_clear();

    if (this->gen_01_del3_reader >= this->gen_01_del3__maxdelay || this->gen_01_del3_writer >= this->gen_01_del3__maxdelay) {
        this->gen_01_del3_reader = 0;
        this->gen_01_del3_writer = 0;
    }
}

void gen_01_del3_dspsetup() {
    this->gen_01_del3_reset();
}

number gen_01_del3_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return this->samplerate();
}

number gen_01_del3_size() {
    return this->gen_01_del3__maxdelay;
}

void gen_01_del2_step() {
    this->gen_01_del2_reader++;

    if (this->gen_01_del2_reader >= (int)(this->gen_01_del2_buffer->getSize()))
        this->gen_01_del2_reader = 0;
}

number gen_01_del2_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_01_del2_buffer->getSize()) + this->gen_01_del2_reader - ((size > this->gen_01_del2__maxdelay ? this->gen_01_del2__maxdelay : (size < (this->gen_01_del2_reader != this->gen_01_del2_writer) ? this->gen_01_del2_reader != this->gen_01_del2_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_01_del2_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del2_wrap))
        ), this->gen_01_del2_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_01_del2_wrap))
        ));
    }

    number r = (int)(this->gen_01_del2_buffer->getSize()) + this->gen_01_del2_reader - ((size > this->gen_01_del2__maxdelay ? this->gen_01_del2__maxdelay : (size < (this->gen_01_del2_reader != this->gen_01_del2_writer) ? this->gen_01_del2_reader != this->gen_01_del2_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_01_del2_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del2_wrap))
    );
}

void gen_01_del2_write(number v) {
    this->gen_01_del2_writer = this->gen_01_del2_reader;
    this->gen_01_del2_buffer[(Index)this->gen_01_del2_writer] = v;
}

number gen_01_del2_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_01_del2__maxdelay : size);
    number val = this->gen_01_del2_read(effectiveSize, 0);
    this->gen_01_del2_write(v);
    this->gen_01_del2_step();
    return val;
}

array<Index, 2> gen_01_del2_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_01_del2_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_01_del2_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_01_del2_init() {
    auto result = this->gen_01_del2_calcSizeInSamples();
    this->gen_01_del2__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_01_del2_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_01_del2_wrap = requestedSizeInSamples - 1;
}

void gen_01_del2_clear() {
    this->gen_01_del2_buffer->setZero();
}

void gen_01_del2_reset() {
    auto result = this->gen_01_del2_calcSizeInSamples();
    this->gen_01_del2__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_01_del2_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_01_del2_buffer);
    this->gen_01_del2_wrap = this->gen_01_del2_buffer->getSize() - 1;
    this->gen_01_del2_clear();

    if (this->gen_01_del2_reader >= this->gen_01_del2__maxdelay || this->gen_01_del2_writer >= this->gen_01_del2__maxdelay) {
        this->gen_01_del2_reader = 0;
        this->gen_01_del2_writer = 0;
    }
}

void gen_01_del2_dspsetup() {
    this->gen_01_del2_reset();
}

number gen_01_del2_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return this->samplerate();
}

number gen_01_del2_size() {
    return this->gen_01_del2__maxdelay;
}

void gen_01_dspsetup(bool force) {
    if ((bool)(this->gen_01_setupDone) && (bool)(!(bool)(force)))
        return;

    this->gen_01_setupDone = true;
    this->gen_01_del_9_dspsetup();
    this->gen_01_del_8_dspsetup();
    this->gen_01_del_7_dspsetup();
    this->gen_01_del1_dspsetup();
    this->gen_01_del_6_dspsetup();
    this->gen_01_del_5_dspsetup();
    this->gen_01_del_4_dspsetup();
    this->gen_01_del_3_dspsetup();
    this->gen_01_del_2_dspsetup();
    this->gen_01_del_1_dspsetup();
    this->gen_01_del3_dspsetup();
    this->gen_01_del2_dspsetup();
}

void param_01_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_01_value;
}

void param_01_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_01_value_set(preset["value"]);
}

void param_02_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_02_value;
}

void param_02_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_02_value_set(preset["value"]);
}

void param_03_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_03_value;
}

void param_03_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_03_value_set(preset["value"]);
}

Index globaltransport_getSampleOffset(MillisecondTime time) {
    return this->mstosamps(this->maximum(0, time - this->getEngine()->getCurrentTime()));
}

number globaltransport_getTempoAtSample(SampleIndex sampleOffset) {
    return (sampleOffset >= 0 && sampleOffset < this->vs ? this->globaltransport_tempo[(Index)sampleOffset] : this->globaltransport_lastTempo);
}

number globaltransport_getStateAtSample(SampleIndex sampleOffset) {
    return (sampleOffset >= 0 && sampleOffset < this->vs ? this->globaltransport_state[(Index)sampleOffset] : this->globaltransport_lastState);
}

number globaltransport_getState(MillisecondTime time) {
    return this->globaltransport_getStateAtSample(this->globaltransport_getSampleOffset(time));
}

number globaltransport_getBeatTime(MillisecondTime time) {
    number i = 2;

    while (i < this->globaltransport_beatTimeChanges->length && this->globaltransport_beatTimeChanges[(Index)(i + 1)] <= time) {
        i += 2;
    }

    i -= 2;
    number beatTimeBase = this->globaltransport_beatTimeChanges[(Index)i];

    if (this->globaltransport_getState(time) == 0)
        return beatTimeBase;

    number beatTimeBaseMsTime = this->globaltransport_beatTimeChanges[(Index)(i + 1)];
    number diff = time - beatTimeBaseMsTime;
    return beatTimeBase + this->mstobeats(diff);
}

bool globaltransport_setTempo(MillisecondTime time, number tempo, bool notify) {
    if ((bool)(notify)) {
        this->processTempoEvent(time, tempo);
        this->globaltransport_notify = true;
    } else {
        Index offset = (Index)(this->globaltransport_getSampleOffset(time));

        if (this->globaltransport_getTempoAtSample(offset) != tempo) {
            this->globaltransport_beatTimeChanges->push(this->globaltransport_getBeatTime(time));
            this->globaltransport_beatTimeChanges->push(time);
            fillSignal(this->globaltransport_tempo, this->vs, tempo, offset);
            this->globaltransport_lastTempo = tempo;
            this->globaltransport_tempoNeedsReset = true;
            return true;
        }
    }

    return false;
}

number globaltransport_getTempo(MillisecondTime time) {
    return this->globaltransport_getTempoAtSample(this->globaltransport_getSampleOffset(time));
}

bool globaltransport_setState(MillisecondTime time, number state, bool notify) {
    if ((bool)(notify)) {
        this->processTransportEvent(time, TransportState(state));
        this->globaltransport_notify = true;
    } else {
        Index offset = (Index)(this->globaltransport_getSampleOffset(time));

        if (this->globaltransport_getStateAtSample(offset) != state) {
            fillSignal(this->globaltransport_state, this->vs, state, offset);
            this->globaltransport_lastState = TransportState(state);
            this->globaltransport_stateNeedsReset = true;

            if (state == 0) {
                this->globaltransport_beatTimeChanges->push(this->globaltransport_getBeatTime(time));
                this->globaltransport_beatTimeChanges->push(time);
            }

            return true;
        }
    }

    return false;
}

bool globaltransport_setBeatTime(MillisecondTime time, number beattime, bool notify) {
    if ((bool)(notify)) {
        this->processBeatTimeEvent(time, beattime);
        this->globaltransport_notify = true;
        return false;
    } else {
        bool beatTimeHasChanged = false;
        float oldBeatTime = (float)(this->globaltransport_getBeatTime(time));
        float newBeatTime = (float)(beattime);

        if (oldBeatTime != newBeatTime) {
            beatTimeHasChanged = true;
        }

        this->globaltransport_beatTimeChanges->push(beattime);
        this->globaltransport_beatTimeChanges->push(time);
        return beatTimeHasChanged;
    }
}

number globaltransport_getBeatTimeAtSample(SampleIndex sampleOffset) {
    auto msOffset = this->sampstoms(sampleOffset);
    return this->globaltransport_getBeatTime(this->getEngine()->getCurrentTime() + msOffset);
}

array<number, 2> globaltransport_getTimeSignature(MillisecondTime time) {
    number i = 3;

    while (i < this->globaltransport_timeSignatureChanges->length && this->globaltransport_timeSignatureChanges[(Index)(i + 2)] <= time) {
        i += 3;
    }

    i -= 3;

    return {
        this->globaltransport_timeSignatureChanges[(Index)i],
        this->globaltransport_timeSignatureChanges[(Index)(i + 1)]
    };
}

array<number, 2> globaltransport_getTimeSignatureAtSample(SampleIndex sampleOffset) {
    auto msOffset = this->sampstoms(sampleOffset);
    return this->globaltransport_getTimeSignature(this->getEngine()->getCurrentTime() + msOffset);
}

bool globaltransport_setTimeSignature(MillisecondTime time, number numerator, number denominator, bool notify) {
    if ((bool)(notify)) {
        this->processTimeSignatureEvent(time, (int)(numerator), (int)(denominator));
        this->globaltransport_notify = true;
    } else {
        array<number, 2> currentSig = this->globaltransport_getTimeSignature(time);

        if (currentSig[0] != numerator || currentSig[1] != denominator) {
            this->globaltransport_timeSignatureChanges->push(numerator);
            this->globaltransport_timeSignatureChanges->push(denominator);
            this->globaltransport_timeSignatureChanges->push(time);
            return true;
        }
    }

    return false;
}

void globaltransport_advance() {
    if ((bool)(this->globaltransport_tempoNeedsReset)) {
        fillSignal(this->globaltransport_tempo, this->vs, this->globaltransport_lastTempo);
        this->globaltransport_tempoNeedsReset = false;

        if ((bool)(this->globaltransport_notify)) {
            this->getEngine()->sendTempoEvent(this->globaltransport_lastTempo);
        }
    }

    if ((bool)(this->globaltransport_stateNeedsReset)) {
        fillSignal(this->globaltransport_state, this->vs, this->globaltransport_lastState);
        this->globaltransport_stateNeedsReset = false;

        if ((bool)(this->globaltransport_notify)) {
            this->getEngine()->sendTransportEvent(TransportState(this->globaltransport_lastState));
        }
    }

    if (this->globaltransport_beatTimeChanges->length > 2) {
        this->globaltransport_beatTimeChanges[0] = this->globaltransport_beatTimeChanges[(Index)(this->globaltransport_beatTimeChanges->length - 2)];
        this->globaltransport_beatTimeChanges[1] = this->globaltransport_beatTimeChanges[(Index)(this->globaltransport_beatTimeChanges->length - 1)];
        this->globaltransport_beatTimeChanges->length = 2;

        if ((bool)(this->globaltransport_notify)) {
            this->getEngine()->sendBeatTimeEvent(this->globaltransport_beatTimeChanges[0]);
        }
    }

    if (this->globaltransport_timeSignatureChanges->length > 3) {
        this->globaltransport_timeSignatureChanges[0] = this->globaltransport_timeSignatureChanges[(Index)(this->globaltransport_timeSignatureChanges->length - 3)];
        this->globaltransport_timeSignatureChanges[1] = this->globaltransport_timeSignatureChanges[(Index)(this->globaltransport_timeSignatureChanges->length - 2)];
        this->globaltransport_timeSignatureChanges[2] = this->globaltransport_timeSignatureChanges[(Index)(this->globaltransport_timeSignatureChanges->length - 1)];
        this->globaltransport_timeSignatureChanges->length = 3;

        if ((bool)(this->globaltransport_notify)) {
            this->getEngine()->sendTimeSignatureEvent(
                (int)(this->globaltransport_timeSignatureChanges[0]),
                (int)(this->globaltransport_timeSignatureChanges[1])
            );
        }
    }

    this->globaltransport_notify = false;
}

void globaltransport_dspsetup(bool force) {
    if ((bool)(this->globaltransport_setupDone) && (bool)(!(bool)(force)))
        return;

    fillSignal(this->globaltransport_tempo, this->vs, this->globaltransport_lastTempo);
    this->globaltransport_tempoNeedsReset = false;
    fillSignal(this->globaltransport_state, this->vs, this->globaltransport_lastState);
    this->globaltransport_stateNeedsReset = false;
    this->globaltransport_setupDone = true;
}

bool stackprotect_check() {
    this->stackprotect_count++;

    if (this->stackprotect_count > 128) {
        console->log("STACK OVERFLOW DETECTED - stopped processing branch !");
        return true;
    }

    return false;
}

void updateTime(MillisecondTime time) {
    this->_currentTime = time;
    this->sampleOffsetIntoNextAudioBuffer = (SampleIndex)(rnbo_fround(this->msToSamps(time - this->getEngine()->getCurrentTime(), this->sr)));

    if (this->sampleOffsetIntoNextAudioBuffer >= (SampleIndex)(this->vs))
        this->sampleOffsetIntoNextAudioBuffer = (SampleIndex)(this->vs) - 1;

    if (this->sampleOffsetIntoNextAudioBuffer < 0)
        this->sampleOffsetIntoNextAudioBuffer = 0;
}

void assign_defaults()
{
    gen_01_in1 = 0;
    gen_01_in2 = 0;
    gen_01_delayAll = 0;
    gen_01_delayCom = 0;
    gen_01_mix = 0;
    param_01_value = 0;
    param_02_value = 5;
    param_03_value = 39.85;
    _currentTime = 0;
    audioProcessSampleCount = 0;
    sampleOffsetIntoNextAudioBuffer = 0;
    zeroBuffer = nullptr;
    dummyBuffer = nullptr;
    didAllocateSignals = 0;
    vs = 0;
    maxvs = 0;
    sr = 44100;
    invsr = 0.00002267573696;
    scopetilde_01_lastValue = 0;
    scopetilde_01_effectiveCount = 256;
    scopetilde_01_xsign = 1;
    scopetilde_01_ysign = 1;
    scopetilde_01_mode = 0;
    scopetilde_01_setupDone = false;
    gen_01_del_9__maxdelay = 0;
    gen_01_del_9_sizemode = 0;
    gen_01_del_9_wrap = 0;
    gen_01_del_9_reader = 0;
    gen_01_del_9_writer = 0;
    gen_01_del_8__maxdelay = 0;
    gen_01_del_8_sizemode = 0;
    gen_01_del_8_wrap = 0;
    gen_01_del_8_reader = 0;
    gen_01_del_8_writer = 0;
    gen_01_del_7__maxdelay = 0;
    gen_01_del_7_sizemode = 0;
    gen_01_del_7_wrap = 0;
    gen_01_del_7_reader = 0;
    gen_01_del_7_writer = 0;
    gen_01_del1__maxdelay = 0;
    gen_01_del1_sizemode = 0;
    gen_01_del1_wrap = 0;
    gen_01_del1_reader = 0;
    gen_01_del1_writer = 0;
    gen_01_del_6__maxdelay = 0;
    gen_01_del_6_sizemode = 0;
    gen_01_del_6_wrap = 0;
    gen_01_del_6_reader = 0;
    gen_01_del_6_writer = 0;
    gen_01_del_5__maxdelay = 0;
    gen_01_del_5_sizemode = 0;
    gen_01_del_5_wrap = 0;
    gen_01_del_5_reader = 0;
    gen_01_del_5_writer = 0;
    gen_01_del_4__maxdelay = 0;
    gen_01_del_4_sizemode = 0;
    gen_01_del_4_wrap = 0;
    gen_01_del_4_reader = 0;
    gen_01_del_4_writer = 0;
    gen_01_del_3__maxdelay = 0;
    gen_01_del_3_sizemode = 0;
    gen_01_del_3_wrap = 0;
    gen_01_del_3_reader = 0;
    gen_01_del_3_writer = 0;
    gen_01_del_2__maxdelay = 0;
    gen_01_del_2_sizemode = 0;
    gen_01_del_2_wrap = 0;
    gen_01_del_2_reader = 0;
    gen_01_del_2_writer = 0;
    gen_01_del_1__maxdelay = 0;
    gen_01_del_1_sizemode = 0;
    gen_01_del_1_wrap = 0;
    gen_01_del_1_reader = 0;
    gen_01_del_1_writer = 0;
    gen_01_del3__maxdelay = 0;
    gen_01_del3_sizemode = 0;
    gen_01_del3_wrap = 0;
    gen_01_del3_reader = 0;
    gen_01_del3_writer = 0;
    gen_01_del2__maxdelay = 0;
    gen_01_del2_sizemode = 0;
    gen_01_del2_wrap = 0;
    gen_01_del2_reader = 0;
    gen_01_del2_writer = 0;
    gen_01_setupDone = false;
    param_01_lastValue = 0;
    param_02_lastValue = 0;
    param_03_lastValue = 0;
    globaltransport_tempo = nullptr;
    globaltransport_tempoNeedsReset = false;
    globaltransport_lastTempo = 120;
    globaltransport_state = nullptr;
    globaltransport_stateNeedsReset = false;
    globaltransport_lastState = 0;
    globaltransport_beatTimeChanges = { 0, 0 };
    globaltransport_timeSignatureChanges = { 4, 4, 0 };
    globaltransport_notify = false;
    globaltransport_setupDone = false;
    stackprotect_count = 0;
    _voiceIndex = 0;
    _noteNumber = 0;
    isMuted = 1;
}

// member variables

    number gen_01_in1;
    number gen_01_in2;
    number gen_01_delayAll;
    number gen_01_delayCom;
    number gen_01_mix;
    number param_01_value;
    number param_02_value;
    number param_03_value;
    MillisecondTime _currentTime;
    SampleIndex audioProcessSampleCount;
    SampleIndex sampleOffsetIntoNextAudioBuffer;
    signal zeroBuffer;
    signal dummyBuffer;
    bool didAllocateSignals;
    Index vs;
    Index maxvs;
    number sr;
    number invsr;
    number scopetilde_01_lastValue;
    number scopetilde_01_effectiveCount;
    number scopetilde_01_xmonitorvalue;
    number scopetilde_01_ymonitorvalue;
    list scopetilde_01_monitorbuffer;
    number scopetilde_01_xsign;
    number scopetilde_01_ysign;
    Int scopetilde_01_mode;
    bool scopetilde_01_setupDone;
    Float64BufferRef gen_01_del_9_buffer;
    Index gen_01_del_9__maxdelay;
    Int gen_01_del_9_sizemode;
    Index gen_01_del_9_wrap;
    Int gen_01_del_9_reader;
    Int gen_01_del_9_writer;
    Float64BufferRef gen_01_del_8_buffer;
    Index gen_01_del_8__maxdelay;
    Int gen_01_del_8_sizemode;
    Index gen_01_del_8_wrap;
    Int gen_01_del_8_reader;
    Int gen_01_del_8_writer;
    Float64BufferRef gen_01_del_7_buffer;
    Index gen_01_del_7__maxdelay;
    Int gen_01_del_7_sizemode;
    Index gen_01_del_7_wrap;
    Int gen_01_del_7_reader;
    Int gen_01_del_7_writer;
    Float64BufferRef gen_01_del1_buffer;
    Index gen_01_del1__maxdelay;
    Int gen_01_del1_sizemode;
    Index gen_01_del1_wrap;
    Int gen_01_del1_reader;
    Int gen_01_del1_writer;
    Float64BufferRef gen_01_del_6_buffer;
    Index gen_01_del_6__maxdelay;
    Int gen_01_del_6_sizemode;
    Index gen_01_del_6_wrap;
    Int gen_01_del_6_reader;
    Int gen_01_del_6_writer;
    Float64BufferRef gen_01_del_5_buffer;
    Index gen_01_del_5__maxdelay;
    Int gen_01_del_5_sizemode;
    Index gen_01_del_5_wrap;
    Int gen_01_del_5_reader;
    Int gen_01_del_5_writer;
    Float64BufferRef gen_01_del_4_buffer;
    Index gen_01_del_4__maxdelay;
    Int gen_01_del_4_sizemode;
    Index gen_01_del_4_wrap;
    Int gen_01_del_4_reader;
    Int gen_01_del_4_writer;
    Float64BufferRef gen_01_del_3_buffer;
    Index gen_01_del_3__maxdelay;
    Int gen_01_del_3_sizemode;
    Index gen_01_del_3_wrap;
    Int gen_01_del_3_reader;
    Int gen_01_del_3_writer;
    Float64BufferRef gen_01_del_2_buffer;
    Index gen_01_del_2__maxdelay;
    Int gen_01_del_2_sizemode;
    Index gen_01_del_2_wrap;
    Int gen_01_del_2_reader;
    Int gen_01_del_2_writer;
    Float64BufferRef gen_01_del_1_buffer;
    Index gen_01_del_1__maxdelay;
    Int gen_01_del_1_sizemode;
    Index gen_01_del_1_wrap;
    Int gen_01_del_1_reader;
    Int gen_01_del_1_writer;
    Float64BufferRef gen_01_del3_buffer;
    Index gen_01_del3__maxdelay;
    Int gen_01_del3_sizemode;
    Index gen_01_del3_wrap;
    Int gen_01_del3_reader;
    Int gen_01_del3_writer;
    Float64BufferRef gen_01_del2_buffer;
    Index gen_01_del2__maxdelay;
    Int gen_01_del2_sizemode;
    Index gen_01_del2_wrap;
    Int gen_01_del2_reader;
    Int gen_01_del2_writer;
    bool gen_01_setupDone;
    number param_01_lastValue;
    number param_02_lastValue;
    number param_03_lastValue;
    signal globaltransport_tempo;
    bool globaltransport_tempoNeedsReset;
    number globaltransport_lastTempo;
    signal globaltransport_state;
    bool globaltransport_stateNeedsReset;
    number globaltransport_lastState;
    list globaltransport_beatTimeChanges;
    list globaltransport_timeSignatureChanges;
    bool globaltransport_notify;
    bool globaltransport_setupDone;
    number stackprotect_count;
    DataRef gen_01_del_9_bufferobj;
    DataRef gen_01_del_8_bufferobj;
    DataRef gen_01_del_7_bufferobj;
    DataRef gen_01_del1_bufferobj;
    DataRef gen_01_del_6_bufferobj;
    DataRef gen_01_del_5_bufferobj;
    DataRef gen_01_del_4_bufferobj;
    DataRef gen_01_del_3_bufferobj;
    DataRef gen_01_del_2_bufferobj;
    DataRef gen_01_del_1_bufferobj;
    DataRef gen_01_del3_bufferobj;
    DataRef gen_01_del2_bufferobj;
    Index _voiceIndex;
    Int _noteNumber;
    Index isMuted;
    indexlist paramInitIndices;
    indexlist paramInitOrder;

};

PatcherInterface* creaternbomatic()
{
    return new rnbomatic();
}

#ifndef RNBO_NO_PATCHERFACTORY

extern "C" PatcherFactoryFunctionPtr GetPatcherFactoryFunction(PlatformInterface* platformInterface)
#else

extern "C" PatcherFactoryFunctionPtr rnbomaticFactoryFunction(PlatformInterface* platformInterface)
#endif

{
    Platform::set(platformInterface);
    return creaternbomatic;
}

} // end RNBO namespace

