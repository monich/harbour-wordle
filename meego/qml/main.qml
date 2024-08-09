import QtQuick 1.1
import com.nokia.meego 1.0
import harbour.wordle 1.0

Window {
    id: window

    WordleGame {
        id: game

        language: WordleSettings.language
        playing: gameState === WordleGame.GameInProgress &&
            !HarbourSystemState.locked &&
            !HarbourSystemState.displayOff
    }

    MainPage {
        anchors.fill: parent
        landscape: !window.inPortrait
        wordle: game
    }

    HarbourDisplayBlanking {
        requestInterval: 10
        pauseRequested: Qt.application.active && WordleSettings.keepDisplayOn &&
            game.gameState === WordleGame.GameInProgress
    }
}
