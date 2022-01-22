import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.wordle 1.0

import "harbour"

Grid {
    id: thisItem

    property int cellSize: Theme.itemSizeMedium
    property alias model: repeater.model
    property bool enableLetterFlipAnimation

    signal shakeRow(var row)
    signal celebrate()

    spacing: Theme.paddingMedium
    columns: Wordle.WordLength

    Repeater {
        id: repeater

        delegate: Item {
            width: cellSize
            height: cellSize
            z: model.index

            WordleSlot {
                id: slot

                width: parent.width
                height: parent.height
                letter: model.letter
                letterState: model.state
                enableLetterFlipAnimation: thisItem.enableLetterFlipAnimation

                readonly property int rowNumber: model.index / Wordle.WordLength
                property bool wantToJump
                property var shakeAnimation
                property var jumpAnimation

                onFlippingChanged: {
                    if (!flipping && wantToJump) {
                        wantToJump = false
                        startJumping()
                    }
                }

                function startJumping() {
                    if (!jumpAnimation) {
                        jumpAnimation = jumpAnimationComponent.createObject(slot)
                    }
                    jumpAnimation.start()
                }

                function startShaking() {
                    if (!shakeAnimation) {
                        shakeAnimation = shakeAnimationComponent.createObject(slot)
                    }
                    shakeAnimation.start()
                }

                Component {
                    id: shakeAnimationComponent

                    HarbourShakeAnimation {
                        target: slot
                        amplitude: thisItem.spacing
                    }
                }

                Component {
                    id: jumpAnimationComponent

                    WordleJumpAnimation {
                        target: slot
                        amplitude: - 3 * thisItem.spacing
                        startDelay: Math.max((model.index - slot.rowNumber * Wordle.WordLength) * 200, 0)
                    }
                }

                Connections {
                    target: thisItem
                    onShakeRow: {
                        if (row === slot.rowNumber) {
                            slot.startShaking()
                        }
                    }
                    onCelebrate: {
                        if (slot.rowNumber === (thisItem.model.fullRows - 1)) {
                            if (slot.flipping) {
                                slot.wantToJump = true
                            } else {
                                slot.startJumping()
                            }
                        }
                    }
                }
            }
        }
    }
}
