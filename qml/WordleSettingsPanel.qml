import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.wordle 1.0

Item {
    id: thisItem

    property bool landscape

    signal flip()

    Rectangle {
        id: panelBorder

        anchors {
            fill: parent
            margins: Theme.paddingMedium
        }
        color: Theme.rgba(Theme.highlightBackgroundColor, 0.1)
        border {
            color: Theme.rgba(Theme.highlightColor, 0.4)
            width: Math.max(2, Math.floor(Theme.paddingSmall/3))
        }
        radius: Theme.paddingMedium

        SilicaFlickable {
            x: panelBorder.border.width
            y: x
            width: parent.width - 2 * x
            height: parent.height - 2 * y

            Label {
                id: header

                y: Theme.paddingLarge
                x: Theme.horizontalPageMargin
                color: Theme.highlightColor
                font {
                    pixelSize: Theme.fontSizeLarge
                    family: Theme.fontFamilyHeading
                }
                //: Page header
                //% "Settings"
                text: qsTrId("wordle-settings-header")
            }

            IconButton {
                id: okButton

                anchors {
                    right: parent.right
                    rightMargin: Theme.paddingMedium
                    verticalCenter: header.verticalCenter
                }
                icon.source: "image://theme/icon-m-acknowledge"
                onClicked: thisItem.flip()
            }

            Column {
                id: column

                anchors{
                    top: header.bottom
                    topMargin: Theme.paddingLarge
                }
                width: parent.width

                ComboBox {
                    id: languageComboBox

                    //: Combo box label
                    //% "Dictionary"
                    label: qsTrId("wordle-settings-language-label")
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

                TextSwitch {
                    width: parent.width
                    automaticCheck: false
                    checked: WordleSettings.showPlayTime
                    //: Text switch label
                    //% "Show timer"
                    text: qsTrId("wordle-settings-show_play_time")
                    onClicked: WordleSettings.showPlayTime = !WordleSettings.showPlayTime
                }

                TextSwitch {
                    width: parent.width
                    automaticCheck: false
                    checked: WordleSettings.keepDisplayOn
                    //: Text switch label
                    //% "Keep display on while playing"
                    text: qsTrId("wordle-settings-keep_display_on")
                    //: Text switch label description
                    //% "To avoid completely discharging the battery, display blanking would still be allowed if the battery level drops below %1% and the phone is not on charger."
                    description: HarbourBattery.batteryLevel > 0 ? qsTrId("wordle-settings-keep_display_on-description").arg(20) : ""
                    onClicked: WordleSettings.keepDisplayOn = !WordleSettings.keepDisplayOn
                }
            }

            Label {
                anchors {
                    horizontalCenter: parent.horizontalCenter
                    bottom: parent.bottom
                    bottomMargin: Theme.paddingMedium
                }
                font.pixelSize: Theme.fontSizeExtraSmall
                color: panelBorder.border.color
                //: Small description label (app version)
                //% "Version %1"
                text: qsTrId("wordle-settings-version").arg("1.0.13")
            }
        }
    }
}
