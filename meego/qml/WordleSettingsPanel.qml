import QtQuick 1.1
import com.nokia.meego 1.0
import harbour.wordle 1.0

Item {
    id: thisItem

    property bool landscape

    signal flip()

    Label {
        id: header

        y: WordleTheme.paddingLarge
        x: WordleTheme.horizontalPageMargin
        color: WordleTheme.highlightColor
        font {
            capitalization: Font.AllUppercase
            pixelSize: WordleTheme.fontSizeLarge
            weight: Font.Black
        }
        //: Page header
        //% "Settings"
        text: qsTrId("wordle-settings-header")
    }

    IconButton {
        anchors {
            right: parent.right
            rightMargin: WordleTheme.paddingMedium
            verticalCenter: header.verticalCenter
        }
        iconSource: "images/ok.svg"
        onClicked: thisItem.flip()
    }

    Column {

        width: parent.width
        anchors{
            top: header.bottom
            topMargin: WordleTheme.paddingLarge
            bottom: versionLabel.top
            bottomMargin: WordleTheme.paddingMedium
        }
        spacing: WordleTheme.paddingMedium

        ValueButton {
            width: parent.width
            label: qsTrId("wordle-settings-language-label")
            value: WordleSettings.languageName
            onValueChanged: languageSelection.selectedIndex = languageModel.indexOf(value)
            onClicked: {
                languageSelection.selectedIndex = languageModel.indexOf(WordleSettings.language)
                languageSelection.open()
            }
        }

        TextSwitch {
            id: showPlayTimeSwitch

            anchors{
                left: parent.left
                leftMargin: WordleTheme.horizontalPageMargin
                right: parent.right
                rightMargin: WordleTheme.horizontalPageMargin
            }
            text: qsTrId("wordle-settings-show_play_time")
            checked: WordleSettings.showPlayTime
            onClicked: WordleSettings.showPlayTime = checked
        }

        TextSwitch {
            id: keepDisplayOnSwitch

            anchors{
                left: parent.left
                leftMargin: WordleTheme.horizontalPageMargin
                right: parent.right
                rightMargin: WordleTheme.horizontalPageMargin
            }
            text: qsTrId("wordle-settings-keep_display_on")
            checked: WordleSettings.keepDisplayOn
            onClicked: WordleSettings.keepDisplayOn = checked
        }

        Connections {
            target: WordleSettings
            onShowPlayTimeChanged: showPlayTimeSwitch.checked = WordleSettings.showPlayTime
            onKeepDisplayOnChanged: keepDisplayOnSwitch.checked = WordleSettings.keepDisplayOn
        }
    }

    WordleLanguageModel {
        id: languageModel
    }

    SelectionDialog {
        id: languageSelection

        //: Combo box label
        //% "Dictionary"
        titleText: qsTrId("wordle-settings-language-label")
        model: languageModel
        onAccepted: WordleSettings.language = languageModel.languageAt(selectedIndex)
    }

    /*
        SilicaFlickable {
            contentHeight: content.height
            width: parent.width
            clip: true
            anchors{
                top: header.bottom
                topMargin: Theme.paddingLarge
                bottom: versionLabel.top
                bottomMargin: Theme.paddingMedium
            }

            Grid {
                id: content

                readonly property real columnWidth: width/columns

                width: parent.width
                columns: landscape ? 2 : 1
                flow: Grid.TopToBottom

                ComboBox {
                    id: languageComboBox

                    //: Combo box label
                    //% "Dictionary"
                    label: qsTrId("wordle-settings-language-label")
                    width: parent.columnWidth
                    menu: ContextMenu {
                        id: languageMenu

                        x: 0
                        width: languageComboBox.width

                        Repeater {
                            model: WordleLanguageModel { }
                            MenuItem {
                                readonly property string languageCode: model.languageCode
                                text: model.languageName
                                onClicked: WordleSettings.language = languageCode
                            }
                        }
                    }

                    property int ignoreCurrentItemChange

                    Component.onCompleted: updateValue()

                    onCurrentItemChanged: updateValue()

                    function updateValue() {
                        if (!ignoreCurrentItemChange) {
                            var itemFound = null
                            var items = languageMenu.children
                            if (items) {
                                for (var i = 0; i < items.length; i++) {
                                    if (items[i].languageCode === WordleSettings.language) {
                                        itemFound = items[i]
                                        break;
                                    }
                                }
                            }
                            // Prevent recursion
                            ignoreCurrentItemChange++
                            currentItem = itemFound
                            ignoreCurrentItemChange--
                        }
                    }

                    Connections {
                        target: WordleSettings
                        onLanguageChanged: languageComboBox.updateValue()
                    }
                }

                ComboBox {
                    id: orientationComboBox

                    //: Combo box label
                    //% "Orientation"
                    label: qsTrId("wordle-settings-orientation-label")
                    value: currentItem ? currentItem.text : ""
                    width: parent.columnWidth
                    menu: ContextMenu {
                        x: 0
                        width: orientationComboBox.width

                        MenuItem {
                            readonly property int value: WordleSettings.OrientationAny
                            //: Combo box value for dynamic orientation
                            //% "Dynamic"
                            text: qsTrId("wordle-settings-orientation-dynamic")
                            onClicked: WordleSettings.orientation = value
                        }
                        MenuItem {
                            readonly property int value: WordleSettings.OrientationPortrait
                            //: Combo box value for portrait orientation
                            //% "Portrait"
                            text: qsTrId("wordle-settings-orientation-portrait")
                            onClicked: WordleSettings.orientation = value
                        }
                        MenuItem {
                            readonly property int value: WordleSettings.OrientationLandscape
                            //: Combo box value for landscape orientation
                            //% "Landscape"
                            text: qsTrId("wordle-settings-orientation-landscape")
                            onClicked: WordleSettings.orientation = value
                        }
                    }

                    Component.onCompleted: updateValue()

                    function updateValue() {
                        var index = 0
                        var items = languageMenu.children
                        if (items) {
                            for (var i = 0; i < items.length; i++) {
                                if (items[i].value === value) {
                                    index = i
                                    break
                                }
                            }
                        }
                        orientationComboBox.currentIndex = index
                    }

                    Connections {
                        target: WordleSettings
                        onOrientationChanged: orientationComboBox.updateValue()
                    }
                }

                TextSwitch {
                    width: parent.columnWidth
                    automaticCheck: false
                    checked: WordleSettings.showPlayTime
                    //: Text switch label
                    //% "Show timer"
                    text: qsTrId("wordle-settings-show_play_time")
                    onClicked: WordleSettings.showPlayTime = !WordleSettings.showPlayTime
                }

                TextSwitch {
                    width: parent.columnWidth
                    automaticCheck: false
                    checked: WordleSettings.keepDisplayOn
                    //: Text switch label
                    //% "Keep display on while playing"
                    text: qsTrId("wordle-settings-keep_display_on")
                    //: Text switch label description
                    //% "To avoid completely discharging the battery, display blanking would still be allowed if the battery level drops below %1% and the phone is not on charger."
                    description: HarbourBattery.batteryLevel > 0 ? qsTrId("wordle-settings-keep_display_on-description").arg(WordleSettings.MinChangeToKeepDisplayOn) : ""
                    onClicked: WordleSettings.keepDisplayOn = !WordleSettings.keepDisplayOn
                }
            }
        }

*/

    Label {
        id: versionLabel

        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            bottomMargin: WordleTheme.paddingMedium
        }
        font.pixelSize: WordleTheme.fontSizeSmall
        color: WordleTheme.rgba(WordleTheme.highlightColor, 0.4)
        //: Small description label (app version)
        //% "Version %1"
        text: qsTrId("wordle-settings-version").arg("1.1.8")
    }
}
