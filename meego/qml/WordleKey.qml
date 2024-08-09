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

    MouseArea {
        id: mouseArea

        anchors {
            fill: parent
            margins: -Theme.paddingMedium
        }
        enabled: parent.enabled
        onClicked: thisItem.pressed()
    }
}
