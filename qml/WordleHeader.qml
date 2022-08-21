import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.wordle 1.0

import "Utils.js" as Utils

Item {
    height: content.height

    property string title
    property string answer
    property bool showAnswer
    property int secondsPlayed
    property real titleCenterY: content.y + background.y + background.height/2

    property string _answer
    property bool _showAnswer

    onAnswerChanged: changeState()
    onShowAnswerChanged: changeState()

    function changeState() {
        if (enabled) {
            flipAnimation.start()
        } else if (!flipAnimation.running) {
            applyChanges()
        }
    }

    function applyChanges() {
        _answer = answer
        _showAnswer = showAnswer
    }

    Column {
        id: content

        anchors.centerIn: parent

        Rectangle {
            id: background

            height: text.height
            width: text.width + 2 * Theme.paddingLarge
            radius: Theme.paddingSmall
            color: _showAnswer ? Wordle.defaultKeyBackgroundColor : "transparent"

            Label {
                id: text

                anchors.horizontalCenter: parent.horizontalCenter
                font {
                    capitalization: Font.AllUppercase
                    pixelSize: Theme.fontSizeExtraLarge
                    family: Theme.fontFamilyHeading
                    weight: Font.Black
                }
                color: _showAnswer ? Wordle.keyTextColor : Theme.primaryColor
                text: _showAnswer ? _answer : title
            }

            transform: Rotation {
                id: rotation

                origin {
                    x: background.width / 2
                    y: background.height / 2
                }

                axis {
                    x: 1
                    y: 0
                    z: 0
                }
            }
        }

        Label {
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: Theme.fontSizeSmall
            color: Theme.secondaryColor
            text: visible ? Utils.formatPlayTime(secondsPlayed) : ""
            visible: WordleSettings.showPlayTime
        }
    }

    SequentialAnimation {
        id: flipAnimation

        alwaysRunToEnd: true

        NumberAnimation {
            easing.type: Easing.InOutSine
            target: rotation
            property: "angle"
            from: 0
            to: 90
            duration: 250
        }
        ScriptAction { script: applyChanges() }
        NumberAnimation {
            easing.type: Easing.InOutSine
            target: rotation
            property: "angle"
            to: 0
            duration: 250
        }
        ScriptAction { script: applyChanges() }
    }
}
