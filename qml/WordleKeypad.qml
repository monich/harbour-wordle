import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.wordle 1.0

Column {
    id: thisItem

    property bool landscape
    property var wordle

    signal keyPressed(var letter)

    spacing: Theme.paddingMedium

    Repeater {
        id: rows

        model: thisItem.wordle.keypad
        delegate: Row {
            id: row

            // Functional key width is 2 * baseKeyWidth + spacing
            readonly property string buttons: modelData
            readonly property int functionalKeys: Wordle.functionalKeyCount(buttons)
            readonly property int baseKeyWidth: (thisItem.width + spacing) / (buttons.length + functionalKeys) - spacing

            spacing: Theme.paddingMedium

            Repeater {
                model: row.buttons.length

                Loader {
                    id: key

                    readonly property string letter: row.buttons.charAt(index)
                    readonly property bool functionalKey: Wordle.isFunctionalKey(letter)

                    width: row.baseKeyWidth + (functionalKey ? (row.baseKeyWidth + row.spacing) : 0)
                    height: landscape ? Theme.itemSizeExtraSmall : Theme.itemSizeSmall

                    sourceComponent: !functionalKey ? letterKeyComponent :
                        letter === '\n' ? enterKeyComponent :
                        letter === '\b' ? backspaceKeyComponent : null

                    Binding {
                        target: key.item
                        when: !key.functionalKey
                        property: "letter"
                        value: key.letter
                    }

                    Binding {
                        target: key.item
                        property: "wordle"
                        value: thisItem.wordle
                    }

                    Connections {
                        target: key.item
                        onPressed: {
                            buzz.item.play()
                            thisItem.keyPressed(key.letter)
                        }
                    }
                }
            }
        }
    }

    Loader {
        id: buzz

        active: rows.count > 0
        source: "Buzz.qml"
    }

    Component {
        id: letterKeyComponent

        WordleKey {
            id: letterKey

            property var letterState: wordle.knownLetterState(letter)
            property alias letter: label.text

            enabled: wordle.canInputLetter
            color: (letterState === Wordle.LetterStateNotPresent) ? Wordle.notPresentBackgroundColor :
                (letterState === Wordle.LetterStatePresent) ?  Wordle.presentBackgroundColor :
                (letterState === Wordle.LetterStatePresentHere) ?  Wordle.presentHereBackgroundColor :
                Wordle.defaultKeyBackgroundColor

            Label {
                id: label

                anchors.fill: parent
                text: letter
                color: letterState === Wordle.LetterStateUnknown ? Wordle.keyTextColor : Wordle.textColor
                font {
                    capitalization: Font.AllUppercase
                    bold: true
                }
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            function updateLetterState() {
                letterState = wordle.knownLetterState(letter)
            }

            Connections {
                target: letterKey.wordle
                ignoreUnknownSignals: true
                onGameStateChanged: updateLetterState()
                onInputSubmitted: {
                    if (word.indexOf(letter) >= 0) {
                        updateLetterState()
                    }
                }
            }
        }
    }

    Component {
        id: enterKeyComponent

        WordleFunctionalKey {
            iconSource: "images/key-enter.svg"
            enabled: wordle.canSubmitInput
        }
    }

    Component {
        id: backspaceKeyComponent

        WordleFunctionalKey {
            id: backspaceKey

            iconSource: "images/key-backspace.svg"
            enabled: wordle.canDeleteLastLetter

            onDownChanged: {
                if (down) {
                    repeatDelayTimer.restart()
                } else {
                    repeatDelayTimer.stop()
                    repeatTimer.stop()
                }
            }

            Timer {
                id: repeatDelayTimer

                interval: 250
                repeat: false
                onTriggered: repeatTimer.restart()
            }

            Timer {
                id: repeatTimer

                interval: 100
                repeat: true
                onTriggered: backspaceKey.pressed()
            }
        }
    }
}
