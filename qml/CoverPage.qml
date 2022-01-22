import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.wordle 1.0

CoverBackground {
    id: cover

    property alias title: header.text
    property alias wordle: board.model

    Label {
        id: header

        x: 2 * Theme.paddingLarge
        width: parent.width - 2 * x
        anchors {
            top: parent.top
            topMargin: Theme.paddingMedium
            bottom: board.top
            bottomMargin: Theme.paddingMedium
        }
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        minimumPixelSize: Theme.fontSizeLarge
        fontSizeMode: Text.Fit
        font {
            pixelSize: Theme.fontSizeHuge
            bold: true
        }
    }

    WordleBoard {
        id: board

        x: Theme.paddingLarge
        width: parent.width - 2 * x
        anchors {
            bottom: parent.bottom
            bottomMargin: Theme.paddingLarge
        }
        cellSize: Math.floor((width - spacing * (Wordle.WordLength - 1))/Wordle.WordLength)
        model: game
        spacing: Theme.paddingSmall
    }
}
