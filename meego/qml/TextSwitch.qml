import QtQuick 1.1
import com.nokia.meego 1.0
import harbour.wordle 1.0

Item {
    id: thisItem

    implicitHeight: Math.max(checkBox.implicitHeight, label.implicitHeight)
    height: implicitHeight

    property alias checked: checkBox.checked
    property alias text: label.text

    signal clicked()

    ButtonStyle {
        id: buttonStyle

        inverted: true
    }

    CheckBox {
        id: checkBox

        width: parent.width
        platformStyle: CheckBoxStyle { inverted: true }
        onClicked: thisItem.clicked()
    }

    Label {
        id: label

        x: checkBox.height + WordleTheme.paddingMedium
        verticalAlignment: Text.AlignVCenter
        width: parent.width - x
        height: Math.max(checkBox.implicitHeight, implicitHeight)
        color: checkBox.pressed ? buttonStyle.pressedTextColor : buttonStyle.textColor;
    }
}
