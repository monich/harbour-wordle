import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.wordle 1.0

Item {
    id: thisItem

    property alias history: historyList.model
    property alias flickable: menu.flickable
    property bool landscape
    property bool active

    readonly property bool _haveHistory: history.totalCount > 0
    readonly property bool _remorsePopupVisible: _remorsePopup ? _remorsePopup.visible : false
    property int _highlightIndex: -1
    property var _remorsePopup

    signal flip()

    anchors.fill: parent

    function scrollAndHighlight(index) {
        historyList.positionViewAtIndex(index, ListView.Center)
        highlightTimer.startHighlight(index)
    }

    Timer {
        id: highlightTimer
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

        header: WordleStatisticsPanel {
            x: Theme.horizontalPageMargin
            width: parent.width - 2 * x
            history: thisItem.history
            landscape: thisItem.landscape
            headerHistoryTitle {
                color: historyHeaderLabel.color
                opacity: 1 - historyHeaderLabel.opacity
                font: historyHeaderLabel.font
            }
            onHighlight: scrollAndHighlight(index)
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
