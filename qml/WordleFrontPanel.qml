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
    property var history

    signal flip(var where)

    // Still use portrait layout for really wide displays (e.g. Jolla tablet 3:4)
    readonly property bool _landscapeLayout: landscape && (Screen.width / Screen.height) < 0.75
    readonly property bool _gameWon: wordle.gameState === WordleGame.GameWon
    readonly property int _topNotch: ('topCutout' in Screen) ? Screen.topCutout.height : 0

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

            readonly property int _padding: Theme.paddingLarge
            readonly property int _notch: landscape ? 0 : _topNotch
            readonly property int _buttonCenter: y + titleCenterY - (y - _padding)

            y: Math.max(_padding, _notch)
            anchors.horizontalCenter: parent.horizontalCenter
            answer: showAnswer ? wordle.answer : ""
            showAnswer: wordle.gameState === WordleGame.GameLost
            enabled: !wordle.loading
            secondsPlayed: wordle.secondsPlayed
        }

        HarbourIconTextButton {
            y: header._buttonCenter - height/2
            anchors {
                left: parent.left
                leftMargin: Theme.paddingMedium
            }
            iconSource: "images/statistics.svg"
            opacity: (history && history.totalCount > 0) ? 1 : 0
            visible: opacity > 0
            onClicked: thisItem.flip(2)
            FadeAnimation on opacity {}
        }

        HarbourIconTextButton {
            y: header._buttonCenter - height/2
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
                enabled: thisItem.enabled && _gameWon
                anchors.fill: board
                onClicked: board.celebrate()
            }
        }
    }

    OpacityRampEffect {
        sourceItem: _landscapeLayout ? null : flickable
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
        visible: !_landscapeLayout
        landscape: false
        wordle: thisItem.wordle
        keypad: visible ? thisItem.wordle.keypad : []
        onKeyPressed: _keyPressed(letter)
    }

    WordleKeypad {
        anchors {
            bottom: parent.bottom
            bottomMargin: Theme.paddingLarge
            left: parent.left
            leftMargin: Theme.horizontalPageMargin
        }
        width: (parent.width - board.width)/2 - 2 * Theme.horizontalPageMargin
        visible: _landscapeLayout
        landscape: true
        wordle: thisItem.wordle
        keypad: visible ? thisItem.wordle.keypad1 : []
        letterHeight: board.cellSize
        onKeyPressed: _keyPressed(letter)
    }

    WordleKeypad {
        anchors {
            bottom: parent.bottom
            bottomMargin: Theme.paddingLarge
            right: parent.right
            rightMargin: Theme.horizontalPageMargin
        }
        width: (parent.width - board.width)/2 - 2 * Theme.horizontalPageMargin
        visible: _landscapeLayout
        landscape: true
        wordle: thisItem.wordle
        keypad: visible ? thisItem.wordle.keypad2 : []
        letterHeight: board.cellSize
        onKeyPressed: _keyPressed(letter)
    }

    states: [
        State {
            name: "portrait"
            when: !_landscapeLayout
            AnchorChanges {
                target: flickable
                anchors.bottom: keypad.top
            }
        },
        State {
            name: "landscape"
            when: _landscapeLayout
            AnchorChanges {
                target: flickable
                anchors.bottom: thisItem.bottom
            }
        }
    ]

    Loader {
        active: _gameWon
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
