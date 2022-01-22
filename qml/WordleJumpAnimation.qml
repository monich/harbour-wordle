import QtQuick 2.0
import Sailfish.Silica 1.0

SequentialAnimation  {
    id: animation

    property alias startDelay: pauseAnimation.duration
    property var target
    property real amplitude: Theme.paddingLarge
    property real y: 0

    alwaysRunToEnd: true

    PauseAnimation {
        id: pauseAnimation
    }

    NumberAnimation {
        target: animation.target
        property: "y"
        to: animation.y + animation.amplitude
        duration: 200
        easing.type: Easing.OutQuad
    }

    NumberAnimation {
        target: animation.target
        property: "y"
        from: animation.y + animation.amplitude
        to: animation.y
        duration: 800
        easing.type: Easing.OutBounce
        easing.amplitude:  2
    }
}
