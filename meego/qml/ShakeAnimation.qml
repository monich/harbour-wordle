import QtQuick 1.1

SequentialAnimation  {
    id: animation

    property variant target
    property real amplitude: Theme.horizontalPageMargin * 4 / 5
    property real x: 0

    alwaysRunToEnd: true

    NumberAnimation {
        target: animation.target
        property: "x"
        to: animation.x + animation.amplitude
        duration: 25
        easing.type: Easing.InOutSine
    }

    NumberAnimation {
        target: animation.target
        property: "x"
        from: animation.x + animation.amplitude
        to: animation.x
        duration: 500
        easing.type: Easing.OutElastic
        easing.amplitude:  2
        easing.period: 0.2
    }
}
