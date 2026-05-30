import QtQuick 2.0
import Sailfish.Silica 1.0

import "Constants.js" as Constants

Row {
    id: thisItem

    property alias value: valueLabel.text
    property alias suffix: suffixLabel.text
    property color color: valueLabel.color
    property real padding: Theme.paddingMedium

    spacing: Constants.thinBorder

    Label {
        id: valueLabel

        anchors.verticalCenter: parent.verticalCenter
        leftPadding: thisItem.padding
        rightPadding: suffixLabel.visible ? 0 : thisItem.padding
        font {
            pixelSize: Theme.fontSizeSmall
            weight: Font.Black
        }
    }

    Label {
        id: suffixLabel

        anchors.baseline: valueLabel.baseline
        rightPadding: thisItem.padding
        font.pixelSize: valueLabel.font.pixelSize
        visible: !!text
    }
}
