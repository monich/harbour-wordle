import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.wordle 1.0

import "Utils.js" as Utils
import "harbour"

Column {
    id: thisItem

    readonly property alias headerHistoryTitle: historyTitle
    property var history
    property bool landscape

    signal highlight(var index)

    readonly property real _guessDistributionMax: maxCount(history.guessDistribution)
    readonly property bool _haveHistory: history.totalCount > 0
    property real _lastHeight: height

    // For whatever reason the initial height of the header after orientation change
    // is smaller and then grows (probably because the list is repositioned first and
    // then the grid gets resized). The intention here is to show the entire header
    // if the list was positioned at top before the screen got rotated.
    onHeightChanged: {
        if (historyList.contentY === -_lastHeight) {
            historyList.contentY = -height
        }
        _lastHeight = height
    }

    function maxCount(h) {
        var max = 0
        for (var i = 0; i < h.length; i++) {
            max = Math.max(max, h[i].count)
        }
        return max
    }

    Grid {
        id: statisticsGrid

        readonly property int _cellWidth: Math.floor((parent.width + spacing)/ columns) - spacing

        columns: landscape ? 6 : 3
        spacing: Theme.paddingMedium

        WordleStatisticsItem {
            width: statisticsGrid._cellWidth
            value: Utils.formatPlayTime(history.totalSec)
            //: Statistics item label
            //% "Total time"
            description: qsTrId("wordle-statistics-total_time")
        }

        WordleStatisticsItem {
            width: statisticsGrid._cellWidth
            value: Utils.formatPlayTime(history.shortestGameSec)
            //: Statistics item label
            //% "Best time"
            description: qsTrId("wordle-statistics-best_time")
            enabled: _haveHistory
            onClicked: highlight(history.shortestGameIndex)
        }

        WordleStatisticsItem {
            width: statisticsGrid._cellWidth
            value: history.totalCount
            //: Statistics item label
            //% "Games played"
            description: qsTrId("wordle-statistics-games_played")
        }

        WordleStatisticsItem {
            width: statisticsGrid._cellWidth
            value: _haveHistory ? Math.ceil(100. * history.winCount/history.totalCount) : 0
            //: Statistics item label
            //% "Win %"
            description: qsTrId("wordle-statistics-win_percentage")
        }

        WordleStatisticsItem {
            width: statisticsGrid._cellWidth
            value: history.currentStreak
            //: Statistics item label
            //% "Current streak"
            description: qsTrId("wordle-statistics-current_streak")
        }

        WordleStatisticsItem {
            width: statisticsGrid._cellWidth
            value: history.maxStreak
            //: Statistics item label
            //% "Longest streak"
            description: qsTrId("wordle-statistics-longest_streak")
            enabled: _haveHistory
            onClicked: highlight(history.maxStreakIndex)
        }
    }

    Item {
        width: 1
        height: Theme.paddingLarge
    }

    Label {
        anchors.horizontalCenter: parent.horizontalCenter
        font {
            capitalization: Font.AllUppercase
            weight: Font.Black
        }
        color: Theme.highlightColor
        //: Guess distribution historgram title
        //% "Guess distribution"
        text: qsTrId("wordle-history-guess_distribution")
    }

    // Guess distributions
    MouseArea {
        id: distributionItem

        property bool percentMode

        width: parent.width
        height: distributionColumn.height
        onClicked: percentMode = !percentMode

        Column {
            id: distributionColumn

            width: parent.width
            spacing: Theme.paddingSmall

            Repeater {
                model: history.guessDistribution.length
                delegate: Row {
                    id: distributionRow

                    Label {
                        id: rowIndexLabel

                        horizontalAlignment: Text.AlignRight
                        rightPadding: Theme.paddingMedium
                        anchors.verticalCenter: parent.verticalCenter
                        font.weight: Font.Black
                        text: model.index + 1
                        width: Math.ceil(Math.max(contentWidth, Theme.horizontalPageMargin))
                    }

                    Rectangle {
                        id: histogramBar

                        readonly property var value: history.guessDistribution[model.index]
                        readonly property int maxWidth: distributionColumn.width - rowIndexLabel.width
                        readonly property int minWidth: _guessDistributionMax ? Math.ceil(maxWidth * value.count/_guessDistributionMax) : 0

                        width: historyValue.visible ? Math.max(minWidth, historyValue.implicitWidth) : Theme.paddingSmall
                        height: rowIndexLabel.height
                        color: (history.lastAttempts === model.index + 1) ?
                            Wordle.presentHereBackgroundColor : Wordle.notPresentBackgroundColor
                        layer {
                            enabled: distributionItem.pressed
                            effect: HarbourPressEffect {
                                color: Theme.rgba(Theme.highlightBackgroundColor, 0.1)
                                source: histogramBar
                            }
                        }

                        WordleSuffixLabel {
                            id: historyValue

                            anchors {
                                right: parent.right
                                verticalCenter: parent.verticalCenter
                            }
                            layer {
                                enabled: distributionItem.pressed
                                effect: HarbourPressEffect {
                                    source: historyValue
                                }
                            }
                            value: distributionItem.percentMode ? parent.value.percent : parent.value.count
                            suffix: distributionItem.percentMode ? "%" : ""
                            color: Wordle.textColor
                            visible: !!parent.value.count
                        }
                    }
                }
            }
        }
    }

    Item {
        width: 1
        height: Theme.paddingLarge
    }

    Label {
        id: historyTitle

        // Font, color and opacity are controlled by WordleHistoryPanel
        anchors.right: parent.right
        //: Page header
        //% "History"
        text: qsTrId("wordle-history-header")
    }

    Item {
        width: 1
        height: Theme.paddingMedium
    }
}
