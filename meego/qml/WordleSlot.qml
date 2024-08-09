import QtQuick 1.1
import com.nokia.meego 1.0
import harbour.wordle 1.0

Rectangle {
    id: thisItem

    property string letter
    property int letterState
    property bool enableLetterFlipAnimation
    /* readonly */ property bool flipping: !!_flipAnimation && _flipAnimation.running

    property string _letter
    property int _letterState
    property variant _flipAnimation

    color: Wordle.letterBackgroundColor(_letterState)
    border {
        width: _letterState ? 0 : Math.max(Math.floor(thisItem.width/30),1)
        color: Wordle.emptySlotBorderColor
    }

    onLetterStateChanged: changeState()

    onLetterChanged: {
        if (!flipping) {
            _letter = letter
        }
    }

    function changeState() {
        if (enableLetterFlipAnimation) {
            if (!flipping) {
                if (!_flipAnimation) {
                    _flipAnimation = flipAnimationComponent.createObject(thisItem)
                }
                _flipAnimation.start()
            }
        } else {
            applyChanges()
        }
    }

    function applyChanges() {
        _letterState = letterState
        _letter = letter
    }

    Label {
        anchors.fill: parent
        text: _letter
        // No background in unknown state => use theme primary color
        color: _letterState ? Wordle.textColor : WordleTheme.primaryColor
        font {
            pixelSize: height/2
            capitalization: Font.AllUppercase
            bold: true
        }
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    transform: Rotation {
        id: rotation

        origin {
            x: thisItem.width / 2
            y: thisItem.height / 2
        }

        axis {
            x: 1
            y: 0
            z: 0
        }
    }

    Component {
        id: flipAnimationComponent

        FlipAnimation {
            target: rotation
            onApplyChanges: thisItem.applyChanges()
            onDone: thisItem.applyChanges()
        }
    }
}
