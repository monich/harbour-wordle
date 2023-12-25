import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.wordle 1.0

ApplicationWindow {
    id: main

    readonly property string title: "WORDLE"

    allowedOrientations: {
        switch (WordleSettings.orientation) {
        default:
        case WordleSettings.OrientationAny: return Orientation.All
        case WordleSettings.OrientationPortrait: return Orientation.Portrait
        case WordleSettings.OrientationLandscape: return Orientation.Landscape
        }
    }

    initialPage: Component {
        MainPage {
            title: main.title
            wordle: game
            allowedOrientations: main.allowedOrientations
        }
    }

    cover: Component {
        CoverPage {
            title: main.title
            wordle: game
        }
    }

    Binding {
        target: Wordle
        property: "darkOnLight"
        value: "colorScheme" in Theme && Theme.colorScheme === Theme.DarkOnLight
    }

    HarbourDisplayBlanking {
        pauseRequested: Qt.application.active && WordleSettings.keepDisplayOn &&
            game.gameState === WordleGame.GameInProgress &&
            (HarbourBattery.batteryState === HarbourBattery.BatteryStateCharging ||
             HarbourBattery.batteryLevel === 0 || // Zero if unknown (not reported by mce)
             HarbourBattery.batteryLevel >= 20)
    }

    WordleGame {
        id: game
        language: WordleSettings.language
        playing: gameState === WordleGame.GameInProgress &&
            !HarbourSystemState.locked &&
            !HarbourSystemState.displayOff
    }
}
