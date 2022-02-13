import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.wordle 1.0

import "harbour"

WordleKey {
    id: thisItem

    property alias iconSource: icon.source

    opacity: enabled ? 1.0 : 0.6

    HarbourHighlightIcon {
        id: icon

        anchors.centerIn: parent
        sourceSize.height: thisItem.height/2
        highlightColor: Wordle.keyTextColor
        opacity: thisItem.enabled ? 1.0 : 0.4
    }
}
