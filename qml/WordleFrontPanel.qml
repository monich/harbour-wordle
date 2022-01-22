import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.wordle 1.0

import "harbour"

Item {
    id: thisItem

    property bool enableLetterFlipAnimation
    property bool landscape
    property alias title: header.title
    property var wordle

    signal flip()

    WordleHeader {
        id: header

        y: Theme.paddingLarge
        anchors.horizontalCenter: parent.horizontalCenter
        answer: showAnswer ? wordle.answer : ""
        showAnswer: wordle.gameState === WordleGame.GameLost
        enabled: !wordle.loading
    }

    HarbourIconTextButton {
        anchors {
            right: parent.right
            rightMargin: Theme.paddingMedium
            verticalCenter: header.verticalCenter
        }
        iconSource: "images/settings.svg"
        onClicked: thisItem.flip()
    }

    Item {
        x: Theme.horizontalPageMargin
        width: parent.width - 2 * x
        anchors {
            top: header.bottom
            topMargin: Theme.paddingLarge
            bottom: keypad.top
            bottomMargin: Theme.paddingLarge
        }

        WordleBoard {
            id: board

            cellSize: Math.min(Math.floor((parent.width - spacing * (Wordle.WordLength - 1))/Wordle.WordLength),
                Math.floor((parent.height - spacing * (Wordle.MaxAttempts - 1))/Wordle.MaxAttempts))
            model: wordle
            anchors.centerIn: parent
            enableLetterFlipAnimation: thisItem.enableLetterFlipAnimation
        }

        MouseArea {
            enabled: thisItem.enabled && wordle.gameState === WordleGame.GameWon
            anchors.fill: board
            onClicked: board.celebrate()
        }
    }

    WordleKeypad {
        id: keypad

        anchors {
            bottom: parent.bottom
            bottomMargin: Theme.paddingLarge
        }
        x: Theme.horizontalPageMargin
        width: parent.width - 2 * x
        landscape: thisItem.landscape
        model: wordle
        onKeyPressed: {
            if (letter === "\b") {
                // Backspace
                wordle.deleteLastLetter()
            } else if (letter === "\n") {
                // Enter
                if (wordle.canSubmitInput) {
                    if (wordle.submitInput()) {
                        if (wordle.gameState === WordleGame.GameWon) {
                            board.celebrate()
                        }
                    } else {
                        board.shakeRow(wordle.fullRows)
                    }
                }
            } else {
                // A normal letter
                wordle.inputLetter(letter)
            }
        }
    }
}
