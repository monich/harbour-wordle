import QtQuick 1.1

SequentialAnimation {
    id: flipAnimation

    property variant target
    property string property: "angle"

    signal applyChanges()
    signal done()

    alwaysRunToEnd: true

    NumberAnimation {
        easing.type: Easing.InOutSine
        target: flipAnimation.target
        property: flipAnimation.property
        from: 0
        to: 90
        duration: 250
    }
    ScriptAction { script: flipAnimation.applyChanges() }
    NumberAnimation {
        easing.type: Easing.InOutSine
        target: flipAnimation.target
        property: flipAnimation.property
        to: 0
        duration: 250
    }
    ScriptAction { script: flipAnimation.done() }
}
