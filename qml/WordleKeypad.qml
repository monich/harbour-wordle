import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.wordle 1.0

import "harbour"

Column {
    id: thisItem

    property bool landscape
    property var wordle
    property alias keypad: rows.model
    property int letterHeight: landscape ? Theme.itemSizeExtraSmall : Theme.itemSizeSmall

    signal keyPressed(var letter)

    spacing: Theme.paddingMedium

    Repeater {
        id: rows

        delegate: Row {
            id: row

            // Functional key width is 2 * baseKeyWidth
            readonly property string buttons: modelData
            readonly property int functionalKeys: Wordle.functionalKeyCount(buttons)
            readonly property int baseKeyWidth: (thisItem.width - (buttons.length - 1) * spacing) / (buttons.length + functionalKeys)

            spacing: Theme.paddingMedium

            Repeater {
                model: row.buttons.length

                Loader {
                    id: key

                    readonly property string letter: row.buttons.charAt(index)
                    readonly property bool functionalKey: Wordle.isFunctionalKey(letter)

                    width: row.baseKeyWidth * (functionalKey ? 2 : 1)
                    height: thisItem.letterHeight

                    sourceComponent: !functionalKey ? letterKeyComponent :
                        letter === '\b' ? backspaceKeyComponent :
                        letter === '\n' ? enterKeyComponent :
                        letter === '\r' ? rightEnterKeyComponent : null

                    Binding {
                        target: key.item
                        when: !key.functionalKey
                        property: "letter"
                        value: key.letter
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

            wordle: thisItem.wordle
            enabled: wordle.canInputLetter
            opacity: wordle.gameState === WordleGame.GameInProgress ? 1.0 : 0.6
            border.width: (letterState === Wordle.LetterStateUnknown) ? 1 : 0
            color: Wordle.letterBackgroundColor(letterState)

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

            Component.onCompleted: opacityBehavior.enabled = true

            Behavior on opacity {
                id: opacityBehavior

                enabled: false
                FadeAnimation { }
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
            wordle: thisItem.wordle
        }
    }

    Component {
        id: rightEnterKeyComponent

        WordleFunctionalKey {
            iconSource: "images/key-enter-right.svg"
            enabled: wordle.canSubmitInput
            wordle: thisItem.wordle
        }
    }

    Component {
        id: backspaceKeyComponent

        WordleFunctionalKey {
            id: backspaceKey

            iconSource: "images/key-backspace.svg"
            enabled: wordle.canDeleteLastLetter
            wordle: thisItem.wordle

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
