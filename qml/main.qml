import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.wordle 1.0

ApplicationWindow {
    id: main

    readonly property string title: "WORDLE"

    allowedOrientations: Orientation.Portrait // Orientation.All

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
        pauseRequested: Qt.application.active && WordleSettings.keepDisplayOn
    }

    WordleGame {
        id: game
        language: WordleSettings.language
    }
}
