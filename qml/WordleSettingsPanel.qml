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

                Column
                {
                    width: parent.columnWidth

                    ComboBox {
                        id: languageComboBox

                        //: Combo box label
                        //% "Dictionary"
                        label: qsTrId("wordle-settings-language-label")
                        width: parent.width
                        menu: ContextMenu {
                            id: languageMenu

                            x: 0
                            width: languageComboBox.width

                            Repeater {
                                model: WordleLanguageModel { }
                                MenuItem {
                                    readonly property string languageCode: model.languageCode
                                    readonly property string description: formatDescription(model.wordCount, model.extraWordCount)
                                    text: model.languageName
                                    onClicked: WordleSettings.language = languageCode
                                    function formatDescription(words,xwords) {
                                        return xwords ?
                                            //: Language descriptions (word counts)
                                            //% "%1 + %2 words"
                                            qsTrId("wordle-settings-language-description2", xwords).arg(words).arg(xwords) :
                                            //: Language descriptions (word count)
                                            //% "%1 words"
                                            qsTrId("wordle-settings-language-description", words).arg(words)
                                    }
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
                        width: parent.width
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
                }

                Column
                {
                    width: parent.columnWidth

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
                        description: HarbourBattery.batteryLevel > 0 ? qsTrId("wordle-settings-keep_display_on-description").arg(WordleSettings.MinChargeToKeepDisplayOn) : ""
                        onClicked: WordleSettings.keepDisplayOn = !WordleSettings.keepDisplayOn
                    }

                    TextSwitch {
                        id: searchEngineSwitch

                        width: parent.width
                        automaticCheck: false
                        checked: WordleSettings.whatsThis
                        //: Text switch label
                        //% "Enable «What's this» menu"
                        text: qsTrId("wordle-settings-show_whats_this")
                        //: Text switch label description
                        //% "If the word which you were guessing isn't familiar to you, this function allows you to quickly look up the meaning of the word in the selected search engine."
                        description: qsTrId("wordle-settings-show_whats_this-description")
                        onClicked: WordleSettings.whatsThis = !WordleSettings.whatsThis
                    }

                    ComboBox {
                        id: searchEngineComboBox

                        x: Theme.itemSizeExtraSmall + searchEngineSwitch.leftMargin - Theme.paddingLarge
                        width: parent.width - x
                        leftMargin: 0
                        visible: WordleSettings.whatsThis
                        //: Combo box label
                        //% "Search engine"
                        label: qsTrId("wordle-settings-search_engine-label")
                        menu: ContextMenu {
                            id: searchEngineMenu

                            x: 0
                            width: searchEngineComboBox.width

                            Repeater {
                                model: WordleSearchEngineModel { }
                                MenuItem {
                                    readonly property string ident: model.ident
                                    readonly property bool isDefault: model.isDefault
                                    text: model.name
                                    onClicked: WordleSettings.searchEngine = model.ident
                                }
                            }
                        }

                        Component.onCompleted: updateValue()

                        function updateValue() {
                            var itemFound = null
                            var items = searchEngineMenu.children
                            if (items) {
                                for (var i = 0; i < items.length; i++) {
                                    var item = items[i]
                                    if (item.ident === WordleSettings.searchEngine) {
                                        itemFound = item
                                        break;
                                    } else if (item.isDefault) {
                                        // Will use this one if we don't find anything better than that
                                        itemFound = item
                                    }
                                }
                            }
                            currentItem = itemFound
                        }

                        Connections {
                            target: WordleSettings
                            onSearchEngineChanged: searchEngineComboBox.updateValue()
                        }
                    }
                }
            }

            VerticalScrollDecorator { }
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
            text: qsTrId("wordle-settings-version").arg("1.1.8")
        }
    }
}
