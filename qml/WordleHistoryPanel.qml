import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.wordle 1.0

import "Utils.js" as Utils

Item {
    id: thisItem

    property alias history: historyList.model
    property alias flickable: menu.flickable
    property bool landscape
    property bool active

    readonly property real _guessDistributionMax: maxValue(history.guessDistribution)
    readonly property bool _haveHistory: history.totalCount > 0
    readonly property bool _remorsePopupVisible: _remorsePopup ? _remorsePopup.visible : false
    property int _highlightIndex: -1
    property var _remorsePopup

    signal flip()

    anchors.fill: parent

    function maxValue(v) {
        var max = 0
        for (var i = 0; i < v.length; i++) {
            max = Math.max(max, v[i])
        }
        return max
    }

    function scrollAndHighlight(index) {
        historyList.positionViewAtIndex(index, ListView.Center)
        resetHighlightTimer.startHighlight(index)
    }

    Timer {
        id: resetHighlightTimer
                                             //  on   off  on   off  on
        readonly property var highlightPattern: [350, 250, 250, 250, 250]
        property int highlightStep
        property int highlightItem: -1

        function startHighlight(index) {
            stop()
            _highlightIndex = highlightItem = index;
            interval = highlightPattern[highlightStep = 0]
            start()
        }

        onTriggered: {
            highlightStep++
            if (highlightStep < highlightPattern.length) {
                _highlightIndex = (_highlightIndex < 0) ? highlightItem : -1
                interval = highlightPattern[highlightStep]
                start()
            } else {
                _highlightIndex = -1
            }
        }
    }

    PullDownMenu {
        id: menu

        property var _pendingAction

        opacity: (thisItem.active && !_remorsePopupVisible) ? 1 : 0
        visible: opacity > 0

        FadeAnimation on opacity {}

        onActiveChanged: {
            if (!active && _pendingAction) {
                var action = _pendingAction
                _pendingAction = null
                action()
            }
        }

        MenuItem {
            //: Pulley menu item
            //% "Clear history"
            text: qsTrId("wordle-history-menu-clear")

            onClicked: menu._pendingAction = action

            function action() {
                if (!_remorsePopup) {
                    _remorsePopup = remorsePopupComponent.createObject(flickable)
                }
                //: Remorse popup description
                //% "All history and statistics will be deleted"
                _remorsePopup.execute(qsTrId("wordle-history-clear-remorse"),
                function() {
                    history.clear()
                    thisItem.flip()
                })
            }
        }
    }

    Component {
        id: remorsePopupComponent

        RemorsePopup { }
    }

    Item {
        width: statisticsHeaderLabel.width
        height: statisticsHeaderLabel.height + statisticsHeaderLabel.y
        anchors {
            right: parent.right
            rightMargin: Theme.horizontalPageMargin
        }

        Label {
            id: statisticsHeaderLabel

            readonly property real _angle: (historyHeaderLabel.y > y + height) ? 0 : (90 * (height - historyHeaderLabel.y + y) / height)

            y:  Theme.paddingLarge
            color: Theme.highlightColor
            font {
                capitalization: Font.AllUppercase
                pixelSize: Theme.fontSizeLarge
                family: Theme.fontFamilyHeading
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

        y: Theme.paddingLarge
        height: statisticsHeaderLabel.height

        IconButton {
            anchors {
                left: parent.left
                leftMargin: Theme.paddingMedium
                verticalCenter: parent.verticalCenter
            }
            icon.source: "image://theme/icon-m-acknowledge"
            onClicked: thisItem.flip()
        }
    }

    // The special label which follows the position of the HISTORY label
    // until it pushes up and replaces the STATISTICS label at the top
    // of the panel. Then it stays there even if the history list gets
    // scrolled further down.
    Label {
        id: historyHeaderLabel

        // Additional parameters are forcing re-evaluation
        function calcPos(item,p1,p2,p3,p4,p5,p6,p7,p8) {
            return parent.mapFromItem(item, 0, 0)
        }

        readonly property point pos: calcPos(historyList.historyTitle,
            historyList.historyTitle ? historyList.historyTitle.x : 0,
            historyList.historyTitle ? historyList.historyTitle.y : 0,
            historyList.x, historyList.y,
            historyList.originX, historyList.originY,
            historyList.contentX, historyList.contentY)

        x: pos.x
        y: Math.max(pos.y, statisticsHeaderLabel.y)
        width: historyList.historyTitle ? historyList.historyTitle.width : 0
        height: historyList.historyTitle ? historyList.historyTitle.height : 0
        color: Theme.highlightColor
        opacity: (y < historyList.y) ? 1 : 0
        visible: opacity > 0
        font {
            capitalization: Font.AllUppercase
            pixelSize: Theme.fontSizeLarge
            family: Theme.fontFamilyHeading
            weight: Font.Black
        }
        //: Page header
        //% "History"
        text: qsTrId("wordle-history-header")
    }

    SilicaListView {
        id: historyList

        readonly property Item historyTitle: headerItem ?  headerItem.headerHistoryTitle : null

        clip: true
        width: parent.width
        anchors {
            top: header.bottom
            topMargin: Theme.paddingMedium
            bottom: parent.bottom
            bottomMargin: Theme.paddingMedium
        }

        header: Component {
            Column {
                readonly property alias headerHistoryTitle: historyTitle
                property real lastHeight: height

                x: Theme.horizontalPageMargin
                width: parent.width - 2 * x
                visible: _haveHistory

                // For whatever reason the initial height of the header after orientation change
                // is smaller and then grows (probably because the list is repositioned first and
                // then the grid gets resized). The intention here is to show the entire header
                // if the list was positioned at top before the screen got rotated.
                onHeightChanged: {
                    if (historyList.contentY === -lastHeight) {
                        historyList.contentY = -height
                    }
                    lastHeight = height
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
                        onClicked: scrollAndHighlight(history.shortestGameIndex)
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
                        onClicked: scrollAndHighlight(history.maxStreakIndex)
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
                                readonly property int value: history.guessDistribution[model.index]
                                readonly property int maxWidth: distributionColumn.width - rowIndexLabel.width
                                readonly property int minWidth: _guessDistributionMax ? Math.ceil(maxWidth * value/_guessDistributionMax) : 0

                                width: historyValueLabel.visible ? Math.max(minWidth, historyValueLabel.implicitWidth) : Theme.paddingSmall
                                height: rowIndexLabel.height
                                color: (history.lastAttempts === model.index + 1) ?
                                    Wordle.presentHereBackgroundColor : Wordle.notPresentBackgroundColor

                                Label {
                                    id: historyValueLabel

                                    anchors {
                                        left: parent.left
                                        right: parent.right
                                        verticalCenter: parent.verticalCenter
                                    }
                                    leftPadding: Theme.paddingMedium
                                    rightPadding: Theme.paddingMedium
                                    horizontalAlignment: Text.AlignRight
                                    font {
                                        pixelSize: Theme.fontSizeSmall
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

                Item {
                    width: 1
                    height: Theme.paddingLarge
                }

                Label {
                    id: historyTitle

                    anchors.right: parent.right
                    color: historyHeaderLabel.color
                    opacity: 1 - historyHeaderLabel.opacity
                    font: historyHeaderLabel.font
                    //: Page header
                    //% "History"
                    text: qsTrId("wordle-history-header")
                }

                Item {
                    width: 1
                    height: Theme.paddingMedium
                }
            }
        }

        delegate: WordleHistoryItem {
            landscape: thisItem.landscape
            win: model.win
            answer: model.answer
            streak: model.streak
            attempts: model.attempts
            endTime: model.endTime
            secondsPlayed: model.secondsPlayed
            forceHighlight: model.index === _highlightIndex
        }

        VerticalScrollDecorator { }
    }
}
