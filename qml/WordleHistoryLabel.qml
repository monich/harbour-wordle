import QtQuick 2.0
import Sailfish.Silica 1.0

Item {
    id: thisItem

    property color color
    property string icon
    property real spacing: Theme.paddingSmall
    property alias text: label.text

    implicitWidth: image.width + spacing + label.implicitWidth
    implicitHeight: Math.max(image.height, label.implicitHeight)

    Image {
        id: image

        anchors.verticalCenter: parent.verticalCenter
        source: icon + "?" + color
    }

    Label {
        id: label

        anchors {
            left: image.right
            leftMargin: spacing
            verticalCenter: parent.verticalCenter
        }
        truncationMode: TruncationMode.Fade
        font.pixelSize: Theme.fontSizeSmall
        color: thisItem.color
    }
}
