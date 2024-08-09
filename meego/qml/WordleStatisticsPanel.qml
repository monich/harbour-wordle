import QtQuick 1.1
import com.nokia.meego 1.0
import harbour.wordle 1.0

import "Utils.js" as Utils

Column {
    property variant history
    property bool landscape

    /* readonly */ property bool _haveHistory: history.totalCount > 0
    /* readonly */ property int _guessDistributionMax: maxValue(history.guessDistribution)

    Grid {
        id: grid

        /* readonly */ property int _cellWidth: Math.floor((parent.width + spacing)/ columns) - spacing

        columns: landscape ? 6 : 3
        spacing: WordleTheme.paddingMedium

        WordleStatisticsItem {
            width: grid._cellWidth
            value: Utils.formatPlayTime(history.totalSec)
            //: Statistics item label
            //% "Total time"
            description: qsTrId("wordle-statistics-total_time")
        }

        WordleStatisticsItem {
            width: grid._cellWidth
            value: Utils.formatPlayTime(history.minGameSec)
            //: Statistics item label
            //% "Best time"
            description: qsTrId("wordle-statistics-best_time")
        }

        WordleStatisticsItem {
            width: grid._cellWidth
            value: history.totalCount
            //: Statistics item label
            //% "Games played"
            description: qsTrId("wordle-statistics-games_played")
        }

        WordleStatisticsItem {
            width: grid._cellWidth
            value: _haveHistory ? Math.ceil(100. * history.winCount/history.totalCount) : 0
            //: Statistics item label
            //% "Win %"
            description: qsTrId("wordle-statistics-win_percentage")
        }

        WordleStatisticsItem {
            width: grid._cellWidth
            value: history.currentStreak
            //: Statistics item label
            //% "Current streak"
            description: qsTrId("wordle-statistics-current_streak")
        }

        WordleStatisticsItem {
            width: grid._cellWidth
            value: history.maxStreak
            //: Statistics item label
            //% "Longest streak"
            description: qsTrId("wordle-statistics-longest_streak")
        }
    }

    Item {
        width: 1
        height: WordleTheme.paddingLarge
    }

    Label {
        anchors.horizontalCenter: parent.horizontalCenter
        font {
            capitalization: Font.AllUppercase
            weight: Font.Black
        }
        color: WordleTheme.highlightColor
        //: Guess distribution historgram title
        //% "Guess distribution"
        text: qsTrId("wordle-history-guess_distribution")
    }

    // Guess distributions
    Column {
        id: distributionColumn

        width: parent.width
        spacing: WordleTheme.paddingSmall

        Repeater {
            model: history.guessDistribution.length
            delegate: Row {
                id: distributionRow

                spacing: WordleTheme.paddingMedium

                Label {
                    id: rowIndexLabel

                    horizontalAlignment: Text.AlignRight
                    color: WordleTheme.primaryColor
                    anchors.verticalCenter: parent.verticalCenter
                    font.weight: Font.Black
                    text: model.index + 1
                    width: Math.ceil(Math.max(implicitWidth, WordleTheme.horizontalPageMargin))
                }

                Rectangle {
                    /* readonly */ property int value: history.guessDistribution[model.index]
                    /* readonly */ property int maxWidth: distributionColumn.width - rowIndexLabel.width - distributionRow.spacing
                    /* readonly */ property int minWidth: _guessDistributionMax ? Math.ceil(maxWidth * value/_guessDistributionMax) : 0

                    width: value ? Math.max(minWidth, historyValueLabel.implicitWidth + 2 * WordleTheme.paddingMedium) : WordleTheme.paddingSmall
                    height: rowIndexLabel.height
                    color: (history.lastAttempts === model.index + 1) ?
                        Wordle.presentHereBackgroundColor : Wordle.notPresentBackgroundColor

                    Label {
                        id: historyValueLabel

                        anchors {
                            right: parent.right
                            rightMargin: WordleTheme.paddingMedium
                            verticalCenter: parent.verticalCenter
                        }
                        horizontalAlignment: Text.AlignRight
                        font {
                            pixelSize: WordleTheme.fontSizeSmall
                            weight: Font.Black
                        }
                        text: parent.value
                        color: Wordle.textColor
                        visible: !!parent.value
                    }
                }
            }
        }
    }
}
