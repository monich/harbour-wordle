import QtQuick 1.1
import com.nokia.meego 1.0
import harbour.wordle 1.0

Item {
    id: thisItem

    property bool active
    property bool landscape
    property alias wordle: board.wordle
    property variant history

    signal flip(variant where)

    /* readonly */ property bool _gameWon: wordle.gameState === WordleGame.GameWon

    function _keyPressed(letter) {
        if (letter === "\b") {
            // Backspace
            wordle.deleteLastLetter()
        } else if (letter === "\n" || letter === "\r") {
            // Enter
            if (wordle.canSubmitInput) {
                if (wordle.submitInput()) {
                    if (_gameWon) {
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

    QueryDialog {
        id: newGameDialog

        message: qsTrId("wordle-menu-new_game") + "?"
        icon: "images/logo.svg"
        //: Generic answer
        //% "Yes"
        acceptButtonText: qsTrId("wordle-generic-yes")
        //: Generic answer
        //% "No"
        rejectButtonText: qsTrId("wordle-generic-no")
        onAccepted: wordle.newGame()
        Component.onCompleted: {
            // Suggest to start the new game when the app is starting
            if (wordle.gameState !== WordleGame.GameInProgress) {
                open()
            }
        }
    }

    Item {
        id: boardArea

        width: parent.width
        anchors {
            top: parent.top
            bottom: fullKeypad.top
        }

        WordleHeader {
            id: header

            y: WordleTheme.paddingLarge
            anchors.horizontalCenter: parent.horizontalCenter
            answer: showAnswer ? wordle.answer : ""
            showAnswer: wordle.gameState === WordleGame.GameLost
            enabled: !wordle.loading
            secondsPlayed: wordle.secondsPlayed
            title: "WORDLE"
            onClicked: {
                if (wordle.gameState !== WordleGame.GameInProgress) {
                    newGameDialog.open()
                }
            }
        }

        IconButton {
            y: header.y +  header.titleCenterY - height/2
            anchors {
                left: parent.left
                leftMargin: WordleTheme.paddingMedium
            }
            iconSource: "images/statistics.svg"
            opacity: (history && history.totalCount > 0) ? 1 : 0
            visible: opacity > 0
            onClicked: thisItem.flip(2)
            FadeAnimation on opacity {}
        }

        IconButton {
            y: header.y +  header.titleCenterY - height/2
            anchors {
                right: parent.right
                rightMargin: WordleTheme.paddingMedium
            }
            iconSource: "images/settings.svg"
            onClicked: thisItem.flip(1)
        }

        Item {
            x: WordleTheme.horizontalPageMargin
            width: parent.width - 2 * x
            anchors {
                top: header.bottom
                topMargin: WordleTheme.paddingMedium
                bottom: parent.bottom
                bottomMargin: WordleTheme.paddingLarge
            }

            WordleBoard {
                id: board

                cellSize: Math.min(Math.floor((parent.width - spacing * (Constants.WordLength - 1))/Constants.WordLength),
                    Math.floor((parent.height - spacing * (Constants.MaxAttempts - 1))/Constants.MaxAttempts))
                anchors.centerIn: parent
                enableLetterFlipAnimation: thisItem.active
            }

            MouseArea {
                enabled: thisItem.enabled && _gameWon
                anchors.fill: board
                onClicked: board.celebrate()
            }
        }
    }

    WordleKeypad {
        id: fullKeypad

        anchors {
            bottom: parent.bottom
            bottomMargin: WordleTheme.paddingLarge
        }
        x: WordleTheme.horizontalPageMargin
        width: parent.width - 2 * x
        visible: !thisItem.landscape
        landscape: false
        wordle: thisItem.wordle
        keypad: visible ? thisItem.wordle.keypad : []
        onKeyPressed: _keyPressed(letter)
    }

    WordleKeypad {
        anchors {
            bottom: parent.bottom
            bottomMargin: WordleTheme.paddingLarge
            left: parent.left
            leftMargin: WordleTheme.horizontalPageMargin
        }
        width: (parent.width - board.width)/2 - 2 * WordleTheme.horizontalPageMargin
        visible: thisItem.landscape
        landscape: true
        wordle: thisItem.wordle
        keypad: visible ? thisItem.wordle.keypad1 : []
        letterHeight: board.cellSize
        onKeyPressed: _keyPressed(letter)
    }

    WordleKeypad {
        anchors {
            bottom: parent.bottom
            bottomMargin: WordleTheme.paddingLarge
            right: parent.right
            rightMargin: WordleTheme.horizontalPageMargin
        }
        width: (parent.width - board.width)/2 - 2 * WordleTheme.horizontalPageMargin
        visible: thisItem.landscape
        landscape: true
        wordle: thisItem.wordle
        keypad: visible ? thisItem.wordle.keypad2 : []
        letterHeight: board.cellSize
        onKeyPressed: _keyPressed(letter)
    }

    states: [
        State {
            name: "portrait"
            when: !landscape
            AnchorChanges {
                target: boardArea
                anchors.bottom: fullKeypad.top
            }
        },
        State {
            name: "landscape"
            when: landscape
            AnchorChanges {
                target: boardArea
                anchors.bottom: thisItem.bottom
            }
        }
    ]
}
