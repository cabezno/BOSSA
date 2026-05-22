import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import Bossa.Controls as Bossa

Bossa.KeyframableFilter {
    width: 350
    height: 1000
    keyframableParameters: ["topStart", "topEnd", "topBlendIn", "topBlendOut", "topFadeIn", "topBlurWidthStart", "topBlurWidthEnd", "topBlurHeightStart", "topBlurHeightEnd", "bottomStart", "bottomEnd", "bottomBlendIn", "bottomBlendOut", "bottomFadeIn", "bottomBlurWidthStart", "bottomBlurWidthEnd", "bottomBlurHeightStart", "bottomBlurHeightEnd"]
    startValues: [0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,]
    middleValues: [45, 80, 0, 10, 10, 0, 360, 0, 2, 45, 80, 0, 10, 10, 0, 360, 0, 2]
    endValues: [0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,]
    property var allParameters: [
        {
            name: "interpolation",
            type: "combobox",
            def: 1
        },
        {
            name: "topStart",
            type: "simple",
            def: 45
        },
        {
            name: "topEnd",
            type: "simple",
            def: 80
        },
        {
            name: "topFadeIn",
            type: "simple",
            def: 10
        },
        {
            name: "topBlendIn",
            type: "simple",
            def: 0
        },
        {
            name: "topBlendOut",
            type: "simple",
            def: 10
        },
        {
            name: "topBlurWidthStart",
            type: "simple",
            def: 0
        },
        {
            name: "topBlurHeightStart",
            type: "simple",
            def: 0
        },
        {
            name: "topBlurWidthEnd",
            type: "simple",
            def: 360
        },
        {
            name: "topBlurHeightEnd",
            type: "simple",
            def: 2
        },
        {
            name: "bottomStart",
            type: "simple",
            def: 45
        },
        {
            name: "bottomEnd",
            type: "simple",
            def: 80
        },
        {
            name: "bottomFadeIn",
            type: "simple",
            def: 10
        },
        {
            name: "bottomBlendIn",
            type: "simple",
            def: 0
        },
        {
            name: "bottomBlendOut",
            type: "simple",
            def: 10
        },
        {
            name: "bottomBlurWidthStart",
            type: "simple",
            def: 0
        },
        {
            name: "bottomBlurHeightStart",
            type: "simple",
            def: 0
        },
        {
            name: "bottomBlurWidthEnd",
            type: "simple",
            def: 360
        },
        {
            name: "bottomBlurHeightEnd",
            type: "simple",
            def: 2
        },
        {
            name: "topEnabled",
            type: "checkbox",
            def: true
        },
        {
            name: "bottomEnabled",
            type: "checkbox",
            def: true
        },
    ]
    function isKeyframeButtonChecked(control) {
        return filter.animateIn <= 0 && filter.animateOut <= 0 && filter.keyframeCount(control) > 0;
    }
    function setSimpleControl(parameter, controlSlider, controlKeyframeButton) {
        controlSlider.value = filter.getDouble(parameter, getPosition());
        controlKeyframeButton.checked = isKeyframeButtonChecked(parameter);
    }
    function setSimpleControlStatic(parameter, controlSlider) {
        controlSlider.value = filter.getDouble(parameter, getPosition());
    }
    function setSimpleComboBox(parameter, controlComboBox) {
        controlComboBox.currentIndex = filter.get(parameter);
    }
    function setSimpleCheckBox(parameter, controlCheckBox) {
        controlCheckBox.checked = filter.get(parameter) == "1";
    }
    function setSimpleTextField(parameter, controlTextField) {
        controlTextField.text = filter.get(parameter);
    }
    function setSimpleNumTextField(parameter, controlTextField) {
        controlTextField.text = filter.getDouble(parameter).toFixed(4);
    }
    function getFrameRate() {
        return producer.getDouble("meta.media.frame_rate_num", getPosition()) / producer.getDouble("meta.media.frame_rate_den", getPosition());
    }
    function getClipOffset() {
        return filter.in;
    }
    function getKeyframesButton(param) {
        return this["prop_" + param.name + "KeyframesButton"];
    }
    function getControl(param) {
        if (param.control != null && param.control !== undefined) {
            return this["prop_" + param.control];
        } else if (param.type == "simple" || param.type == "static") {
            return this["prop_" + param.name + "Slider"];
        } else if (param.type == "checkbox") {
            return this["prop_" + param.name + "CheckBox"];
        } else if (param.type == "combobox") {
            return this["prop_" + param.name + "ComboBox"];
        } else if (param.type == "textfield" || param.type == "numtextfield") {
            return this["prop_" + param.name + "TextField"];
        } else {
            return null;
        }
    }
    function enableControls(enabled) {
        for (var i = 0; i < allParameters.length; ++i) {
            var control = getControl(allParameters[i]);
            control.enabled = enabled;
        }
    }
    function defaultBeforePresetLoaded() {
        for (var i in preset.parameters) {
            filter.resetProperty(preset.parameters[i]);
        }
        resetSimpleKeyframes();
    }
    function defaultPresetSelected() {
        initializeSimpleKeyframes();
        setControls();
    }
    function setControlsForAllParameters() {
        for (var i = 0; i < allParameters.length; ++i) {
            var param = allParameters[i];
            var control = getControl(param);
            if (param.type == "simple") {
                setSimpleControl(param.name, control, getKeyframesButton(param));
            } else if (param.type == "static") {
                setSimpleControlStatic(param.name, control);
            } else if (param.type == "combobox") {
                setSimpleComboBox(param.name, control);
            } else if (param.type == "checkbox") {
                setSimpleCheckBox(param.name, control);
            } else if (param.type == "textfield") {
                setSimpleTextField(param.name, control);
            } else if (param.type == "numtextfield") {
                setSimpleNumTextField(param.name, control);
            }
        }
    }
    function setControls() {
        blockUpdate = true;
        setControlsForAllParameters();
        blockUpdate = false;
        enableControls(isSimpleKeyframesActive());
    }
    function defaultOnCompleted() {
        if (filter.isNew) {
            for (var i = 0; i < allParameters.length; ++i) {
                var param = allParameters[i];
                if (param.def != null) {
                    var control = getControl(param);
                    filter.set(param.name, param.def);
                }
            }
            filter.savePreset(preset.parameters);
        }
        setControls();
    }
    function updateSimpleKeyframes() {
        setControlsForAllParameters();
    }
    Component.onCompleted: {
        defaultOnCompleted();
    }
    function updateProperty_interpolation() {
        if (!blockUpdate) {
            filter.set("interpolation", interpolationComboBox.currentIndex);
        }
    }
    property alias prop_interpolationComboBox: interpolationComboBox
    function updateProperty_topEnabled() {
        if (!blockUpdate) {
            filter.set("topEnabled", topEnabledCheckBox.checked);
        }
    }
    property alias prop_topEnabledCheckBox: topEnabledCheckBox
    function updateProperty_topStart() {
        if (!blockUpdate) {
            updateFilter("topStart", topStartSlider.value, topStartKeyframesButton, getPosition());
        }
    }
    property alias prop_topStartSlider: topStartSlider
    property alias prop_topStartKeyframesButton: topStartKeyframesButton
    function updateProperty_topEnd() {
        if (!blockUpdate) {
            updateFilter("topEnd", topEndSlider.value, topEndKeyframesButton, getPosition());
        }
    }
    property alias prop_topEndSlider: topEndSlider
    property alias prop_topEndKeyframesButton: topEndKeyframesButton
    function updateProperty_topFadeIn() {
        if (!blockUpdate) {
            updateFilter("topFadeIn", topFadeInSlider.value, topFadeInKeyframesButton, getPosition());
        }
    }
    property alias prop_topFadeInSlider: topFadeInSlider
    property alias prop_topFadeInKeyframesButton: topFadeInKeyframesButton
    function updateProperty_topBlendIn() {
        if (!blockUpdate) {
            updateFilter("topBlendIn", topBlendInSlider.value, topBlendInKeyframesButton, getPosition());
        }
    }
    property alias prop_topBlendInSlider: topBlendInSlider
    property alias prop_topBlendInKeyframesButton: topBlendInKeyframesButton
    function updateProperty_topBlendOut() {
        if (!blockUpdate) {
            updateFilter("topBlendOut", topBlendOutSlider.value, topBlendOutKeyframesButton, getPosition());
        }
    }
    property alias prop_topBlendOutSlider: topBlendOutSlider
    property alias prop_topBlendOutKeyframesButton: topBlendOutKeyframesButton
    function updateProperty_topBlurWidthStart() {
        if (!blockUpdate) {
            updateFilter("topBlurWidthStart", topBlurWidthStartSlider.value, topBlurWidthStartKeyframesButton, getPosition());
        }
    }
    property alias prop_topBlurWidthStartSlider: topBlurWidthStartSlider
    property alias prop_topBlurWidthStartKeyframesButton: topBlurWidthStartKeyframesButton
    function updateProperty_topBlurHeightStart() {
        if (!blockUpdate) {
            updateFilter("topBlurHeightStart", topBlurHeightStartSlider.value, topBlurHeightStartKeyframesButton, getPosition());
        }
    }
    property alias prop_topBlurHeightStartSlider: topBlurHeightStartSlider
    property alias prop_topBlurHeightStartKeyframesButton: topBlurHeightStartKeyframesButton
    function updateProperty_topBlurWidthEnd() {
        if (!blockUpdate) {
            updateFilter("topBlurWidthEnd", topBlurWidthEndSlider.value, topBlurWidthEndKeyframesButton, getPosition());
        }
    }
    property alias prop_topBlurWidthEndSlider: topBlurWidthEndSlider
    property alias prop_topBlurWidthEndKeyframesButton: topBlurWidthEndKeyframesButton
    function updateProperty_topBlurHeightEnd() {
        if (!blockUpdate) {
            updateFilter("topBlurHeightEnd", topBlurHeightEndSlider.value, topBlurHeightEndKeyframesButton, getPosition());
        }
    }
    property alias prop_topBlurHeightEndSlider: topBlurHeightEndSlider
    property alias prop_topBlurHeightEndKeyframesButton: topBlurHeightEndKeyframesButton
    function updateProperty_bottomEnabled() {
        if (!blockUpdate) {
            filter.set("bottomEnabled", bottomEnabledCheckBox.checked);
        }
    }
    property alias prop_bottomEnabledCheckBox: bottomEnabledCheckBox
    function updateProperty_bottomStart() {
        if (!blockUpdate) {
            updateFilter("bottomStart", bottomStartSlider.value, bottomStartKeyframesButton, getPosition());
        }
    }
    property alias prop_bottomStartSlider: bottomStartSlider
    property alias prop_bottomStartKeyframesButton: bottomStartKeyframesButton
    function updateProperty_bottomEnd() {
        if (!blockUpdate) {
            updateFilter("bottomEnd", bottomEndSlider.value, bottomEndKeyframesButton, getPosition());
        }
    }
    property alias prop_bottomEndSlider: bottomEndSlider
    property alias prop_bottomEndKeyframesButton: bottomEndKeyframesButton
    function updateProperty_bottomFadeIn() {
        if (!blockUpdate) {
            updateFilter("bottomFadeIn", bottomFadeInSlider.value, bottomFadeInKeyframesButton, getPosition());
        }
    }
    property alias prop_bottomFadeInSlider: bottomFadeInSlider
    property alias prop_bottomFadeInKeyframesButton: bottomFadeInKeyframesButton
    function updateProperty_bottomBlendIn() {
        if (!blockUpdate) {
            updateFilter("bottomBlendIn", bottomBlendInSlider.value, bottomBlendInKeyframesButton, getPosition());
        }
    }
    property alias prop_bottomBlendInSlider: bottomBlendInSlider
    property alias prop_bottomBlendInKeyframesButton: bottomBlendInKeyframesButton
    function updateProperty_bottomBlendOut() {
        if (!blockUpdate) {
            updateFilter("bottomBlendOut", bottomBlendOutSlider.value, bottomBlendOutKeyframesButton, getPosition());
        }
    }
    property alias prop_bottomBlendOutSlider: bottomBlendOutSlider
    property alias prop_bottomBlendOutKeyframesButton: bottomBlendOutKeyframesButton
    function updateProperty_bottomBlurWidthStart() {
        if (!blockUpdate) {
            updateFilter("bottomBlurWidthStart", bottomBlurWidthStartSlider.value, bottomBlurWidthStartKeyframesButton, getPosition());
        }
    }
    property alias prop_bottomBlurWidthStartSlider: bottomBlurWidthStartSlider
    property alias prop_bottomBlurWidthStartKeyframesButton: bottomBlurWidthStartKeyframesButton
    function updateProperty_bottomBlurHeightStart() {
        if (!blockUpdate) {
            updateFilter("bottomBlurHeightStart", bottomBlurHeightStartSlider.value, bottomBlurHeightStartKeyframesButton, getPosition());
        }
    }
    property alias prop_bottomBlurHeightStartSlider: bottomBlurHeightStartSlider
    property alias prop_bottomBlurHeightStartKeyframesButton: bottomBlurHeightStartKeyframesButton
    function updateProperty_bottomBlurWidthEnd() {
        if (!blockUpdate) {
            updateFilter("bottomBlurWidthEnd", bottomBlurWidthEndSlider.value, bottomBlurWidthEndKeyframesButton, getPosition());
        }
    }
    property alias prop_bottomBlurWidthEndSlider: bottomBlurWidthEndSlider
    property alias prop_bottomBlurWidthEndKeyframesButton: bottomBlurWidthEndKeyframesButton
    function updateProperty_bottomBlurHeightEnd() {
        if (!blockUpdate) {
            updateFilter("bottomBlurHeightEnd", bottomBlurHeightEndSlider.value, bottomBlurHeightEndKeyframesButton, getPosition());
        }
    }
    property alias prop_bottomBlurHeightEndSlider: bottomBlurHeightEndSlider
    property alias prop_bottomBlurHeightEndKeyframesButton: bottomBlurHeightEndKeyframesButton
    GridLayout {
        columns: 4
        anchors.fill: parent
        anchors.margins: 8
        Label {
            text: qsTr('Preset')
            Layout.alignment: Qt.AlignRight
        }
        Bossa.Preset {
            id: preset
            parameters: ["interpolation", "topStart", "topEnd", "topBlendIn", "topBlendOut", "topFadeIn", "topBlurWidthStart", "topBlurWidthEnd", "topBlurHeightStart", "topBlurHeightEnd", "bottomStart", "bottomEnd", "bottomBlendIn", "bottomBlendOut", "bottomFadeIn", "bottomBlurWidthStart", "bottomBlurWidthEnd", "bottomBlurHeightStart", "bottomBlurHeightEnd", "topEnabled", "bottomEnabled"]
            Layout.columnSpan: 3
            onBeforePresetLoaded: {
                defaultBeforePresetLoaded();
            }
            onPresetSelected: {
                defaultPresetSelected();
            }
        }
        Label {
            text: qsTr('Interpolation')
            Layout.alignment: Qt.AlignRight
        }
        ComboBox {
            id: interpolationComboBox
            currentIndex: 0
            model: ["Nearest-neighbor", "Bilinear"]
            Layout.columnSpan: 2
            onCurrentIndexChanged: updateProperty_interpolation()
        }
        Bossa.UndoButton {
            id: interpolationUndo
            onClicked: interpolationComboBox.currentIndex = 1
        }
        Label {
            text: qsTr('Top')
            Layout.alignment: Qt.AlignLeft
        }
        CheckBox {
            id: topEnabledCheckBox
            text: qsTr('')
            checked: true
            Layout.columnSpan: 3
            onCheckedChanged: updateProperty_topEnabled()
        }
        Label {
            text: qsTr('Start')
            Layout.alignment: Qt.AlignRight
        }
        Bossa.SliderSpinner {
            id: topStartSlider
            minimumValue: 0
            maximumValue: 90
            spinnerWidth: 120
            suffix: ' deg'
            decimals: 3
            stepSize: 1
            onValueChanged: updateProperty_topStart()
        }
        Bossa.KeyframesButton {
            id: topStartKeyframesButton
            checked: isKeyframeButtonChecked("topStart")
            onToggled: {
                enableControls(true);
                toggleKeyframes(checked, "topStart", topStartSlider.value);
            }
        }
        Bossa.UndoButton {
            id: topStartUndo
            onClicked: topStartSlider.value = 45
        }
        Label {
            text: qsTr('End')
            Layout.alignment: Qt.AlignRight
        }
        Bossa.SliderSpinner {
            id: topEndSlider
            minimumValue: 0
            maximumValue: 90
            spinnerWidth: 120
            suffix: ' deg'
            decimals: 3
            stepSize: 1
            onValueChanged: updateProperty_topEnd()
        }
        Bossa.KeyframesButton {
            id: topEndKeyframesButton
            checked: isKeyframeButtonChecked("topEnd")
            onToggled: {
                enableControls(true);
                toggleKeyframes(checked, "topEnd", topEndSlider.value);
            }
        }
        Bossa.UndoButton {
            id: topEndUndo
            onClicked: topEndSlider.value = 80
        }
        Label {
            text: qsTr('Fade')
            Layout.alignment: Qt.AlignRight
        }
        Bossa.SliderSpinner {
            id: topFadeInSlider
            minimumValue: 0
            maximumValue: 90
            spinnerWidth: 120
            suffix: ' deg'
            decimals: 3
            stepSize: 1
            onValueChanged: updateProperty_topFadeIn()
        }
        Bossa.KeyframesButton {
            id: topFadeInKeyframesButton
            checked: isKeyframeButtonChecked("topFadeIn")
            onToggled: {
                enableControls(true);
                toggleKeyframes(checked, "topFadeIn", topFadeInSlider.value);
            }
        }
        Bossa.UndoButton {
            id: topFadeInUndo
            onClicked: topFadeInSlider.value = 10
        }
        Label {
            text: qsTr('Blend')
            Layout.alignment: Qt.AlignRight
        }
        Label {
            text: qsTr('')
            Layout.alignment: Qt.AlignRight
            Layout.columnSpan: 3
        }
        Label {
            text: qsTr('In')
            Layout.alignment: Qt.AlignRight
        }
        Bossa.SliderSpinner {
            id: topBlendInSlider
            minimumValue: 0
            maximumValue: 90
            spinnerWidth: 120
            suffix: ' deg'
            decimals: 3
            stepSize: 1
            onValueChanged: updateProperty_topBlendIn()
        }
        Bossa.KeyframesButton {
            id: topBlendInKeyframesButton
            checked: isKeyframeButtonChecked("topBlendIn")
            onToggled: {
                enableControls(true);
                toggleKeyframes(checked, "topBlendIn", topBlendInSlider.value);
            }
        }
        Bossa.UndoButton {
            id: topBlendInUndo
            onClicked: topBlendInSlider.value = 0
        }
        Label {
            text: qsTr('Out')
            Layout.alignment: Qt.AlignRight
        }
        Bossa.SliderSpinner {
            id: topBlendOutSlider
            minimumValue: 0
            maximumValue: 90
            spinnerWidth: 120
            suffix: ' deg'
            decimals: 3
            stepSize: 1
            onValueChanged: updateProperty_topBlendOut()
        }
        Bossa.KeyframesButton {
            id: topBlendOutKeyframesButton
            checked: isKeyframeButtonChecked("topBlendOut")
            onToggled: {
                enableControls(true);
                toggleKeyframes(checked, "topBlendOut", topBlendOutSlider.value);
            }
        }
        Bossa.UndoButton {
            id: topBlendOutUndo
            onClicked: topBlendOutSlider.value = 10
        }
        Label {
            text: qsTr('Blur')
            Layout.alignment: Qt.AlignRight
        }
        Label {
            text: qsTr('')
            Layout.alignment: Qt.AlignRight
            Layout.columnSpan: 3
        }
        Label {
            text: qsTr('Width at start')
            Layout.alignment: Qt.AlignRight
        }
        Bossa.SliderSpinner {
            id: topBlurWidthStartSlider
            minimumValue: 0
            maximumValue: 360
            spinnerWidth: 120
            suffix: ' deg'
            decimals: 3
            stepSize: 1
            onValueChanged: updateProperty_topBlurWidthStart()
        }
        Bossa.KeyframesButton {
            id: topBlurWidthStartKeyframesButton
            checked: isKeyframeButtonChecked("topBlurWidthStart")
            onToggled: {
                enableControls(true);
                toggleKeyframes(checked, "topBlurWidthStart", topBlurWidthStartSlider.value);
            }
        }
        Bossa.UndoButton {
            id: topBlurWidthStartUndo
            onClicked: topBlurWidthStartSlider.value = 0
        }
        Label {
            text: qsTr('Height at start')
            Layout.alignment: Qt.AlignRight
        }
        Bossa.SliderSpinner {
            id: topBlurHeightStartSlider
            minimumValue: 0
            maximumValue: 90
            spinnerWidth: 120
            suffix: ' deg'
            decimals: 3
            stepSize: 1
            onValueChanged: updateProperty_topBlurHeightStart()
        }
        Bossa.KeyframesButton {
            id: topBlurHeightStartKeyframesButton
            checked: isKeyframeButtonChecked("topBlurHeightStart")
            onToggled: {
                enableControls(true);
                toggleKeyframes(checked, "topBlurHeightStart", topBlurHeightStartSlider.value);
            }
        }
        Bossa.UndoButton {
            id: topBlurHeightStartUndo
            onClicked: topBlurHeightStartSlider.value = 0
        }
        Label {
            text: qsTr('Width at end')
            Layout.alignment: Qt.AlignRight
        }
        Bossa.SliderSpinner {
            id: topBlurWidthEndSlider
            minimumValue: 0
            maximumValue: 360
            spinnerWidth: 120
            suffix: ' deg'
            decimals: 3
            stepSize: 1
            onValueChanged: updateProperty_topBlurWidthEnd()
        }
        Bossa.KeyframesButton {
            id: topBlurWidthEndKeyframesButton
            checked: isKeyframeButtonChecked("topBlurWidthEnd")
            onToggled: {
                enableControls(true);
                toggleKeyframes(checked, "topBlurWidthEnd", topBlurWidthEndSlider.value);
            }
        }
        Bossa.UndoButton {
            id: topBlurWidthEndUndo
            onClicked: topBlurWidthEndSlider.value = 360
        }
        Label {
            text: qsTr('Height at end')
            Layout.alignment: Qt.AlignRight
        }
        Bossa.SliderSpinner {
            id: topBlurHeightEndSlider
            minimumValue: 0
            maximumValue: 90
            spinnerWidth: 120
            suffix: ' deg'
            decimals: 3
            stepSize: 1
            onValueChanged: updateProperty_topBlurHeightEnd()
        }
        Bossa.KeyframesButton {
            id: topBlurHeightEndKeyframesButton
            checked: isKeyframeButtonChecked("topBlurHeightEnd")
            onToggled: {
                enableControls(true);
                toggleKeyframes(checked, "topBlurHeightEnd", topBlurHeightEndSlider.value);
            }
        }
        Bossa.UndoButton {
            id: topBlurHeightEndUndo
            onClicked: topBlurHeightEndSlider.value = 2
        }
        Label {
            text: qsTr('Bottom')
            Layout.alignment: Qt.AlignLeft
        }
        CheckBox {
            id: bottomEnabledCheckBox
            text: qsTr('')
            checked: true
            Layout.columnSpan: 3
            onCheckedChanged: updateProperty_bottomEnabled()
        }
        Label {
            text: qsTr('Start')
            Layout.alignment: Qt.AlignRight
        }
        Bossa.SliderSpinner {
            id: bottomStartSlider
            minimumValue: 0
            maximumValue: 90
            spinnerWidth: 120
            suffix: ' deg'
            decimals: 3
            stepSize: 1
            onValueChanged: updateProperty_bottomStart()
        }
        Bossa.KeyframesButton {
            id: bottomStartKeyframesButton
            checked: isKeyframeButtonChecked("bottomStart")
            onToggled: {
                enableControls(true);
                toggleKeyframes(checked, "bottomStart", bottomStartSlider.value);
            }
        }
        Bossa.UndoButton {
            id: bottomStartUndo
            onClicked: bottomStartSlider.value = 45
        }
        Label {
            text: qsTr('End')
            Layout.alignment: Qt.AlignRight
        }
        Bossa.SliderSpinner {
            id: bottomEndSlider
            minimumValue: 0
            maximumValue: 90
            spinnerWidth: 120
            suffix: ' deg'
            decimals: 3
            stepSize: 1
            onValueChanged: updateProperty_bottomEnd()
        }
        Bossa.KeyframesButton {
            id: bottomEndKeyframesButton
            checked: isKeyframeButtonChecked("bottomEnd")
            onToggled: {
                enableControls(true);
                toggleKeyframes(checked, "bottomEnd", bottomEndSlider.value);
            }
        }
        Bossa.UndoButton {
            id: bottomEndUndo
            onClicked: bottomEndSlider.value = 80
        }
        Label {
            text: qsTr('Fade')
            Layout.alignment: Qt.AlignRight
        }
        Bossa.SliderSpinner {
            id: bottomFadeInSlider
            minimumValue: 0
            maximumValue: 90
            spinnerWidth: 120
            suffix: ' deg'
            decimals: 3
            stepSize: 1
            onValueChanged: updateProperty_bottomFadeIn()
        }
        Bossa.KeyframesButton {
            id: bottomFadeInKeyframesButton
            checked: isKeyframeButtonChecked("bottomFadeIn")
            onToggled: {
                enableControls(true);
                toggleKeyframes(checked, "bottomFadeIn", bottomFadeInSlider.value);
            }
        }
        Bossa.UndoButton {
            id: bottomFadeInUndo
            onClicked: bottomFadeInSlider.value = 0
        }
        Label {
            text: qsTr('Blend')
            Layout.alignment: Qt.AlignRight
        }
        Label {
            text: qsTr('')
            Layout.alignment: Qt.AlignRight
            Layout.columnSpan: 3
        }
        Label {
            text: qsTr('In')
            Layout.alignment: Qt.AlignRight
        }
        Bossa.SliderSpinner {
            id: bottomBlendInSlider
            minimumValue: 0
            maximumValue: 90
            spinnerWidth: 120
            suffix: ' deg'
            decimals: 3
            stepSize: 1
            onValueChanged: updateProperty_bottomBlendIn()
        }
        Bossa.KeyframesButton {
            id: bottomBlendInKeyframesButton
            checked: isKeyframeButtonChecked("bottomBlendIn")
            onToggled: {
                enableControls(true);
                toggleKeyframes(checked, "bottomBlendIn", bottomBlendInSlider.value);
            }
        }
        Bossa.UndoButton {
            id: bottomBlendInUndo
            onClicked: bottomBlendInSlider.value = 10
        }
        Label {
            text: qsTr('Out')
            Layout.alignment: Qt.AlignRight
        }
        Bossa.SliderSpinner {
            id: bottomBlendOutSlider
            minimumValue: 0
            maximumValue: 90
            spinnerWidth: 120
            suffix: ' deg'
            decimals: 3
            stepSize: 1
            onValueChanged: updateProperty_bottomBlendOut()
        }
        Bossa.KeyframesButton {
            id: bottomBlendOutKeyframesButton
            checked: isKeyframeButtonChecked("bottomBlendOut")
            onToggled: {
                enableControls(true);
                toggleKeyframes(checked, "bottomBlendOut", bottomBlendOutSlider.value);
            }
        }
        Bossa.UndoButton {
            id: bottomBlendOutUndo
            onClicked: bottomBlendOutSlider.value = 10
        }
        Label {
            text: qsTr('Blur')
            Layout.alignment: Qt.AlignRight
        }
        Label {
            text: qsTr('')
            Layout.alignment: Qt.AlignRight
            Layout.columnSpan: 3
        }
        Label {
            text: qsTr('Width at start')
            Layout.alignment: Qt.AlignRight
        }
        Bossa.SliderSpinner {
            id: bottomBlurWidthStartSlider
            minimumValue: 0
            maximumValue: 360
            spinnerWidth: 120
            suffix: ' deg'
            decimals: 3
            stepSize: 1
            onValueChanged: updateProperty_bottomBlurWidthStart()
        }
        Bossa.KeyframesButton {
            id: bottomBlurWidthStartKeyframesButton
            checked: isKeyframeButtonChecked("bottomBlurWidthStart")
            onToggled: {
                enableControls(true);
                toggleKeyframes(checked, "bottomBlurWidthStart", bottomBlurWidthStartSlider.value);
            }
        }
        Bossa.UndoButton {
            id: bottomBlurWidthStartUndo
            onClicked: bottomBlurWidthStartSlider.value = 0
        }
        Label {
            text: qsTr('Height at start')
            Layout.alignment: Qt.AlignRight
        }
        Bossa.SliderSpinner {
            id: bottomBlurHeightStartSlider
            minimumValue: 0
            maximumValue: 90
            spinnerWidth: 120
            suffix: ' deg'
            decimals: 3
            stepSize: 1
            onValueChanged: updateProperty_bottomBlurHeightStart()
        }
        Bossa.KeyframesButton {
            id: bottomBlurHeightStartKeyframesButton
            checked: isKeyframeButtonChecked("bottomBlurHeightStart")
            onToggled: {
                enableControls(true);
                toggleKeyframes(checked, "bottomBlurHeightStart", bottomBlurHeightStartSlider.value);
            }
        }
        Bossa.UndoButton {
            id: bottomBlurHeightStartUndo
            onClicked: bottomBlurHeightStartSlider.value = 0
        }
        Label {
            text: qsTr('Width at end')
            Layout.alignment: Qt.AlignRight
        }
        Bossa.SliderSpinner {
            id: bottomBlurWidthEndSlider
            minimumValue: 0
            maximumValue: 360
            spinnerWidth: 120
            suffix: ' deg'
            decimals: 3
            stepSize: 1
            onValueChanged: updateProperty_bottomBlurWidthEnd()
        }
        Bossa.KeyframesButton {
            id: bottomBlurWidthEndKeyframesButton
            checked: isKeyframeButtonChecked("bottomBlurWidthEnd")
            onToggled: {
                enableControls(true);
                toggleKeyframes(checked, "bottomBlurWidthEnd", bottomBlurWidthEndSlider.value);
            }
        }
        Bossa.UndoButton {
            id: bottomBlurWidthEndUndo
            onClicked: bottomBlurWidthEndSlider.value = 360
        }
        Label {
            text: qsTr('Height at end')
            Layout.alignment: Qt.AlignRight
        }
        Bossa.SliderSpinner {
            id: bottomBlurHeightEndSlider
            minimumValue: 0
            maximumValue: 90
            spinnerWidth: 120
            suffix: ' deg'
            decimals: 3
            stepSize: 1
            onValueChanged: updateProperty_bottomBlurHeightEnd()
        }
        Bossa.KeyframesButton {
            id: bottomBlurHeightEndKeyframesButton
            checked: isKeyframeButtonChecked("bottomBlurHeightEnd")
            onToggled: {
                enableControls(true);
                toggleKeyframes(checked, "bottomBlurHeightEnd", bottomBlurHeightEndSlider.value);
            }
        }
        Bossa.UndoButton {
            id: bottomBlurHeightEndUndo
            onClicked: bottomBlurHeightEndSlider.value = 2
        }
    }
    Connections {
        target: filter
        onInChanged: updateSimpleKeyframes()
        onOutChanged: updateSimpleKeyframes()
        onAnimateInChanged: updateSimpleKeyframes()
        onAnimateOutChanged: updateSimpleKeyframes()
        onChanged: setControls()
    }
    Connections {
        target: producer
        onPositionChanged: setControls()
    }
}
