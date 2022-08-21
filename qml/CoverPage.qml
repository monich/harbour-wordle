import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.wordle 1.0

import "Utils.js" as Utils

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
            bottom: column.top
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

    Column {
        id: column

        x: Theme.paddingLarge
        width: parent.width - 2 * x
        spacing: Theme.paddingMedium
        anchors {
            bottom: parent.bottom
            bottomMargin: WordleSettings.showPlayTime ? Theme.paddingMedium : Theme.paddingLarge
        }

        WordleBoard {
            id: board

            width: parent.width
            cellSize: Math.floor((width - spacing * (Wordle.WordLength - 1))/Wordle.WordLength)
            model: game
            spacing: Theme.paddingSmall
        }

        Label {
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: Theme.fontSizeSmall
            color: Theme.secondaryColor
            text: visible ? Utils.formatPlayTime(wordle.secondsPlayed) : ""
            visible: WordleSettings.showPlayTime
        }
    }
}
