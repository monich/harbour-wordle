import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.wordle 1.0

Item {
    id: thisItem

    property bool landscape

    signal flip()

    SilicaFlickable {
        anchors.fill: parent

        Label {
            id: header

            y: Theme.paddingLarge
            x: Theme.horizontalPageMargin
            color: Theme.highlightColor
            font {
                capitalization: Font.AllUppercase
                pixelSize: Theme.fontSizeLarge
                family: Theme.fontFamilyHeading
                weight: Font.Black
            }
            //: Page header
            //% "Settings"
            text: qsTrId("wordle-settings-header")
        }

        IconButton {
            anchors {
                right: parent.right
                rightMargin: Theme.paddingMedium
                verticalCenter: header.verticalCenter
            }
            icon.source: "image://theme/icon-m-acknowledge"
            onClicked: thisItem.flip()
        }

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

        Label {
            id: versionLabel

            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: parent.bottom
                bottomMargin: Theme.paddingMedium
            }
            font.pixelSize: Theme.fontSizeExtraSmall
            color: Theme.rgba(Theme.highlightColor, 0.4)
            //: Small description label (app version)
            //% "Version %1"
            text: qsTrId("wordle-settings-version").arg("1.1.2")
        }
    }
}
