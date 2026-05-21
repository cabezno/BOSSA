/*
 * Copyright (c) 2026 Bossa Project, LLC
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Bossa.Controls as Bossa

Bossa.KeyframableFilter {
    property string delaySyncParam: '1'
    property string delayNoteParam: '2'
    property string delayMsParam: '3'
    property string delayWarpParam: '4'
    property string clearParam: '5'
    property string feedbackParam: '6'
    property string densityParam: '7'
    property string widthParam: '8'
    property string lowCutParam: '9'
    property string highCutParam: '10'
    property string modRateParam: '11'
    property string modDepthParam: '12'
    property string modeParam: '13'
    property string wetnessParam: 'wetness'

    property double delaySyncDefault: 0.25
    property double delayNoteDefault: 0.285714
    property double delayMsDefault: 0.5
    property double delayWarpDefault: 0
    property double clearDefault: 1
    property double feedbackDefault: 0.5
    property double densityDefault: 0
    property double widthDefault: 1
    property double lowCutDefault: 0
    property double highCutDefault: 1
    property double modRateDefault: 0.273834
    property double modDepthDefault: 0.5
    property double modeDefault: 0.0416667
    property double wetnessDefault: 0.5

    function setControls() {
        const position = getPosition();
        blockUpdate = true;
        delaySyncSlider.value = filter.getDouble(delaySyncParam, position) * delaySyncSlider.maximumValue;
        delaySyncKeyframesButton.checked = filter.animateIn <= 0 && filter.animateOut <= 0 && filter.keyframeCount(delaySyncParam) > 0;
        delayNoteSlider.value = filter.getDouble(delayNoteParam, position) * delayNoteSlider.maximumValue;
        delayNoteKeyframesButton.checked = filter.animateIn <= 0 && filter.animateOut <= 0 && filter.keyframeCount(delayNoteParam) > 0;
        delayMsSlider.value = filter.getDouble(delayMsParam, position) * delayMsSlider.maximumValue;
        delayMsKeyframesButton.checked = filter.animateIn <= 0 && filter.animateOut <= 0 && filter.keyframeCount(delayMsParam) > 0;
        delayWarpSlider.value = filter.getDouble(delayWarpParam, position) * delayWarpSlider.maximumValue;
        delayWarpKeyframesButton.checked = filter.animateIn <= 0 && filter.animateOut <= 0 && filter.keyframeCount(delayWarpParam) > 0;
        clearSlider.value = filter.getDouble(clearParam, position) * clearSlider.maximumValue;
        clearKeyframesButton.checked = filter.animateIn <= 0 && filter.animateOut <= 0 && filter.keyframeCount(clearParam) > 0;
        feedbackSlider.value = filter.getDouble(feedbackParam, position) * feedbackSlider.maximumValue;
        feedbackKeyframesButton.checked = filter.animateIn <= 0 && filter.animateOut <= 0 && filter.keyframeCount(feedbackParam) > 0;
        densitySlider.value = filter.getDouble(densityParam, position) * densitySlider.maximumValue;
        densityKeyframesButton.checked = filter.animateIn <= 0 && filter.animateOut <= 0 && filter.keyframeCount(densityParam) > 0;
        widthSlider.value = filter.getDouble(widthParam, position) * widthSlider.maximumValue;
        widthKeyframesButton.checked = filter.animateIn <= 0 && filter.animateOut <= 0 && filter.keyframeCount(widthParam) > 0;
        lowCutSlider.value = filter.getDouble(lowCutParam, position) * lowCutSlider.maximumValue;
        lowCutKeyframesButton.checked = filter.animateIn <= 0 && filter.animateOut <= 0 && filter.keyframeCount(lowCutParam) > 0;
        highCutSlider.value = filter.getDouble(highCutParam, position) * highCutSlider.maximumValue;
        highCutKeyframesButton.checked = filter.animateIn <= 0 && filter.animateOut <= 0 && filter.keyframeCount(highCutParam) > 0;
        modRateSlider.value = filter.getDouble(modRateParam, position) * modRateSlider.maximumValue;
        modRateKeyframesButton.checked = filter.animateIn <= 0 && filter.animateOut <= 0 && filter.keyframeCount(modRateParam) > 0;
        modDepthSlider.value = filter.getDouble(modDepthParam, position) * modDepthSlider.maximumValue;
        modDepthKeyframesButton.checked = filter.animateIn <= 0 && filter.animateOut <= 0 && filter.keyframeCount(modDepthParam) > 0;
        modeSlider.value = filter.getDouble(modeParam, position) * modeSlider.maximumValue;
        modeKeyframesButton.checked = filter.animateIn <= 0 && filter.animateOut <= 0 && filter.keyframeCount(modeParam) > 0;
        wetnessSlider.value = filter.getDouble(wetnessParam, position) * wetnessSlider.maximumValue;
        wetnessKeyframesButton.checked = filter.animateIn <= 0 && filter.animateOut <= 0 && filter.keyframeCount(wetnessParam) > 0;
        blockUpdate = false;
        enableControls(isSimpleKeyframesActive());
        channelMask.setChannelsControls();
    }

    function enableControls(enabled) {
        delaySyncSlider.enabled = enabled;
        delayNoteSlider.enabled = enabled;
        delayMsSlider.enabled = enabled;
        delayWarpSlider.enabled = enabled;
        clearSlider.enabled = enabled;
        feedbackSlider.enabled = enabled;
        densitySlider.enabled = enabled;
        widthSlider.enabled = enabled;
        lowCutSlider.enabled = enabled;
        highCutSlider.enabled = enabled;
        modRateSlider.enabled = enabled;
        modDepthSlider.enabled = enabled;
        modeSlider.enabled = enabled;
        wetnessSlider.enabled = enabled;
    }

    function updateSimpleKeyframes() {
        updateFilter(delaySyncParam, delaySyncSlider.value / delaySyncSlider.maximumValue, delaySyncKeyframesButton, null);
        updateFilter(delayNoteParam, delayNoteSlider.value / delayNoteSlider.maximumValue, delayNoteKeyframesButton, null);
        updateFilter(delayMsParam, delayMsSlider.value / delayMsSlider.maximumValue, delayMsKeyframesButton, null);
        updateFilter(delayWarpParam, delayWarpSlider.value / delayWarpSlider.maximumValue, delayWarpKeyframesButton, null);
        updateFilter(clearParam, clearSlider.value / clearSlider.maximumValue, clearKeyframesButton, null);
        updateFilter(feedbackParam, feedbackSlider.value / feedbackSlider.maximumValue, feedbackKeyframesButton, null);
        updateFilter(densityParam, densitySlider.value / densitySlider.maximumValue, densityKeyframesButton, null);
        updateFilter(widthParam, widthSlider.value / widthSlider.maximumValue, widthKeyframesButton, null);
        updateFilter(lowCutParam, lowCutSlider.value / lowCutSlider.maximumValue, lowCutKeyframesButton, null);
        updateFilter(highCutParam, highCutSlider.value / highCutSlider.maximumValue, highCutKeyframesButton, null);
        updateFilter(modRateParam, modRateSlider.value / modRateSlider.maximumValue, modRateKeyframesButton, null);
        updateFilter(modDepthParam, modDepthSlider.value / modDepthSlider.maximumValue, modDepthKeyframesButton, null);
        updateFilter(modeParam, modeSlider.value / modeSlider.maximumValue, modeKeyframesButton, null);
        updateFilter(wetnessParam, wetnessSlider.value / wetnessSlider.maximumValue, wetnessKeyframesButton, null);
    }

    keyframableParameters: [delaySyncParam, delayNoteParam, delayMsParam, delayWarpParam, clearParam, feedbackParam, densityParam, widthParam, lowCutParam, highCutParam, modRateParam, modDepthParam, modeParam, wetnessParam]
    startValues: [0.25, 0.285714, 0.5, 0, 1, 0.5, 0, 1, 0, 1, 0.273834, 0.5, 0.0416667, 1]
    middleValues: [0.25, 0.285714, 0.5, 0, 1, 0.5, 0, 1, 0, 1, 0.273834, 0.5, 0.0416667, 1]
    endValues: [0.25, 0.285714, 0.5, 0, 1, 0.5, 0, 1, 0, 1, 0.273834, 0.5, 0.0416667, 1]
    width: 450
    height: 530

    Component.onCompleted: {
        if (filter.isNew) {
            filter.set('0', 1);
            filter.set(delaySyncParam, delaySyncDefault);
            filter.set(delayNoteParam, delayNoteDefault);
            filter.set(delayMsParam, delayMsDefault);
            filter.set(delayWarpParam, delayWarpDefault);
            filter.set(clearParam, clearDefault);
            filter.set(feedbackParam, feedbackDefault);
            filter.set(densityParam, densityDefault);
            filter.set(widthParam, widthDefault);
            filter.set(lowCutParam, lowCutDefault);
            filter.set(highCutParam, highCutDefault);
            filter.set(modRateParam, modRateDefault);
            filter.set(modDepthParam, modDepthDefault);
            filter.set(modeParam, modeDefault);
            filter.set(wetnessParam, wetnessDefault);
            filter.savePreset(preset.parameters);
        }
        setControls();
    }

    GridLayout {
        anchors.fill: parent
        anchors.margins: 8
        columns: 4

        Label {
            text: qsTr('Preset')
            Layout.alignment: Qt.AlignRight
        }
        Bossa.Preset {
            id: preset
            parameters: [delaySyncParam, delayNoteParam, delayMsParam, delayWarpParam, clearParam, feedbackParam, densityParam, widthParam, lowCutParam, highCutParam, modRateParam, modDepthParam, modeParam, wetnessParam, channelMask.channelMaskProperty]
            Layout.columnSpan: 3
            onPresetSelected: setControls()
        }

        Label {
            text: qsTr('Delay sync')
            Layout.alignment: Qt.AlignRight
        }
        Bossa.SliderSpinner {
            id: delaySyncSlider
            minimumValue: 0
            maximumValue: 100
            decimals: 1
            suffix: ' %'
            onValueChanged: updateFilter(delaySyncParam, value / maximumValue, delaySyncKeyframesButton, getPosition())
        }
        Bossa.UndoButton {
            onClicked: delaySyncSlider.value = delaySyncDefault * delaySyncSlider.maximumValue
        }
        Bossa.KeyframesButton {
            id: delaySyncKeyframesButton
            onToggled: {
                enableControls(true);
                toggleKeyframes(checked, delaySyncParam, delaySyncSlider.value / delaySyncSlider.maximumValue);
            }
        }

        Label {
            text: qsTr('Delay note')
            Layout.alignment: Qt.AlignRight
        }
        Bossa.SliderSpinner {
            id: delayNoteSlider
            minimumValue: 0
            maximumValue: 100
            decimals: 1
            suffix: ' %'
            onValueChanged: updateFilter(delayNoteParam, value / maximumValue, delayNoteKeyframesButton, getPosition())
        }
        Bossa.UndoButton {
            onClicked: delayNoteSlider.value = delayNoteDefault * delayNoteSlider.maximumValue
        }
        Bossa.KeyframesButton {
            id: delayNoteKeyframesButton
            onToggled: {
                enableControls(true);
                toggleKeyframes(checked, delayNoteParam, delayNoteSlider.value / delayNoteSlider.maximumValue);
            }
        }

        Label {
            text: qsTr('Delay (ms)')
            Layout.alignment: Qt.AlignRight
        }
        Bossa.SliderSpinner {
            id: delayMsSlider
            minimumValue: 0
            maximumValue: 100
            decimals: 1
            suffix: ' %'
            onValueChanged: updateFilter(delayMsParam, value / maximumValue, delayMsKeyframesButton, getPosition())
        }
        Bossa.UndoButton {
            onClicked: delayMsSlider.value = delayMsDefault * delayMsSlider.maximumValue
        }
        Bossa.KeyframesButton {
            id: delayMsKeyframesButton
            onToggled: {
                enableControls(true);
                toggleKeyframes(checked, delayMsParam, delayMsSlider.value / delayMsSlider.maximumValue);
            }
        }

        Label {
            text: qsTr('Delay warp')
            Layout.alignment: Qt.AlignRight
        }
        Bossa.SliderSpinner {
            id: delayWarpSlider
            minimumValue: 0
            maximumValue: 100
            decimals: 1
            suffix: ' %'
            onValueChanged: updateFilter(delayWarpParam, value / maximumValue, delayWarpKeyframesButton, getPosition())
        }
        Bossa.UndoButton {
            onClicked: delayWarpSlider.value = delayWarpDefault * delayWarpSlider.maximumValue
        }
        Bossa.KeyframesButton {
            id: delayWarpKeyframesButton
            onToggled: {
                enableControls(true);
                toggleKeyframes(checked, delayWarpParam, delayWarpSlider.value / delayWarpSlider.maximumValue);
            }
        }

        Label {
            text: qsTr('Clear')
            Layout.alignment: Qt.AlignRight
        }
        Bossa.SliderSpinner {
            id: clearSlider
            minimumValue: 0
            maximumValue: 100
            decimals: 1
            suffix: ' %'
            onValueChanged: updateFilter(clearParam, value / maximumValue, clearKeyframesButton, getPosition())
        }
        Bossa.UndoButton {
            onClicked: clearSlider.value = clearDefault * clearSlider.maximumValue
        }
        Bossa.KeyframesButton {
            id: clearKeyframesButton
            onToggled: {
                enableControls(true);
                toggleKeyframes(checked, clearParam, clearSlider.value / clearSlider.maximumValue);
            }
        }

        Label {
            text: qsTr('Feedback')
            Layout.alignment: Qt.AlignRight
        }
        Bossa.SliderSpinner {
            id: feedbackSlider
            minimumValue: 0
            maximumValue: 100
            decimals: 1
            suffix: ' %'
            onValueChanged: updateFilter(feedbackParam, value / maximumValue, feedbackKeyframesButton, getPosition())
        }
        Bossa.UndoButton {
            onClicked: feedbackSlider.value = feedbackDefault * feedbackSlider.maximumValue
        }
        Bossa.KeyframesButton {
            id: feedbackKeyframesButton
            onToggled: {
                enableControls(true);
                toggleKeyframes(checked, feedbackParam, feedbackSlider.value / feedbackSlider.maximumValue);
            }
        }

        Label {
            text: qsTr('Density')
            Layout.alignment: Qt.AlignRight
        }
        Bossa.SliderSpinner {
            id: densitySlider
            minimumValue: 0
            maximumValue: 100
            decimals: 1
            suffix: ' %'
            onValueChanged: updateFilter(densityParam, value / maximumValue, densityKeyframesButton, getPosition())
        }
        Bossa.UndoButton {
            onClicked: densitySlider.value = densityDefault * densitySlider.maximumValue
        }
        Bossa.KeyframesButton {
            id: densityKeyframesButton
            onToggled: {
                enableControls(true);
                toggleKeyframes(checked, densityParam, densitySlider.value / densitySlider.maximumValue);
            }
        }

        Label {
            text: qsTr('Width')
            Layout.alignment: Qt.AlignRight
        }
        Bossa.SliderSpinner {
            id: widthSlider
            minimumValue: 0
            maximumValue: 100
            decimals: 1
            suffix: ' %'
            onValueChanged: updateFilter(widthParam, value / maximumValue, widthKeyframesButton, getPosition())
        }
        Bossa.UndoButton {
            onClicked: widthSlider.value = widthDefault * widthSlider.maximumValue
        }
        Bossa.KeyframesButton {
            id: widthKeyframesButton
            onToggled: {
                enableControls(true);
                toggleKeyframes(checked, widthParam, widthSlider.value / widthSlider.maximumValue);
            }
        }

        Label {
            text: qsTr('Low cut')
            Layout.alignment: Qt.AlignRight
        }
        Bossa.SliderSpinner {
            id: lowCutSlider
            minimumValue: 0
            maximumValue: 100
            decimals: 1
            suffix: ' %'
            onValueChanged: updateFilter(lowCutParam, value / maximumValue, lowCutKeyframesButton, getPosition())
        }
        Bossa.UndoButton {
            onClicked: lowCutSlider.value = lowCutDefault * lowCutSlider.maximumValue
        }
        Bossa.KeyframesButton {
            id: lowCutKeyframesButton
            onToggled: {
                enableControls(true);
                toggleKeyframes(checked, lowCutParam, lowCutSlider.value / lowCutSlider.maximumValue);
            }
        }

        Label {
            text: qsTr('High cut')
            Layout.alignment: Qt.AlignRight
        }
        Bossa.SliderSpinner {
            id: highCutSlider
            minimumValue: 0
            maximumValue: 100
            decimals: 1
            suffix: ' %'
            onValueChanged: updateFilter(highCutParam, value / maximumValue, highCutKeyframesButton, getPosition())
        }
        Bossa.UndoButton {
            onClicked: highCutSlider.value = highCutDefault * highCutSlider.maximumValue
        }
        Bossa.KeyframesButton {
            id: highCutKeyframesButton
            onToggled: {
                enableControls(true);
                toggleKeyframes(checked, highCutParam, highCutSlider.value / highCutSlider.maximumValue);
            }
        }

        Label {
            text: qsTr('Mod rate')
            Layout.alignment: Qt.AlignRight
        }
        Bossa.SliderSpinner {
            id: modRateSlider
            minimumValue: 0
            maximumValue: 100
            decimals: 1
            suffix: ' %'
            onValueChanged: updateFilter(modRateParam, value / maximumValue, modRateKeyframesButton, getPosition())
        }
        Bossa.UndoButton {
            onClicked: modRateSlider.value = modRateDefault * modRateSlider.maximumValue
        }
        Bossa.KeyframesButton {
            id: modRateKeyframesButton
            onToggled: {
                enableControls(true);
                toggleKeyframes(checked, modRateParam, modRateSlider.value / modRateSlider.maximumValue);
            }
        }

        Label {
            text: qsTr('Mod depth')
            Layout.alignment: Qt.AlignRight
        }
        Bossa.SliderSpinner {
            id: modDepthSlider
            minimumValue: 0
            maximumValue: 100
            decimals: 1
            suffix: ' %'
            onValueChanged: updateFilter(modDepthParam, value / maximumValue, modDepthKeyframesButton, getPosition())
        }
        Bossa.UndoButton {
            onClicked: modDepthSlider.value = modDepthDefault * modDepthSlider.maximumValue
        }
        Bossa.KeyframesButton {
            id: modDepthKeyframesButton
            onToggled: {
                enableControls(true);
                toggleKeyframes(checked, modDepthParam, modDepthSlider.value / modDepthSlider.maximumValue);
            }
        }

        Label {
            text: qsTr('Mode')
            Layout.alignment: Qt.AlignRight
        }
        Bossa.SliderSpinner {
            id: modeSlider
            minimumValue: 0
            maximumValue: 100
            decimals: 1
            suffix: ' %'
            onValueChanged: updateFilter(modeParam, value / maximumValue, modeKeyframesButton, getPosition())
        }
        Bossa.UndoButton {
            onClicked: modeSlider.value = modeDefault * modeSlider.maximumValue
        }
        Bossa.KeyframesButton {
            id: modeKeyframesButton
            onToggled: {
                enableControls(true);
                toggleKeyframes(checked, modeParam, modeSlider.value / modeSlider.maximumValue);
            }
        }

        Label {
            text: qsTr('Wet/Dry')
            Layout.alignment: Qt.AlignRight
        }
        Bossa.SliderSpinner {
            id: wetnessSlider
            minimumValue: 0
            maximumValue: 100
            decimals: 1
            suffix: ' %'
            onValueChanged: updateFilter(wetnessParam, value / maximumValue, wetnessKeyframesButton, getPosition())
        }
        Bossa.UndoButton {
            onClicked: wetnessSlider.value = wetnessDefault * wetnessSlider.maximumValue
        }
        Bossa.KeyframesButton {
            id: wetnessKeyframesButton
            onToggled: {
                enableControls(true);
                toggleKeyframes(checked, wetnessParam, wetnessSlider.value / wetnessSlider.maximumValue);
            }
        }

        Label {
            text: qsTr('Channels')
            Layout.alignment: Qt.AlignRight
        }
        Bossa.ChannelMask {
            id: channelMask
            Layout.columnSpan: 3
        }

    }

    Connections {
        function onChanged() { setControls(); }
        function onInChanged() { updateSimpleKeyframes(); }
        function onOutChanged() { updateSimpleKeyframes(); }
        function onAnimateInChanged() { updateSimpleKeyframes(); }
        function onAnimateOutChanged() { updateSimpleKeyframes(); }
        function onPropertyChanged(name) { setControls(); }
        target: filter
    }

    Connections {
        function onPositionChanged() { setControls(); }
        target: producer
    }
}
