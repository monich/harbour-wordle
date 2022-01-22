import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.wordle 1.0

Rectangle {
    id: thisItem

    property string letter
    property int letterState: Wordle.LetterStateUnknown
    property bool enableLetterFlipAnimation
    readonly property bool flipping: !!_flipAnimation && _flipAnimation.running

    property int _letterState: Wordle.LetterStateUnknown
    property var _flipAnimation

    color: (_letterState == Wordle.LetterStateNotPresent) ? Wordle.notPresentBackgroundColor :
        (_letterState == Wordle.LetterStatePresent) ? Wordle.presentBackgroundColor :
        (_letterState == Wordle.LetterStatePresentHere) ? Wordle.presentHereBackgroundColor :
        Wordle.emptySlotBackgroundColor

    border {
        width: (_letterState == Wordle.LetterStateUnknown) ? Math.max(Math.floor(Theme.paddingSmall/2),1) : 0
        color: Wordle.emptySlotBorderColor
    }

    onLetterStateChanged: changeState()

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
    }

    Loader {
        active: letter.length === 1
        anchors.fill: parent
        sourceComponent: Component {
            Label {
                text: letter
                // No background in unknown state => use theme primary color
                color: (_letterState == Wordle.LetterStateUnknown) ? Theme.primaryColor : Wordle.textColor
                font {
                    pixelSize: height/2
                    capitalization: Font.AllUppercase
                    bold: true
                }
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
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

        WordleFlipAnimation {
            target: rotation
            onApplyChanges: thisItem.applyChanges()
            onDone: thisItem.applyChanges()
        }
    }
}
