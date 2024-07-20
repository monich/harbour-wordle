import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.wordle 1.0

Page {
    id: thisPage

    property alias title: frontPanel.title
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
            landscape: thisPage.isLandscape
        }
    }

    Component {
        id: historyComponent

        WordleHistoryPanel {
            history: wordleHistory
            flickable: backPanel
            landscape: thisPage.isLandscape
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
            landscape: thisPage.isLandscape
            enabled: thisPage._completed && !rotation.angle
            active: enabled && Qt.application.active
            history: wordleHistory
            onFlip: {
                backPanel.sourceComponent = (where === 1) ? settingsComponent : historyComponent
                backPanelRotation.angle = 0
                rotation.axis.x = isPortrait ? 0 : 1
                rotation.axis.y = isPortrait ? 1 : 0
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
                var rotateX = isPortrait ? 0 : 1
                if (rotation.axis.x !== rotateX) {
                    rotation.axis.x = rotateX
                    rotation.axis.y = rotateX ? 0 : 1
                    // This fixes a weird problem - after flipping, rotating and flipping again
                    // the back panel gets rotated 180 degrees around z axis
                    backPanelRotation.angle = 180
                }
                _targetAngle = (flipable.flipState === 1 || !isPortrait) ? 360 : -360
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
                    angle: isPortrait ? -180 : 180
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
