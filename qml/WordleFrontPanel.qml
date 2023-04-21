import QtQuick 2.0
import QtSensors 5.0
import Sailfish.Silica 1.0
import harbour.wordle 1.0

import "harbour"

Item {
    id: thisItem

    property bool active
    property bool landscape
    property alias title: header.title
    property var wordle

    signal flip(var where)

    readonly property bool gameWon: wordle.gameState === WordleGame.GameWon

    SilicaFlickable {
        id: flickable

        interactive: menu.visible
        width: parent.width
        clip: true
        anchors {
            top: parent.top
            bottom: keypad.top
        }

        PullDownMenu {
            id: menu

            readonly property bool menuShouldBeVisible: (wordle.gameState === WordleGame.GameInProgress || !thisItem.active) ? 0 : 1

            Component.onCompleted: updateMenuVisibility()
            onMenuShouldBeVisibleChanged: updateMenuVisibility()
            onActiveChanged: updateMenuVisibility()

            // Hide and show the menu when it's not active
            function updateMenuVisibility() {
                if (!active) {
                    visible = menuShouldBeVisible
                    newGameMenuItem.enabled = menuShouldBeVisible
                }
            }

            MenuItem {
                id: newGameMenuItem

                //: Pulley menu item
                //% "New game"
                text: qsTrId("wordle-menu-new_game")
                onClicked: {
                    wordle.newGame()
                    enabled = false
                }
            }
        }

        WordleHeader {
            id: header

            y: Theme.paddingLarge
            anchors.horizontalCenter: parent.horizontalCenter
            answer: showAnswer ? wordle.answer : ""
            showAnswer: wordle.gameState === WordleGame.GameLost
            enabled: !wordle.loading
            secondsPlayed: wordle.secondsPlayed
        }

        HarbourIconTextButton {
            y: header.y +  header.titleCenterY - height/2
            anchors {
                right: parent.right
                rightMargin: Theme.paddingMedium
            }
            iconSource: "images/settings.svg"
            onClicked: thisItem.flip(1)
        }

        Item {
            x: Theme.horizontalPageMargin
            width: parent.width - 2 * x
            anchors {
                top: header.bottom
                topMargin: Theme.paddingMedium
                bottom: parent.bottom
                bottomMargin: Theme.paddingLarge
            }

            WordleBoard {
                id: board

                cellSize: Math.min(Math.floor((parent.width - spacing * (Wordle.WordLength - 1))/Wordle.WordLength),
                    Math.floor((parent.height - spacing * (Wordle.MaxAttempts - 1))/Wordle.MaxAttempts))
                model: wordle
                anchors.centerIn: parent
                enableLetterFlipAnimation: thisItem.active
            }

            MouseArea {
                enabled: thisItem.enabled && gameWon
                anchors.fill: board
                onClicked: board.celebrate()
            }
        }
    }

    OpacityRampEffect {
        sourceItem: flickable
        slope: flickable.height/Theme.paddingLarge
        offset: (flickable.height - Theme.paddingLarge)/flickable.height
        direction: OpacityRamp.TopToBottom
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
        wordle: thisItem.wordle
        onKeyPressed: {
            if (letter === "\b") {
                // Backspace
                wordle.deleteLastLetter()
            } else if (letter === "\n" || letter === "\r") {
                // Enter
                if (wordle.canSubmitInput) {
                    if (wordle.submitInput()) {
                        if (gameWon) {
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

    Loader {
        active: gameWon
        sourceComponent: Component {
            Accelerometer {
                active: thisItem.active

                readonly property real shakeThreshold: 500
                readonly property real accelerationSquared: reading ? (reading.x * reading.x + reading.y * reading.y + reading.z * reading.z) : 0

                onAccelerationSquaredChanged: {
                    if (accelerationSquared > shakeThreshold) {
                        board.celebrate()
                    }
                }
            }
        }
    }
}
