import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.wordle 1.0

import "harbour"

Rectangle {
    id: thisItem

    property var wordle

    readonly property bool down: mouseArea.pressed && mouseArea.containsMouse

    signal pressed()

    width: parent.width
    height: parent.height

    radius: Theme.paddingSmall
    color: Wordle.defaultKeyBackgroundColor
    border {
        width: 1
        color: Wordle.emptySlotBorderColor
    }

    layer.enabled: mouseArea.pressed
    layer.effect: HarbourPressEffect { source: thisItem }
    scale: down ? 1.2 : 1

    Behavior on scale { NumberAnimation { duration: 50 } }

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
