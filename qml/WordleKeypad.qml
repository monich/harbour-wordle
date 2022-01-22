import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.wordle 1.0

Column {
    id: thisItem

    property bool landscape
    property var model

    signal keyPressed(var letter)

    spacing: Theme.paddingMedium

    Loader {
        id: buzz

        active: rows.count > 0
        source: "Buzz.qml"
    }

    Repeater {
        id: rows

        model: thisItem.model.keypad
        delegate: Row {
            id: row

            // Functional key width is 2 * baseKeyWidth + spacing
            readonly property string buttons: modelData
            readonly property int functionalKeys: Wordle.functionalKeyCount(buttons)
            readonly property int baseKeyWidth: (thisItem.width + spacing) / (buttons.length + functionalKeys) - spacing

            spacing: Theme.paddingMedium

            Repeater {
                model: row.buttons.length

                WordleKey {
                    id: key

                    functionalKey: Wordle.isFunctionalKey(letter)
                    width: row.baseKeyWidth + (functionalKey ? (row.baseKeyWidth + row.spacing) : 0)
                    height: landscape ? Theme.itemSizeExtraSmall : Theme.itemSizeSmall
                    letter: row.buttons.charAt(index)
                    letterState: thisItem.model.knownLetterState(letter)
                    enabled: letter === '\b' ? thisItem.model.canDeleteLastLetter :
                        letter === '\n' ?  thisItem.model.canSubmitInput :
                        thisItem.model.canInputLetter
                    onPressed: {
                        buzz.item.play()
                        thisItem.keyPressed(letter)
                    }
                    function updateLetterState() {
                        letterState = thisItem.model.knownLetterState(letter)
                    }
                    Connections {
                        target: thisItem.model
                        onGameStateChanged: key.updateLetterState()
                        onInputSubmitted: {
                            if (word.indexOf(key.letter) >= 0) {
                                key.updateLetterState()
                            }
                        }
                    }
                }
            }
        }
    }
}
