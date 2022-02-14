import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.wordle 1.0

Page {
    id: thisPage

    property alias title: frontPanel.title
    property alias wordle: frontPanel.wordle

    property bool _completed

    Component.onCompleted: _completed = true

    Flipable {
        id: flipable

        anchors.fill: parent

        property bool flipped

        front: WordleFrontPanel {
            id: frontPanel

            anchors.fill: parent
            landscape: thisPage.isLandscape
            enabled: thisPage._completed && rotation.angle < 90
            active: enabled && Qt.application.active
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
