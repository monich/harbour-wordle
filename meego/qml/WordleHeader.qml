import QtQuick 1.1
import com.nokia.meego 1.0
import harbour.wordle 1.0

import "Utils.js" as Utils

Item {
    width: content.width
    height: content.height

    property string title
    property string answer
    property bool showAnswer
    property int secondsPlayed
    property real titleCenterY: content.y + background.y + background.height/2

    signal clicked()

    property string _answer
    property bool _showAnswer
    property bool _completed

    onAnswerChanged: if (_completed) changeState()
    onShowAnswerChanged: if (_completed) changeState()
    Component.onCompleted: {
        _completed = true
        applyChanges()
    }

    function changeState() {
        if (enabled) {
            flipAnimation.start()
        }
        if (!flipAnimation.running) {
            applyChanges()
        }
    }

    function applyChanges() {
        _answer = answer
        _showAnswer = showAnswer
    }

    MouseArea {
        anchors.fill: parent
        enabled: parent.enabled
        onClicked: parent.clicked()
    }

    Column {
        id: content

        anchors.centerIn: parent
        spacing: WordleTheme.paddingSmall

        Rectangle {
            id: background

            height: text.height
            width: text.width + 2 * WordleTheme.paddingLarge
            radius: WordleTheme.paddingSmall
            color: _showAnswer ? Wordle.defaultKeyBackgroundColor : "transparent"

            Label {
                id: text

                anchors.horizontalCenter: parent.horizontalCenter
                font {
                    capitalization: Font.AllUppercase
                    pixelSize: WordleTheme.fontSizeExtraLarge
                    weight: Font.Black
                }
                color: _showAnswer ? Wordle.keyTextColor : WordleTheme.primaryColor
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
            font.pixelSize: WordleTheme.fontSizeSmall
            color: WordleTheme.secondaryColor
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
