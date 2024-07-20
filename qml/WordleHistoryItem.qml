import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.wordle 1.0

import "Utils.js" as Utils
import "harbour"

BackgroundItem {
    id: thisItem

    property bool landscape
    property bool win
    property alias answer: boardModel.answer
    property alias attempts: boardModel.attempts
    property var startTime
    property var endTime
    property int secondsPlayed

    implicitHeight: Theme.itemSizeHuge + (landscape ? Theme.itemSizeMedium : Theme.itemSizeHuge)

    Column {
        id: answerColumn

        x: Theme.horizontalPageMargin
        y: Theme.paddingSmall

        Rectangle {
            id: answerLabelBackground

            height: answerLabel.height
            width: Math.min(Theme.itemSizeHuge, board.x - answerColumn.x - Theme.paddingLarge)
            radius: Theme.paddingSmall
            color: Wordle.defaultKeyBackgroundColor
            layer.enabled: thisItem.highlighted
            layer.effect: HarbourPressEffect { source: answerLabelBackground }

            Label {
                id: answerLabel

                width: parent.width
                leftPadding: Theme.paddingLarge
                rightPadding: Theme.paddingLarge
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                fontSizeMode: Text.Fit
                minimumPixelSize: Theme.fontSizeMedium
                font {
                    capitalization: Font.AllUppercase
                    pixelSize: Theme.fontSizeExtraLarge
                    family: Theme.fontFamilyHeading
                    weight: Font.Black
                }
                color: Wordle.keyTextColor
                text: answer
            }
        }

        Image {
            id: resultIcon

            anchors.left: parent.left
            sourceSize.height: landscape ? Theme.itemSizeExtraSmall : Theme.itemSizeSmall
            source: Qt.resolvedUrl(win ? "images/win.svg" : "images/loss.svg")
            layer.enabled: thisItem.highlighted
            layer.effect: HarbourPressEffect { source: resultIcon }
        }
    }

    Column {
        id: times

        anchors {
            top: landscape ? board.top : answerColumn.bottom
            left: landscape ? answerColumn.right : answerColumn.left
            right: board.left
            rightMargin: Theme.paddingLarge
        }

        Label {
            width: parent.width
            horizontalAlignment: landscape ? Text.AlignRight : Text.AlignLeft
            text: Utils.formatDateTime(startTime)
            font.pixelSize: Theme.fontSizeExtraSmall
            color: Theme.secondaryHighlightColor
        }

        Label {
            width: parent.width
            horizontalAlignment: landscape ? Text.AlignRight : Text.AlignLeft
            text: Utils.formatDateTime(endTime)
            font.pixelSize: Theme.fontSizeExtraSmall
            color: Theme.secondaryHighlightColor
        }

        Label {
            width: parent.width
            horizontalAlignment: landscape ? Text.AlignRight : Text.AlignLeft
            text: Utils.formatPlayTime(secondsPlayed)
            font.pixelSize: Theme.fontSizeSmall
            color: Theme.secondaryColor
        }
    }

    // Non-interactive board
    Grid {
        id: board

        spacing: Theme.paddingSmall
        columns: Wordle.WordLength
        anchors {
            top: answerColumn.top
            right: parent.right
            rightMargin: Theme.horizontalPageMargin
        }

        readonly property int cellSize: Math.floor((thisItem.height - 2 * y + spacing) / Wordle.MaxAttempts - spacing)

        Repeater {
            id: repeater

            model: WordleBoardModel {
                id: boardModel
            }

            delegate: Rectangle {
                id: letterBackground

                width: board.cellSize
                height: board.cellSize
                color: Utils.letterBackgroundColor(model.state)
                layer.enabled: thisItem.highlighted
                layer.effect: HarbourPressEffect { source: letterBackground }
                border {
                    width: (model.state === Wordle.LetterStateUnknown) ? Math.max(Math.floor(board.cellSize / 30), 1) : 0
                    color: Wordle.emptySlotBorderColor
                }

                Label {
                    text: model.letter
                    visible: text != ""
                    anchors.fill: parent
                    color: Wordle.textColor
                    font {
                        pixelSize: height/2
                        capitalization: Font.AllUppercase
                        bold: true
                    }
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }
    }
}
