import QtQuick 1.1
import harbour.wordle 1.0

Grid {
    id: thisItem

    property int cellSize: Theme.itemSizeMedium
    property alias wordle: repeater.model
    property bool enableLetterFlipAnimation

    signal shakeRow(variant row)
    signal celebrate()

    spacing: Theme.paddingMedium
    columns: Constants.WordLength

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

                /* readonly */ property int rowNumber: Math.floor(model.index / Constants.WordLength)
                property bool wantToJump
                property variant shakeAnimation
                property variant jumpAnimation

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

                    ShakeAnimation {
                        target: slot
                        amplitude: thisItem.spacing
                    }
                }

                Component {
                    id: jumpAnimationComponent

                    WordleJumpAnimation {
                        target: slot
                        amplitude: - 3 * thisItem.spacing
                        startDelay: Math.max((model.index - slot.rowNumber * Constants.WordLength) * 200, 0)
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
                        if (slot.rowNumber === (wordle.fullRows - 1)) {
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
