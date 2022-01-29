import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.wordle 1.0

import "harbour"

Rectangle {
    id: thisItem

    property bool functionalKey
    property var letterState
    property string letter

    signal pressed()

    radius: Theme.paddingSmall
    color: (letterState === Wordle.LetterStateNotPresent) ? Wordle.notPresentBackgroundColor :
        (letterState === Wordle.LetterStatePresent) ?  Wordle.presentBackgroundColor :
        (letterState === Wordle.LetterStatePresentHere) ?  Wordle.presentHereBackgroundColor :
        Wordle.defaultKeyBackgroundColor

    layer.enabled: mouseArea.pressed
    layer.effect: HarbourPressEffect { source: thisItem }
    scale: mouseArea.down ? 1.2 : 1

    Behavior on scale { NumberAnimation { duration: 50 } }

    Label {
        visible: !functionalKey
        anchors.fill: parent
        text: functionalKey ? "" : letter
        color: letterState === Wordle.LetterStateUnknown ? Wordle.keyTextColor : Wordle.textColor
        font {
            capitalization: Font.AllUppercase
            bold: true
        }
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    Loader {
        active: functionalKey
        anchors.centerIn: parent
        sourceComponent: Component {
            HarbourHighlightIcon {
                sourceSize.height: thisItem.height/2
                highlightColor: Wordle.keyTextColor
                source: letter === "\b" ? "images/key-backspace.svg" :
                    letter === "\n" ?  "images/key-enter.svg" : ""
                opacity: thisItem.enabled ? 1.0 : 0.4
            }
        }
    }

    MouseArea {
        id: mouseArea

        anchors.fill: parent

        readonly property bool down: pressed && containsMouse
        readonly property bool repeat: letter === "\b" && down

        onClicked: thisItem.pressed()

        onRepeatChanged: {
            if (repeat) {
                repeatDelayTimer.restart()
            } else {
                repeatDelayTimer.stop()
                repeatTimer.stop()
            }
        }

        Timer {
            id: repeatDelayTimer

            interval: 250
            repeat: false
            onTriggered: repeatTimer.restart()
        }

        Timer {
            id: repeatTimer

            interval: 100
            repeat: true
            onTriggered: thisItem.pressed()
        }
    }
}
