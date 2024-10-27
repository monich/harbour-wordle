import QtQuick 1.1
import harbour.wordle 1.0

Rectangle {
    id: thisItem

    property variant wordle

    /* readonly */ property bool down: mouseArea.pressed && mouseArea.containsMouse

    signal pressed()

    width: parent.width
    height: parent.height

    radius: Theme.paddingSmall
    color: Wordle.defaultKeyBackgroundColor
    border {
        width: 1
        color: Wordle.emptySlotBorderColor
    }

    scale: down ? 1.2 : 1

    NumberAnimation on scale { duration: 20 }

    Rectangle {
        anchors.fill: parent
        radius: thisItem.radius
        gradient: Gradient {
            GradientStop { position: 0.0; color: Qt.tint(thisItem.color, "#20FFFFFF") }
            GradientStop { position: 0.1; color: Qt.tint(thisItem.color, "#20AAAAAA") }
            GradientStop { position: 0.9; color: Qt.tint(thisItem.color, "#20666666") }
            GradientStop { position: 1.0; color: Qt.tint(thisItem.color, "#20000000") }
        }
    }

    MouseArea {
        id: mouseArea

        anchors {
            fill: parent
            margins: -Theme.paddingKeypad/2
        }
        enabled: parent.enabled
        onClicked: thisItem.pressed()
    }
}
