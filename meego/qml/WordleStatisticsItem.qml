import QtQuick 1.1
import com.nokia.meego 1.0

Column {
    property alias value: valueLabel.text
    property alias description: descriptionLabel.text

    Label {
        id: valueLabel

        anchors.horizontalCenter: parent.horizontalCenter
        color: WordleTheme.primaryColor
        font {
            pixelSize: WordleTheme.fontSizeLarge
            weight: Font.Black
        }
    }

    Label {
        id: descriptionLabel

        width: parent.width
        font.pixelSize: WordleTheme.fontSizeExtraSmall
        color: WordleTheme.highlightColor
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.WordWrap
    }
}
