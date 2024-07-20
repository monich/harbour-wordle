import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.wordle 1.0

import "Utils.js" as Utils

Rectangle {
    id: thisItem

    property string letter
    property int letterState: Wordle.LetterStateUnknown
    property bool enableLetterFlipAnimation
    readonly property bool flipping: !!_flipAnimation && _flipAnimation.running

    property string _letter
    property int _letterState: Wordle.LetterStateUnknown
    property var _flipAnimation

    color: Utils.letterBackgroundColor(_letterState)
    border {
        width: (_letterState == Wordle.LetterStateUnknown) ? Math.max(Math.floor(thisItem.width/30),1) : 0
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

    Loader {
        active: _letter.length === 1
        anchors.fill: parent
        sourceComponent: Component {
            Label {
                text: _letter
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
