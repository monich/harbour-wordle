import QtQuick 1.1

MouseArea {
    property alias icon: image
    property bool down: pressed && containsMouse
    property bool highlighted: down
    property alias iconSource: image.source

    /* readonly */ property bool _showPress: highlighted || pressTimer.running

    onPressedChanged: {
        if (pressed) {
            pressTimer.start()
        }
    }

    onCanceled: pressTimer.stop()

    implicitWidth: Theme.iconSizeMedium
    implicitHeight: Theme.iconSizeMedium
    width: implicitWidth
    height: implicitHeight

    Image {
        id: image

        sourceSize: Qt.size(width, height)
        opacity: parent.enabled ? 1.0 : 0.4
        anchors.fill: parent
    }

    Timer {
        id: pressTimer

        interval: Theme.minimumPressHighlightTime
    }
}
