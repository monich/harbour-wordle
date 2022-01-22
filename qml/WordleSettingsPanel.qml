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
                    id: keepDisplayOnSwitch

                    width: parent.width
                    automaticCheck: false
                    checked: WordleSettings.keepDisplayOn
                    //: Text switch label
                    //% "Keep display on while playing"
                    text: qsTrId("wordle-settings-keep_display_on")
                    onClicked: WordleSettings.keepDisplayOn = !WordleSettings.keepDisplayOn
                }
            }
        }
    }
}
