import QtQuick 1.1
import harbour.wordle 1.0

WordleKey {
    id: thisItem

    property alias iconSource: icon.source

    opacity: enabled ? 1.0 : 0.6

    Image {
        id: icon

        // Assume square icon
        property int iconSize: Math.ceil(2 * Math.min(thisItem.width, thisItem.height) / 3)

        anchors.centerIn: parent
        sourceSize: Qt.size(iconSize, iconSize)
        fillMode: Image.PreserveAspectFit
        opacity: thisItem.enabled ? 1.0 : 0.4
    }
}
