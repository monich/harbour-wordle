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
            // Unload the back panel
            backPanel.source = ""
            _targetAngle = 0
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
            onFlip: {
                backPanel.source = (where === 1) ?
                    "WordleSettingsPanel.qml" :
                    "WordleStatisticsPanel.qml"
                backPanelRotation.angle = 0
                rotation.axis.x = isPortrait ? 0 : 1
                rotation.axis.y = isPortrait ? 1 : 0
                flipable.flipState = where
            }
        }

        back: WordleBackPanel {
            id: backPanel

            anchors.fill: parent
            landscape: thisPage.isLandscape
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
                    rotation.axis.y = isPortrait ? 1 : 0
                    // This fixes a weird problem - after flipping, rotating and flipping again
                    // the back panel gets rotated 180 degrees around z axis
                    backPanelRotation.angle = 180
                }
                _targetAngle = (flipable.flipState === 1) ? 360 : -360
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
                    angle: -180
                }
            }
        ]

        transitions: Transition {
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
