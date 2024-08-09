import QtQuick 1.1
import harbour.wordle 1.0

Item {
    id: thisPage

    property bool landscape
    property alias wordle: frontPanel.wordle

    property bool _completed
    property real _targetAngle

    Component.onCompleted: _completed = true

    function completeFlip() {
        if (!flipable.flipState) {
            _targetAngle = 0
        }
    }

    WordleHistory {
        id: wordleHistory

        language: WordleSettings.language
    }

    Connections {
        target: wordle
        onGameOver: wordleHistory.add(wordle.answer, wordle.attempts,
            wordle.startTime, wordle.finishTime, wordle.secondsPlayed)
    }

    Component {
        id: settingsComponent

        WordleSettingsPanel {
            landscape: thisPage.landscape
        }
    }

    Component {
        id: historyComponent

        WordleHistoryPanel {
            history: wordleHistory
            //flickable: backPanel
            landscape: thisPage.landscape
            active: flipable.side == Flipable.Back && flipable.flipState == 2 && !flipTransition.running
        }
    }

    Flipable {
        id: flipable

        anchors.fill: parent

        // 0: normal
        // 1: flipped right (settings)
        // 2: flipped left (statistics)
        property int flipState

        front: WordleFrontPanel {
            id: frontPanel

            anchors.fill: parent
            landscape: thisPage.landscape
            enabled: thisPage._completed && !rotation.angle
            active: enabled && Qt.application.active
            history: wordleHistory
            onFlip: {
                backPanel.sourceComponent = (where === 1) ? settingsComponent : historyComponent
                backPanelRotation.angle = 0
                rotation.axis.x = thisPage.landscape ? 1 : 0
                rotation.axis.y = thisPage.landscape ? 0 : 1
                flipable.flipState = where
            }
        }

        back: WordleBackPanel {
            id: backPanel

            anchors.fill: parent
            transform: Rotation {
                id: backPanelRotation

                origin {
                    x: backPanel.width/2
                    y: backPanel.height/2
                }
                axis {
                    x: 0
                    y: 0
                    z: 1
                }
            }
            onFlip: {
                var rotateX = thisPage.landscape ? 1 : 0
                if (rotation.axis.x !== rotateX) {
                    rotation.axis.x = rotateX
                    rotation.axis.y = rotateX ? 0 : 1
                    // This fixes a weird problem - after flipping, rotating and flipping again
                    // the back panel gets rotated 180 degrees around z axis
                    backPanelRotation.angle = 180
                }
                _targetAngle = (flipable.flipState === 1 || thisPage.landscape) ? 360 : -360
                flipable.flipState = 0
            }
        }

        transform: Rotation {
            id: rotation

            origin {
                x: flipable.width/2
                y: flipable.height/2
            }
            axis {
                x: 0
                y: 1
                z: 0
            }
        }

        states: [
            State {
                name: "normal"
                when: flipable.flipState === 0

                PropertyChanges {
                    target: rotation
                    angle: _targetAngle
                }
            },
            State {
                name: "settings"
                when: flipable.flipState === 1

                PropertyChanges {
                    target: rotation
                    angle: 180
                }
            },
            State {
                name: "statistics"
                when: flipable.flipState === 2

                PropertyChanges {
                    target: rotation
                    angle: thisPage.landscape ? 180 : -180
                }
            }
        ]

        transitions: Transition {
            id: flipTransition

            SequentialAnimation {
                alwaysRunToEnd: true
                NumberAnimation {
                    target: rotation
                    property: "angle"
                    duration: 500
                }
                ScriptAction { script: thisPage.completeFlip() }
            }
        }
    }
}
