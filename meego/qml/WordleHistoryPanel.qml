import QtQuick 1.1
import com.nokia.meego 1.0
import harbour.wordle 1.0

Item {
    id: thisItem

    property variant history
    property bool landscape
    property bool active

    /* readonly */ property bool _haveHistory: history.totalCount > 0

    signal flip()

    anchors.fill: parent

    function maxValue(v) {
        var max = 0
        for (var i = 0; i < v.length; i++) {
            max = Math.max(max, v[i])
        }
        return max
    }

    function clearHistory() {
        history.clear()
        thisItem.flip()
    }

    Item {
        width: statisticsHeaderLabel.width
        height: statisticsHeaderLabel.height + statisticsHeaderLabel.y
        anchors {
            right: parent.right
            rightMargin: WordleTheme.horizontalPageMargin
        }

        Label {
            id: statisticsHeaderLabel

            property real _angle: 0

            y:  WordleTheme.paddingLarge
            color: WordleTheme.highlightColor
            font {
                capitalization: Font.AllUppercase
                pixelSize: WordleTheme.fontSizeLarge
                weight: Font.Black
            }
            //: Page header
            //% "Statistics"
            text: qsTrId("wordle-statistics-header")
            transform: Rotation {
                angle: statisticsHeaderLabel._angle
                axis {
                    x: 1
                    y: 0
                    z: 0
                }
            }
        }
    }

    Item {
        id: header

        y: WordleTheme.paddingLarge
        height: statisticsHeaderLabel.height

        IconButton {
            anchors {
                left: parent.left
                leftMargin: WordleTheme.paddingMedium
                verticalCenter: parent.verticalCenter
            }
            iconSource: "images/ok.svg"
            onClicked: thisItem.flip()
        }
    }

    QueryDialog {
        id: clearHistoryDialog

        message: qsTrId("wordle-history-menu-clear") + "?"
        icon: "images/logo.svg"
        acceptButtonText: qsTrId("wordle-generic-yes")
        rejectButtonText: qsTrId("wordle-generic-no")
        onAccepted: {
            history.clear()
            thisItem.flip()
        }
    }

    // The history list seems to be too heavy for N9, let's ignore it for now
    // Just show the overall statistics

    Flickable {
        id: statisticsFlickable

        clip: true
        visible: _haveHistory
        width: parent.width
        contentHeight: statisticsColumn.height
        interactive: contentHeight > height

        anchors {
            top: header.bottom
            topMargin: WordleTheme.paddingMedium
            bottom: parent.bottom
            bottomMargin: WordleTheme.paddingMedium
        }

        Column {
            id: statisticsColumn

            width: parent.width
            spacing: Math.max(WordleTheme.paddingLarge, statisticsFlickable.height - statisticsPanel.height - clearHistoryButton.height)

            WordleStatisticsPanel {
                id: statisticsPanel

                x: WordleTheme.horizontalPageMargin
                width: parent.width - 2 * x
                history: thisItem.history
                landscape: thisItem.landscape
            }

            Button {
                id: clearHistoryButton

                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTrId("wordle-history-menu-clear")
                platformStyle: ButtonStyle {
                    pressedBackground: pressedDialog
                    background: dialog
                    inverted: true
                }
                onClicked: clearHistoryDialog.open()
            }
        }
    }
}
