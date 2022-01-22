import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.wordle 1.0

Page {
    id: thisPage

    property alias title: frontPanel.title
    property alias wordle: frontPanel.wordle

    property bool _completed

    Component.onCompleted: _completed = true

    SilicaFlickable {
        anchors.fill: parent
        interactive: menu.opacity > 0

        PullDownMenu {
            id: menu

            visible: opacity > 0
            opacity: 0

            readonly property bool menuShouldBeVisible: (wordle.gameState === WordleGame.GameInProgress || flipable.flipped) ? 0 : 1

            Component.onCompleted: updateMenuVisibility()
            onMenuShouldBeVisibleChanged: updateMenuVisibility()
            onActiveChanged: updateMenuVisibility()

            Behavior on opacity { FadeAnimation { duration: 500 } }

            // Hide and show the menu when it's not active
            function updateMenuVisibility() {
                if (!active) {
                    opacity = menuShouldBeVisible ? 1 : 0
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

        Flipable {
            id: flipable

            anchors.fill: parent

            property bool flipped

            front: WordleFrontPanel {
                id: frontPanel

                anchors.fill: parent
                landscape: thisPage.isLandscape
                enableLetterFlipAnimation: enabled
                enabled: thisPage._completed && rotation.angle < 90
                onFlip: flipable.flipped = true
            }

            back: WordleSettingsPanel {
                anchors.fill: parent
                landscape: thisPage.isLandscape
                onFlip: flipable.flipped = false
            }

            transform: Rotation {
                id: rotation

                origin.x: flipable.width/2
                origin.y: flipable.height/2
                axis {
                    x: 0
                    y: 1
                    z: 0
                }
            }

            states: State {
                name: "back"
                when: flipable.flipped

                PropertyChanges {
                    target: rotation
                    angle: 180
                }
            }

            transitions: Transition {
                SequentialAnimation {
                    NumberAnimation {
                        id: flipAnimation

                        target: rotation
                        property: "angle"
                        duration: 500
                    }
                }
            }
        }
    }
}
