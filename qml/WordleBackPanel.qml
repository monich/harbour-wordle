import QtQuick 2.0
import Sailfish.Silica 1.0

import "Constants.js" as Constants

SilicaFlickable {
    id: thisItem

    property alias sourceComponent: loader.sourceComponent

    signal flip()

    Rectangle {
        id: panelBorder

        anchors {
            fill: parent
            margins: Theme.paddingMedium
        }
        color: Theme.rgba(Theme.highlightBackgroundColor, 0.1)
        border {
            color: Theme.rgba(Theme.highlightColor, 0.4)
            width: Constants.thinBorder
        }
        radius: Constants.cornerRadius

        Item {
            x: panelBorder.border.width
            y: x
            width: parent.width - 2 * x
            height: parent.height - 2 * y

            Loader {
                id: loader
                anchors.fill: parent
            }
        }
    }

    Connections {
        target: loader.item
        onFlip: thisItem.flip()
    }
}
