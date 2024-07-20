import QtQuick 2.0
import Sailfish.Silica 1.0

Column {
    property alias value: valueLabel.text
    property alias description: descriptionLabel.text

    Label {
        id: valueLabel

        anchors.horizontalCenter: parent.horizontalCenter
        font {
            pixelSize: Theme.fontSizeLarge
            weight: Font.Black
        }
    }

    Label {
        id: descriptionLabel

        width: parent.width
        font.pixelSize: Theme.fontSizeExtraSmall
        color: Theme.highlightColor
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.WordWrap
    }
}
