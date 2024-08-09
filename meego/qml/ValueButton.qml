import QtQuick 1.1
import com.nokia.meego 1.0

MouseArea {
    property string label
    property alias value: valueLabel.text

    /* readonly */ property bool down: pressed && containsMouse

    implicitHeight: selectionDialogStyle.itemHeight

    SelectionDialogStyle {
        id: selectionDialogStyle
    }

    LabelStyle {
        id: labelStyle

        inverted: true
    }

    Rectangle {
        anchors.fill: parent
        color: selectionDialogStyle.itemSelectedBackgroundColor
        opacity: down ? 1 : 0
        visible: opacity > 0
        FadeAnimation on opacity {}
    }

    Row {
        id: content

        spacing: WordleTheme.paddingMedium
        anchors {
            left: parent.left
            leftMargin: WordleTheme.horizontalPageMargin
            verticalCenter: parent.verticalCenter
        }

        Label {
            platformStyle: labelStyle
            font.bold: true
            text: label + ":"
        }

        Label {
            id: valueLabel

            platformStyle: labelStyle
        }
    }
}
